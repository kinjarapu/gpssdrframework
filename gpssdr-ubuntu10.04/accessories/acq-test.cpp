/* test acquisition
 * written by: Dimitrios Symeonidis
 * dimitrios.symeonidis@ext.jrc.ec.europa.eu */

#define GLOBALS_HERE

#include "acq-test.h"

int main(){
	InitAcquisition();

	fp = fopen(SAMPFILE, "rb");
	if(fp == NULL){
		fprintf(stdout, "File %s not found!\n", SAMPFILE);
		exit(-1);
	}
	else
		fprintf(stdout, "Reading from file %s\n", SAMPFILE);

	//Get file length
	fstat(fileno(fp), &buf);
	file_size = buf.st_size;

	total_read = 0;
	for(int i=0; i<2; i++){
		Read_File();
		doPrepIF();
		for(int32 lcv=0; lcv<MAX_SV; lcv++) doAcqStrong(lcv, i);
	}
	Discriminator();

	fclose(fp);
	exit(0);
}


/*----------------------------------------------------------------------------------------------*/
void Read_File()
{
	int32 bytes_read, lcv;
	data_size = SAMPS_MS*sizeof(CPX);
	int32 DESIRED_MAX = 128;

	if((total_read+data_size)>file_size){
		fprintf(stdout, "End of file, quitting\n");
		exit(1);
	}
	else{
		bytes_read = fread(&buff[0], 1, data_size, fp);
		total_read += bytes_read;

		/* only do this for the first millisecond read */
		if(samples_range==0){
			x86_cmax(buff, &samples_range, bytes_read);
			if(samples_range>DESIRED_MAX){
				scaling = ceil(samples_range/DESIRED_MAX);
				fprintf(stdout, "dividing by %i ", scaling);
			}
			else{
				scaling = floor(DESIRED_MAX/samples_range);
				fprintf(stdout, "multiplying with %i ", scaling);
			}
			fprintf(stdout, "to scale to [-%i,%i]\n", DESIRED_MAX, DESIRED_MAX);
		}

		if(samples_range>DESIRED_MAX){
			for(lcv=0; lcv<bytes_read; lcv++){
				buff[lcv].i = ceil(buff[lcv].i/scaling);
				buff[lcv].q = ceil(buff[lcv].q/scaling);
			}
		}
		else{
			for(lcv=0; lcv<bytes_read; lcv++){
				buff[lcv].i *= scaling;
				buff[lcv].q *= scaling;
			}
		}

		usleep(1000);
	}
}
/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/
void InitAcquisition()
{

	int32 lcv, lcv2;
	CPX *p;
	int32 R1[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int32 R2[16] = {0,0,0,0,0,0,0,1,0,1,0,1,1,1,1,1};
	samples_range = 0;

	/* Step one, grab pre-fftd codes from header file */
	for(lcv = 0; lcv < MAX_SV; lcv++)
		fft_codes[lcv] = (CPX *)&PRN_Codes[2*lcv*SAMPS_MS];

	/* Allocate some buffers that will be used later on */
	buff	 = new CPX[310 * SAMPS_MS];
	msbuff   = new CPX[SAMPS_MS];
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

	/* Generate mix to baseband */
	sine_gen(_000Hzwipeoff, -IF_FREQ, SAMPLE_FREQUENCY, 10*SAMPS_MS);

	/* Generate 250 Hz offset wipeoff */
	sine_gen(_250Hzwipeoff, -IF_FREQ-250.0, SAMPLE_FREQUENCY, 10*SAMPS_MS);
	sine_gen(_500Hzwipeoff, -IF_FREQ-500.0, SAMPLE_FREQUENCY, 10*SAMPS_MS);
	sine_gen(_750Hzwipeoff, -IF_FREQ-750.0, SAMPLE_FREQUENCY, 10*SAMPS_MS);

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
 * doPrepIF: Complete all of the upfront IF processing, includes: reampling the buffer, mixing to baseband, 25-Hz, 500 Hz, and 750 Hz, computing the forward FFT, then
 * copying the FFTd data into a 2-D matrix. The matrix is created in such a way to allow the circular-rotation trick to be carried out without repeatedly calling "doRotate"
 * */
void doPrepIF()
{

	int32 lcv, ms;
	CPX *p;

	ms = 1;

	/* 1) Import data */
	memcpy(baseband, buff, ms*SAMPS_MS*sizeof(CPX));

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
void doAcqStrong(int32 _sv, int32 _which)
{
	int32 lcv, lcv2, mag, magt, index, indext;

	index = indext = mag = magt = 0;

	for(lcv = (DOPPMIN/1000); lcv <  (DOPPMAX/1000); lcv++) /* Sweep through the doppler range */
	{
		for(lcv2 = 0; lcv2 < 4; lcv2 ++) /* Covers the 250 Hz spacing */
		{
			/* Multiply in frequency domain, shifting appropriately */
			sse_cmulsc(&baseband_rows[lcv2][100+lcv], fft_codes[_sv], msbuff, SAMPS_MS, 10);

			/* Compute iFFT */
			piFFT->doiFFT(msbuff, true);

			/* Convert to a power */
			x86_cmag(msbuff, SAMPS_MS);

			/* Find the maximum */
			x86_max((int32 *)msbuff, &indext, &magt, SAMPS_MS);

			if(magt > mag)
			/* Found a new maximum */
			{
				mag = magt;
				index = indext;
				results[_which][_sv].code_phase = 2048 - index;
				results[_which][_sv].doppler = (lcv*1000) + (float)lcv2*250;
				results[_which][_sv].magnitude = mag;
			}
		}
	}

	/* compare 1st to 2nd peak to discriminate acquisition */
	int32 SAMPS_PER_CHIP = ceil(SAMPS_MS/CODE_CHIPS);
	int32 start_clear = index - SAMPS_PER_CHIP;
	int32 end_clear = index + SAMPS_PER_CHIP;

	for(lcv = start_clear; lcv < end_clear; lcv++){
		msbuff[lcv % SAMPS_MS].i = 0;
		msbuff[lcv % SAMPS_MS].q = 0;
	}
	x86_max((int32 *)msbuff, &indext, &magt, SAMPS_MS);
	results[_which][_sv].relmagn = (float)results[_which][_sv].magnitude / magt;
	//fprintf(stdout, "1st peak at %i, 2nd peak at %i\n", index, indext);
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void Discriminator()
{
	int lcv, which;
	/* calculate threshold */
	float mean = 0;
	for(lcv=0; lcv<MAX_SV; lcv++){
		which = (results[0][lcv].magnitude > results[1][lcv].magnitude ? 0 : 1);
		mean += results[which][lcv].relmagn / MAX_SV;
	}
	//REL_THRESHOLD = mean * 1.25;
	REL_THRESHOLD = 2.4;
	fprintf(stdout, "mean relative magnitude for %i SVs: %f, new threshold: %f\n", MAX_SV, mean, REL_THRESHOLD);

	for(lcv=0; lcv<MAX_SV; lcv++){
		/* for each SV choose the millisecond with the highest peak */
		which = (results[0][lcv].magnitude > results[1][lcv].magnitude ? 0 : 1);
		
		if(results[which][lcv].relmagn > REL_THRESHOLD){
			success[lcv] = which;
			//fprintf(stdout, "acq success for sv #%2i (ms %i) at doppler %i and codephase %i, relative magnitude %'f\n",
				//lcv+1, which+1, results[which][lcv].doppler, results[which][lcv].code_phase, results[which][lcv].relmagn);
		}
		else{
			success[lcv] = -1;
			//fprintf(stdout, "no acq for sv #%2i (ms %i), relative magnitude %f\n", lcv+1, which+1, results[which][lcv].relmagn);
		}
		fprintf(stdout, "%f\n", results[which][lcv].relmagn);
	}

	PrintAcq();

	//fprintf(stdout, "**********************************************************************\n");
}
/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/
void PrintAcq()
{
	fprintf(stdout, "(");

	for(int lcv=0; lcv<MAX_SV; lcv++){
		if(success[lcv] == -1){
			fprintf(stdout, ".. ");
		}
		else{
			fprintf(stdout, "%02i ", lcv+1);
		}
	}
	fprintf(stdout,")\n");
}
/*----------------------------------------------------------------------------------------------*/
