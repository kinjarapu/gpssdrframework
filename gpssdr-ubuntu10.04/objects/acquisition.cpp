/*----------------------------------------------------------------------------------------------*/
/*! \file acquisition.cpp
//
// FILENAME: acquisition.cpp
//
// DESCRIPTION: Implements member functions of the Acquisition class.
//
// DEVELOPERS: Gregory W. Heckler (2003-2009)
//
// LICENSE TERMS: Copyright (c) Gregory W. Heckler 2009
//
// This file is part of the GPS Software Defined Radio (GPS-SDR)
//
// The GPS-SDR is free software; you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version. The GPS-SDR is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// Note:  Comments within this file follow a syntax that is compatible with
//        DOXYGEN and are utilized for automated document extraction
//
// Reference:
*/
/*----------------------------------------------------------------------------------------------*/

#include "acquisition.h"
#include "prn_codes.h"	//!< Include the pre-fftd PRN codes (done with MATLAB)

/*----------------------------------------------------------------------------------------------*/
void *Acquisition_Thread(void *_arg)
{
	Acquisition *aAcquisition = pAcquisition;

	while(grun)
	{
		aAcquisition->Import();
		aAcquisition->Acquire();
		aAcquisition->Export();
		aAcquisition->IncExecTic();
	}

	pthread_exit(0);
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void Acquisition::Start()
{

	Start_Thread(Acquisition_Thread, NULL);

	if(gopt.verbose)
		fprintf(stdout,"Acquisition thread started\n");
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
/*!
 * Acquisition(): Constructor
 * */
Acquisition::Acquisition():Threaded_Object("ACQTASK")
{

	int32_t lcv, lcv2;
	CPX *p;
	int32_t R1[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int32_t R2[16] = {0,0,0,0,0,0,0,1,0,1,0,1,1,1,1,1};

	/* Acq state */
	sv = 0;
	state = ACQ_TYPE_STRONG;
	
	REL_THRESHOLD = 2.5;

	/* Step one, grab pre-fftd codes from header file */
	for(lcv = 0; lcv < MAX_SV; lcv++)
		fft_codes[lcv] = (CPX *)&PRN_Codes[2*lcv*SAMPS_MS];

	/* Allocate some buffers that will be used later on */
	buff	 = new CPX[310 * SAMPS_MS];
	msbuff	 = new CPX[SAMPS_MS];
	cmags	 = new int32_t[SAMPS_MS];
	power	 = new CPX[10 * SAMPS_MS];
	coherent = new CPX[10 * SAMPS_MS];
	baseband = new CPX[4 * 310 * SAMPS_MS];
	_000Hzwipeoff = new CPX[310 * SAMPS_MS];
	_250Hzwipeoff = new CPX[310 * SAMPS_MS];
	_500Hzwipeoff = new CPX[310 * SAMPS_MS];
	_750Hzwipeoff = new CPX[310 * SAMPS_MS];

	/* Allocate baseband shift vector and map of the row pointers */
	baseband_shift = new CPX[4 * 310 * (SAMPS_MS+201)];
	baseband_rows = new CPX *[1240];
	for(lcv = 0; lcv < 1240; lcv++)
		baseband_rows[lcv] = &baseband_shift[lcv*(SAMPS_MS+201)];

	/* Allocate baseband shift vector and map of the row pointers */
	dft = new MIX[10*10];
	dft_rows = new MIX *[10];
	for(lcv = 0; lcv < 10; lcv++)
		dft_rows[lcv] = &dft[lcv*10];

	/* Generate sinusoid */
	for(lcv = 0; lcv < 10; lcv++)
		wipeoff_gen(dft_rows[lcv], (float)lcv*25.0 - 112.5, 1000.0, 10);

	/* Generate mix to baseband */
	sine_gen(_000Hzwipeoff, -IF_FREQUENCY, SAMPLE_FREQUENCY, 10*SAMPS_MS);

	/* Generate 250 Hz offset wipeoff */
	sine_gen(_250Hzwipeoff, -IF_FREQUENCY-250.0, SAMPLE_FREQUENCY, 10*SAMPS_MS);
	sine_gen(_500Hzwipeoff, -IF_FREQUENCY-500.0, SAMPLE_FREQUENCY, 10*SAMPS_MS);
	sine_gen(_750Hzwipeoff, -IF_FREQUENCY-750.0, SAMPLE_FREQUENCY, 10*SAMPS_MS);

	/* Copy to all 310 ms */
	for(lcv = 1; lcv < 31; lcv++)
	{
		memcpy(&_000Hzwipeoff[lcv*10*SAMPS_MS],_000Hzwipeoff,10*SAMPS_MS*sizeof(CPX));
		memcpy(&_250Hzwipeoff[lcv*10*SAMPS_MS],_250Hzwipeoff,10*SAMPS_MS*sizeof(CPX));
		memcpy(&_500Hzwipeoff[lcv*10*SAMPS_MS],_500Hzwipeoff,10*SAMPS_MS*sizeof(CPX));
		memcpy(&_750Hzwipeoff[lcv*10*SAMPS_MS],_750Hzwipeoff,10*SAMPS_MS*sizeof(CPX));
	}

	/* Allocate the FFTs */
	pFFT = new FFT(SAMPS_MS, R1);
	piFFT = new FFT(SAMPS_MS, R2);

	if(gopt.verbose)
		fprintf(stdout,"Creating Acquisition\n");
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
/*!
 * ~Acquisition(): Deconstructor
 * */
Acquisition::~Acquisition()
{

	int32_t lcv;

	delete pFFT;
	delete piFFT;

	delete [] buff;
	delete [] msbuff;
	delete [] cmags;
	delete [] baseband;
	delete [] baseband_shift;
	delete [] baseband_rows;
	delete [] coherent;
	delete [] power;
	delete [] dft;
	delete [] dft_rows;
	delete [] _000Hzwipeoff;
	delete [] _250Hzwipeoff;
	delete [] _500Hzwipeoff;
	delete [] _750Hzwipeoff;

	if(gopt.verbose)
		fprintf(stdout,"Destructing Acquisition\n");

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
/*!
 * doPrepIF: Complete all of the upfront IF processing, includes: reampling the buffer, mixiing to baseband, 25-Hz, 500 Hz, and 750 Hz, computing the forward FFT, then
 * copying the FFTd data into a 2-D matrix. The matrix is created in such a way to allow the circular-rotation trick to be carried out without repeatedly calling "doRotate"
 * */
void Acquisition::doPrepIF(int32_t _type, CPX *_buff)
{

	int32_t lcv, ms;
	CPX *p;

	switch(_type)
	{
		case 0:
			ms = 1;
			break;
		case 1:
			ms = 10;
			break;
		case 2:
			ms = 310;
			break;
		default:
			ms = 1;
	}

	/* 1) Import data */
	memcpy(baseband, _buff, ms*SAMPS_MS*sizeof(CPX));

	/* Do the 250 Hz offsets */
	sse_cmulsc(&baseband[0], _250Hzwipeoff, &baseband[ms*SAMPS_MS],   ms*SAMPS_MS, 14);
	sse_cmulsc(&baseband[0], _500Hzwipeoff, &baseband[2*ms*SAMPS_MS], ms*SAMPS_MS, 14);
	sse_cmulsc(&baseband[0], _750Hzwipeoff, &baseband[3*ms*SAMPS_MS], ms*SAMPS_MS, 14);

	/* Mix down to baseband */
	sse_cmuls(baseband, _000Hzwipeoff, ms*SAMPS_MS, 14);

	/* Compute forward FFT of IF data */
	for(lcv = 0; lcv < 4*ms; lcv++)
		pFFT->doFFT(&baseband[lcv*SAMPS_MS], true);

	/* Now copy into the rows */
	for(lcv = 0; lcv < 4*ms; lcv++)
	{
		p = baseband_rows[lcv];
		memcpy(p, 				 &baseband[(lcv+1)*SAMPS_MS-100], 100*sizeof(CPX));
		memcpy(p+100,	 		 &baseband[lcv*SAMPS_MS],			SAMPS_MS*sizeof(CPX));
		memcpy(p+100+SAMPS_MS, &baseband[lcv*SAMPS_MS],			100*sizeof(CPX));
	}

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
/*!
 * doAcqStrong: Acquire using a 1 ms coherent integration
 * */
void Acquisition::doAcqStrong(int32_t _sv, int32_t _doppmin, int32_t _doppmax, int32 _which)
{
	int32_t lcv, lcv2, mag, magt, index, indext;
	index = indext = mag = magt = 0;

	/* Sweep through the doppler range */
	for(lcv = (_doppmin/1000); lcv <  (_doppmax/1000); lcv++)
	{
		/* Covers the 250 Hz spacing */
		for(lcv2 = 0; lcv2 < 4; lcv2 ++)
		{
			//if(gopt.realtime)
				//usleep(1000);

			/* Multiply in frequency domain, shifting appropriately */
			sse_cmulsc(&baseband_rows[lcv2][100+lcv], fft_codes[_sv], msbuff, SAMPS_MS, 10);

			/* Compute iFFT */
			piFFT->doiFFT(msbuff, true);

			/* Convert to a power */
			x86_cmag_new(msbuff, cmags, SAMPS_MS);

			/* Find the maximum */
			x86_max(cmags, &indext, &magt, SAMPS_MS);

			/* Found a new maximum */
			if(magt > mag)
			{
				mag = magt;
				index = indext;
				acqstrong[_which].code_phase = 2048 - index;
				acqstrong[_which].doppler = (lcv*1000) + (float)lcv2*250;
				acqstrong[_which].magnitude = mag;
			}
		}
	}

	/* compare 1st to 2nd peak to discriminate acquisition */
	int32_t SAMPS_PER_CHIP = ceil(SAMPS_MS/CODE_CHIPS);
	int32_t start_clear = index - SAMPS_PER_CHIP;
	int32_t end_clear = index + SAMPS_PER_CHIP;

	for(lcv = start_clear; lcv < end_clear; lcv++)
		cmags[lcv % SAMPS_MS] = 0;
	x86_max(cmags, &indext, &magt, SAMPS_MS);
	acqstrong[_which].relmagn = (double_t)acqstrong[_which].magnitude / magt;
	//fprintf(stdout, "1st peak at %i, 2nd peak at %i\n", index, indext);
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void Acquisition::Discriminator(int32_t _sv)
{
	/* calculate threshold */
	//float mean = 0;
	//for(lcv=0; lcv<MAX_SV; lcv++){
		//which = (acqstrong[0].magnitude > acqstrong[1].magnitude ? 0 : 1);
		//mean += acqstrong[which].relmagn / MAX_SV;
	//}
	//REL_THRESHOLD = mean * 1.25;
	//fprintf(stdout, "mean relative magnitude for %i SVs: %f, new threshold: %f\n", MAX_SV, mean, REL_THRESHOLD);
	REL_THRESHOLD = 2.4;

	/* choose the millisecond with the highest peak */
	int which = (acqstrong[0].magnitude > acqstrong[1].magnitude ? 0 : 1);

	if(acqstrong[which].relmagn > REL_THRESHOLD){
		results[_sv].success = 1;
		results[_sv].doppler = acqstrong[which].doppler;
		results[_sv].code_phase = acqstrong[which].code_phase;
		results[_sv].magnitude = acqstrong[which].magnitude;
		results[_sv].relmagn = acqstrong[which].relmagn;
		results[_sv].sv = _sv;
		results[_sv].type = ACQ_TYPE_STRONG;
		fprintf(stdout, "acq success for sv #%2i (ms %i) at doppler %i and codephase %i, relative magnitude %Lf\n",
			_sv+1, which+1, results[_sv].doppler, results[_sv].code_phase, results[_sv].relmagn);
	}
	else{
		results[_sv].success = 0;
		results[_sv].magnitude = acqstrong[which].magnitude;
		results[_sv].relmagn = acqstrong[which].relmagn;
		fprintf(stdout, "no acq for sv #%2i (ms %i), relative magnitude %Lf\n", _sv+1, which+1, results[_sv].relmagn);
	}
	//fprintf(stdout, "%f\n", results[which][lcv].relmagn);

	if(_sv==31){
		fprintf(stdout, "(");
		for(int lcv=0; lcv<MAX_SV; lcv++){
			if(results[lcv].success == 1) fprintf(stdout, "%02i ", lcv+1);
			else fprintf(stdout, ".. ");
		}
		fprintf(stdout,")\n");
		fprintf(stdout, "**********************************************************************\n");
	}
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
/*!
 * doAcqMedium: Acquire using a 10 ms coherent integration
 * */
void Acquisition::doAcqMedium(int32_t _sv, int32_t _doppmin, int32_t _doppmax)
{
	Acq_Command_S *result;
	int32_t lcv, lcv2, lcv3, mag, magt, index, indext, j, k, dopp, skip;
	int32_t iaccum, qaccum;
	CPX temp[10];
	int32_t data[32];
	CPX *dp = (CPX *)&data[0];
	int32_t *dt = (int32_t *)&temp[0];
	int32_t *p;

	result = &results[_sv];
	index = indext = mag = magt = 0;

	/* Sweeps through the doppler range */
	for(lcv = (_doppmin/1000); lcv <=  (_doppmax/1000); lcv++)
	{
		/* Covers the 250 Hz spacing */
		for(lcv2 = 0; lcv2 < 4; lcv2++)
		{
			/* Do both even and odd */
			//for(k = 0; k < 2; k++)
			k = 0;
			{

				//if(gopt.realtime)
					//usleep(1000);

				/* Do the 10 ms of coherent integration */
				for(lcv3 = 0; lcv3 < 10; lcv3++)
				{
					/* Multiply in frequency domain, shifting appropiately */
					sse_cmulsc(&baseband_rows[lcv2*20 + lcv3 + k*10][100+lcv], fft_codes[_sv], &coherent[lcv3*SAMPS_MS], SAMPS_MS, 10);

					/* Compute iFFT */
					piFFT->doiFFT(&coherent[lcv3*SAMPS_MS], true);
				}

				/* For each delay do the post-corr FFT, this REALLY needs sped up */
				for(lcv3 = 0; lcv3 < SAMPS_MS; lcv3++)
				{
					/* Copy over the relevant data pts */
					p = (int32_t *)&coherent[lcv3];
					data[0] = *p; p += SAMPS_MS;
					data[1] = *p; p += SAMPS_MS;
					data[2] = *p; p += SAMPS_MS;
					data[3] = *p; p += SAMPS_MS;
					data[4] = *p; p += SAMPS_MS;
					data[5] = *p; p += SAMPS_MS;
					data[6] = *p; p += SAMPS_MS;
					data[7] = *p; p += SAMPS_MS;
					data[8] = *p; p += SAMPS_MS;
					data[9] = *p;

					/* Do the post-correlation dft */
					sse_cacc(dp, dft_rows[0], 10, &iaccum, &qaccum); temp[0].i = iaccum >> 16; temp[0].q = qaccum >> 16;
					sse_cacc(dp, dft_rows[1], 10, &iaccum, &qaccum); temp[1].i = iaccum >> 16; temp[1].q = qaccum >> 16;
					sse_cacc(dp, dft_rows[2], 10, &iaccum, &qaccum); temp[2].i = iaccum >> 16; temp[2].q = qaccum >> 16;
					sse_cacc(dp, dft_rows[3], 10, &iaccum, &qaccum); temp[3].i = iaccum >> 16; temp[3].q = qaccum >> 16;
					sse_cacc(dp, dft_rows[4], 10, &iaccum, &qaccum); temp[4].i = iaccum >> 16; temp[4].q = qaccum >> 16;
					sse_cacc(dp, dft_rows[5], 10, &iaccum, &qaccum); temp[5].i = iaccum >> 16; temp[5].q = qaccum >> 16;
					sse_cacc(dp, dft_rows[6], 10, &iaccum, &qaccum); temp[6].i = iaccum >> 16; temp[6].q = qaccum >> 16;
					sse_cacc(dp, dft_rows[7], 10, &iaccum, &qaccum); temp[7].i = iaccum >> 16; temp[7].q = qaccum >> 16;
					sse_cacc(dp, dft_rows[8], 10, &iaccum, &qaccum); temp[8].i = iaccum >> 16; temp[8].q = qaccum >> 16;
					sse_cacc(dp, dft_rows[9], 10, &iaccum, &qaccum); temp[9].i = iaccum >> 16; temp[9].q = qaccum >> 16;

					/* Put into the power matrix */
					p = (int32_t *)&power[lcv3];
					*p = dt[0]; p += SAMPS_MS;
					*p = dt[1]; p += SAMPS_MS;
					*p = dt[2]; p += SAMPS_MS;
					*p = dt[3]; p += SAMPS_MS;
					*p = dt[4]; p += SAMPS_MS;
					*p = dt[5]; p += SAMPS_MS;
					*p = dt[6]; p += SAMPS_MS;
					*p = dt[7]; p += SAMPS_MS;
					*p = dt[8]; p += SAMPS_MS;
					*p = dt[9];

				}

				/* Convert to a power */
				x86_cmag(&power[0], 10*SAMPS_MS);

				/* Find the maximum */
				x86_max((int32_t *)power, &indext, &magt, 10*SAMPS_MS);

				/* Found a new maximum */
				if(magt > mag)
				{
					mag = magt;
					index = indext % SAMPS_MS;
					result->code_phase = index;
					result->doppler = (lcv*1000) + (lcv2*250) + (indext/SAMPS_MS)*25.0;
					result->magnitude = mag;
				}

			}//end k

		}//end lcv2

	}//end lcv


	result->sv = _sv;

	result->type = ACQ_TYPE_MEDIUM;

	if(result->magnitude > THRESH_MEDIUM)
		result->success = 1;
	else
		result->success = 0;
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
/*!
 * doAcqWeak: Acquire using a 10 ms coherent integration and 15 incoherent integrations
 * */
void Acquisition::doAcqWeak(int32_t _sv, int32_t _doppmin, int32_t _doppmax)
{

	Acq_Command_S *result;
	int32_t lcv, lcv2, lcv3, mag, magt, index, indext, k, i, j, skip, dopp;
	int32_t iaccum, qaccum;
	int32_t data[32];
	CPX *dp = (CPX *)&data[0];
	CPX temp[10];
	int32_t *dt = (int32_t *)&temp[0];
	int32_t *p;
	double code_doppler;
	double doppler;
	int32_t shift;

	result = &results[_sv];
	index = indext = mag = magt = 0;

	/* Sweeps through the doppler range */
	for(lcv = (_doppmin/1000); lcv <  (_doppmax/1000); lcv++)
	{

		/* Covers the 250 Hz spacing */
		for(lcv2 = 0; lcv2 < 4; lcv2++)
		{
			/* Do both even and odd */
			for(k = 0; k < 2; k++)
			{

				/* Clear out incoherent int */
				memset(power, 0x0, 10*SAMPS_MS*sizeof(CPX));

				/* Loop over 15 incoherent integrations */
				for(i = 0; i < 15; i++)
				{

					if(gopt.realtime)
						usleep(1000);

					/* Do the 10 ms of coherent integration */
					for(lcv3 = 0; lcv3 < 10; lcv3++)
					{
						/* Multiply in frequency domain, shifting appropiately */
						sse_cmulsc(&baseband_rows[lcv2*310 + lcv3 + i*20 + k*10][100+lcv], fft_codes[_sv], &coherent[lcv3*SAMPS_MS], SAMPS_MS, 9);

						/* Compute iFFT */
						piFFT->doiFFT(&coherent[lcv3*SAMPS_MS], true);
					}

					/* Calculate the frquency doppler */
					doppler = (double)(lcv*1000) + (float)(lcv2*250);

					/* Calculate shift in samples */
					code_doppler = (double)i*.02*IF_SAMPLE_FREQUENCY*doppler/L1;

					/* Make an integer */
					shift = (int32_t)floor(code_doppler);

					/* For each delay do the post-corr FFT, this REALLY needs sped up */
					for(lcv3 = 0; lcv3 < SAMPS_MS; lcv3++)
					{
						/* Copy over the relevant data pts */
						p = (int32_t *)&coherent[lcv3];
						data[0] = *p; p += SAMPS_MS;
						data[1] = *p; p += SAMPS_MS;
						data[2] = *p; p += SAMPS_MS;
						data[3] = *p; p += SAMPS_MS;
						data[4] = *p; p += SAMPS_MS;
						data[5] = *p; p += SAMPS_MS;
						data[6] = *p; p += SAMPS_MS;
						data[7] = *p; p += SAMPS_MS;
						data[8] = *p; p += SAMPS_MS;
						data[9] = *p;

						/* Do the post-correlation dft */
						sse_cacc(dp, dft_rows[0], 10, &iaccum, &qaccum); temp[0].i = iaccum >> 16; temp[0].q = qaccum >> 16;
						sse_cacc(dp, dft_rows[1], 10, &iaccum, &qaccum); temp[1].i = iaccum >> 16; temp[1].q = qaccum >> 16;
						sse_cacc(dp, dft_rows[2], 10, &iaccum, &qaccum); temp[2].i = iaccum >> 16; temp[2].q = qaccum >> 16;
						sse_cacc(dp, dft_rows[3], 10, &iaccum, &qaccum); temp[3].i = iaccum >> 16; temp[3].q = qaccum >> 16;
						sse_cacc(dp, dft_rows[4], 10, &iaccum, &qaccum); temp[4].i = iaccum >> 16; temp[4].q = qaccum >> 16;
						sse_cacc(dp, dft_rows[5], 10, &iaccum, &qaccum); temp[5].i = iaccum >> 16; temp[5].q = qaccum >> 16;
						sse_cacc(dp, dft_rows[6], 10, &iaccum, &qaccum); temp[6].i = iaccum >> 16; temp[6].q = qaccum >> 16;
						sse_cacc(dp, dft_rows[7], 10, &iaccum, &qaccum); temp[7].i = iaccum >> 16; temp[7].q = qaccum >> 16;
						sse_cacc(dp, dft_rows[8], 10, &iaccum, &qaccum); temp[8].i = iaccum >> 16; temp[8].q = qaccum >> 16;
						sse_cacc(dp, dft_rows[9], 10, &iaccum, &qaccum); temp[9].i = iaccum >> 16; temp[9].q = qaccum >> 16;

						//sse_dft(dp, dft_rows[0], &temp[0]);

						x86_cmag(&temp[0], 10);

						/* Accumulate into the power matrix */
						p = (int32_t *)&power[(lcv3 + shift + SAMPS_MS) % SAMPS_MS];
						*p += dt[0]; p += SAMPS_MS;
						*p += dt[1]; p += SAMPS_MS;
						*p += dt[2]; p += SAMPS_MS;
						*p += dt[3]; p += SAMPS_MS;
						*p += dt[4]; p += SAMPS_MS;
						*p += dt[5]; p += SAMPS_MS;
						*p += dt[6]; p += SAMPS_MS;
						*p += dt[7]; p += SAMPS_MS;
						*p += dt[8]; p += SAMPS_MS;
						*p += dt[9];
					}

				}//end i

				/* Find the maximum */
				x86_max((int32_t *)power, &indext, &magt, 10*SAMPS_MS);

				/* Found a new maximum */
				if(magt > mag)
				{
					mag = magt;
					index = indext % SAMPS_MS;
					result->code_phase = index;
					result->doppler = (lcv*1000) + (lcv2*250) + (indext/SAMPS_MS)*25.0;
					result->magnitude = mag;
				}

			}//end k

		}//end lcv2

	}//end lcv

	result->sv = _sv;

	result->type = ACQ_TYPE_WEAK;

	if(result->magnitude > THRESH_WEAK)
		result->success = 1;
	else
		result->success = 0;
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
/*!
 *
 * */
void Acquisition::Acquire()
{
	IncStartTic();

	switch(request.type)
	{
		case ACQ_TYPE_STRONG:
			/* 1st millisecond */
			doPrepIF(ACQ_TYPE_STRONG, buff);
			doAcqStrong(request.sv, request.mindopp, request.maxdopp, 0);
			/* 2nd millisecond */
			doPrepIF(ACQ_TYPE_STRONG, &buff[SAMPS_MS]);
			doAcqStrong(request.sv, request.mindopp, request.maxdopp, 1);
			Discriminator(request.sv);
			break;
		case ACQ_TYPE_MEDIUM:
			doPrepIF(ACQ_TYPE_MEDIUM, buff);
			doAcqMedium(request.sv, request.mindopp, request.maxdopp);
			break;
		case ACQ_TYPE_WEAK:
			doPrepIF(ACQ_TYPE_WEAK, buff);
			doAcqWeak(request.sv, request.mindopp, request.maxdopp);
			break;
		default:
			fprintf(stderr, "invalid request type received: %i\n", request.type);
	}

	IncStopTic();
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
/*!
 * Import:
 * */
void Acquisition::Import()
{
	int32_t last;
	int32_t bread;
	int32_t lastcount;
	int32_t ms;
	int32_t ms_per_read;
	int32_t lcv;
	timespec ret;

	ret.tv_sec = 0;
	ret.tv_nsec = 100000;

	/* First wait for a request */
	bread = read(SVS_2_ACQ_P[READ], &request, sizeof(Acq_Command_S));
	memcpy(&results[request.sv],&request,sizeof(Acq_Command_S));

	switch(request.type)
	{
		case ACQ_TYPE_STRONG:
			ms_per_read = 2;
			break;
		case ACQ_TYPE_MEDIUM:
			ms_per_read = 10;
			break;
		case ACQ_TYPE_WEAK:
			ms_per_read = 310;
			break;
		default:
			ms_per_read = 2;
	}

	/* Flush the pipe */
	for(lcv = 0; lcv < 10; lcv++)
		read(FIFO_2_ACQ_P[READ], &packet, sizeof(ms_packet));

	/* Collect necessary data */
	lastcount = 0; ms = 0;
	while((ms < ms_per_read) && grun)
	{
		/* Get the tail */
		last = packet.count;

		/* Read a packet in */
		read(FIFO_2_ACQ_P[READ], &packet, sizeof(ms_packet));
		memcpy(&buff[SAMPS_MS*ms], &packet.data, SAMPS_MS*sizeof(CPX));

		/* Detect broken packets */
		if(ms > 0)
		{
			if((packet.count - lastcount) != 1)
			{
				fprintf(stdout,"Broken GPS stream %d,%d\n",packet.count,lastcount);
				ms = 0; /* Recollect data */
			}
		}
		else
			request.count = packet.count;

		ms++;
		lastcount = packet.count;
	}
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
/*!
 * Export:
 * */
void Acquisition::Export()
{
	int32_t lcv;
	FILE *fp;
	Acq_Command_S *p;

	/* Write result to the tracking task */
	results[request.sv].count = request.count;
	write(ACQ_2_SVS_P[WRITE], &results[request.sv], sizeof(Acq_Command_S));
}
/*----------------------------------------------------------------------------------------------*/
