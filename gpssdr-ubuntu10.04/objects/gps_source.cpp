/*----------------------------------------------------------------------------------------------*/
/*! \file gps_source.cpp
//
// FILENAME: gps_source.cpp
//
// DESCRIPTION: Implements member functions of the GPS_Source class.
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
// Note:	Comments within this file follow a syntax that is compatible with
//			DOXYGEN and are utilized for automated document extraction
//
// Reference:
*/
/*----------------------------------------------------------------------------------------------*/


#include "gps_source.h"


/*----------------------------------------------------------------------------------------------*/
GPS_Source::GPS_Source(Options_S *_opt)
{
	memcpy(&opt, _opt, sizeof(Options_S));

	switch(opt.source)
	{
		case SOURCE_USRP_V1:
			source_type = SOURCE_USRP_V1;
			Open_USRP_V1();
			break;
		case SOURCE_USRP_V2:
//			source_type = SOURCE_USRP_V2;
//			Open_USRP_V2();
			break;
		case SOURCE_SIGE_GN3S:
			source_type = SOURCE_SIGE_GN3S;
			Open_GN3S();
			break;
		case SOURCE_NSL_PRIMO :
			source_type = SOURCE_NSL_PRIMO;
			Open_Primo();
			break;
		case SOURCE_DISK_FILE:
			source_type = SOURCE_DISK_FILE;
			Open_File();
			break;
		default:
			source_type = SOURCE_USRP_V1;
			Open_USRP_V1();
			break;
	}

	overflw = soverflw = 0;
	agc_scale = 1;

	/* Assign to base */
	buff_out_p = &buff_out[0];
	ms_count = 0;

	if(opt.verbose)
		fprintf(stdout,"Creating GPS Source\n");

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
GPS_Source::~GPS_Source()
{

	switch(source_type)
	{
		case SOURCE_USRP_V1:
			Close_USRP_V1();
			break;
		case SOURCE_USRP_V2:
//			Close_SOURCE_USRP_V2();
			break;
		case SOURCE_SIGE_GN3S:
			Close_GN3S();
			break;
		case SOURCE_NSL_PRIMO :
			Close_Primo();
			break;
		case SOURCE_DISK_FILE:
			Close_File();
			break;
		default:
			Close_USRP_V1();
			break;
	}

	if(opt.verbose)
		fprintf(stdout,"Destructing GPS Source\n");
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void GPS_Source::Read(ms_packet *_p)
{

	double gain;
	int32_t shift;

	switch(source_type)
	{
		case SOURCE_USRP_V1:
			Read_USRP_V1(_p);

			/* Count the overflows and shift if needed */
			soverflw += run_agc(&_p->data[0], SAMPS_MS, AGC_BITS, 6);

			/* Figure out the rxa_agc_scale value */
			if((ms_count & 0xFF) == 0)
			{
				gain = dbs_rx_a->rf_gain();
				if(soverflw > OVERFLOW_HIGH)
					gain -= 0.5;
				if(soverflw < OVERFLOW_LOW)
					gain += 0.5;
				dbs_rx_a->rf_gain(gain);
				agc_scale = (int32)floor(2.0*(dbs_rx_a->max_rf_gain() - gain));
				overflw = soverflw;
				soverflw = 0;
			}
                 
                        /* Figure out the rxb_agc_scale value */
	/*		if((ms_count & 0xFF) == 0)
			{
				gain = dbs_rx_b->rf_gain();
				if(soverflw > OVERFLOW_HIGH)
					gain -= 0.5;
				if(soverflw < OVERFLOW_LOW)
					gain += 0.5;
				dbs_rx_b->rf_gain(gain);
				agc_scale = (int32)floor(2.0*(dbs_rx_b->max_rf_gain() - gain));
				overflw = soverflw;
				soverflw = 0;
			}  */
			break;
		case SOURCE_USRP_V2:
//			Read_SOURCE_USRP_V2(_p);
			break;
		case SOURCE_SIGE_GN3S:
			Read_GN3S(_p);
			break;
		case SOURCE_NSL_PRIMO:
			Read_Primo(_p);
			break;
		case SOURCE_DISK_FILE:
			Read_File(_p);
			break;
		default:
			Read_USRP_V1(_p);
			break;
	}

	if(gopt.bb2file){
		FILE *fp;
		fp = fopen("crap.dat","a");
		fwrite(&_p->data[0], 1, SAMPS_MS*sizeof(CPX), fp);
		fclose(fp);
	}

	ms_count++;
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
int32_t GPS_Source::Open_USRP_V1()
{

	double ddc_correct_a = 0;
	double ddc_correct_b = 0;

	leftover = 0;

	if(opt.f_sample == 65.536e6)
	{
		if(opt.mode == 0)
		{
			sample_mode = 0;
			bwrite = 2048 * sizeof(CPX);
		}
		else
		{
			sample_mode = 1;
			bwrite = 4096 * sizeof(CPX);
		}
	}
	else
	{
		if(opt.mode == 0)
		{
			sample_mode = 2;
			bwrite = 2048 * sizeof(CPX);
		}
		else
		{
			sample_mode = 3;
			bwrite = 4096 * sizeof(CPX);
		}
	}

	/* Make the URX */
	urx = usrp_standard_rx::make(0, opt.decimate, 1, -1, 0, 0, 0);
	if(urx == NULL)
	{
		source_success = -1;
		if(opt.verbose)
			fprintf(stdout,"usrp_standard_rx::make FAILED\n");
		return(-1);
	}

	/* Set mux */
	urx->set_mux(0x32103210);
 
	/* N channels according to which mode we are operating in */
	if(opt.mode == 0)
		urx->set_nchannels(1);
	else
		urx->set_nchannels(2);

	/* Set the decimation */
	urx->set_decim_rate(opt.decimate);

	/* Setup board A */
	if(urx->daughterboard_id(0) == 2)
	{

                fprintf(stdout,"DBS-RX A Configuration CHECK\n");
		
                dbs_rx_a = new db_dbs_rx(urx, 0);

		/* Set the default master clock freq */
		dbs_rx_a->set_fpga_master_clock_freq(opt.f_sample);
		dbs_rx_a->set_refclk_divisor(16);
		dbs_rx_a->enable_refclk(true);

		/* Program the board */
		dbs_rx_a->bandwidth(opt.bandwidth);
		dbs_rx_a->if_gain(opt.gi);
		dbs_rx_a->rf_gain(opt.gr);
		dbs_rx_a->tune(opt.f_lo_a);

		/* Add additional frequency to ddc to account for imprecise LO programming */
		ddc_correct_a = dbs_rx_a->freq() - opt.f_lo_a;

		/* Set the DDC frequency */
		opt.f_ddc_a += ddc_correct_a;
		opt.f_ddc_a *= F_SAMPLE_NOM/opt.f_sample;

		if(opt.f_ddc_a > (F_SAMPLE_NOM/2.0))
			opt.f_ddc_a = F_SAMPLE_NOM - opt.f_ddc_a;

		urx->set_rx_freq(0, opt.f_ddc_a);

		/* Reset DDC phase to zero */
		urx->set_ddc_phase(0, 0);

		if(opt.verbose)
		{
			fprintf(stdout,"DBS-RX A Configuration\n");
			fprintf(stdout,"BW:			%15.2f\n",dbs_rx_a->bw());
			fprintf(stdout,"LO:			%15.2f\n",dbs_rx_a->freq());
			fprintf(stdout,"IF Gain: %15.2f\n",dbs_rx_a->if_gain());
			fprintf(stdout,"RF Gain: %15.2f\n",dbs_rx_a->rf_gain());
			fprintf(stdout,"DDC 0:	 %15.2f\n",urx->rx_freq(0));
		}
	}

	/* Setup board B (if it exists) */
	if(urx->daughterboard_id(1) == 2)
	{

                fprintf(stdout,"DBS-RX B Configuration CHECK\n");
		dbs_rx_b = new db_dbs_rx(urx, 1);

		/* Even if you are not using board B, you need to enable the RF
		 * section else it screws up the CN0 on board A */

		/* Set the default master clock freq */
		dbs_rx_b->set_fpga_master_clock_freq(opt.f_sample);
		dbs_rx_b->set_refclk_divisor(16);
		dbs_rx_b->enable_refclk(true);           // dushyanth

		/* Program the board */
		dbs_rx_b->bandwidth(opt.bandwidth);
		dbs_rx_b->if_gain(opt.gi);
		dbs_rx_b->rf_gain(opt.gr);
		dbs_rx_b->tune(opt.f_lo_b);

		/* Dual board mode */
		if(opt.mode)
		{

			/* Add additional frequency to ddc to account for imprecise LO programming */
			ddc_correct_b = dbs_rx_b->freq() - opt.f_lo_b;

			/* Set the DDC frequency */
			opt.f_ddc_b += ddc_correct_b;
			opt.f_ddc_b *= F_SAMPLE_NOM/opt.f_sample;

			if(opt.f_ddc_b > (F_SAMPLE_NOM/2.0))
				opt.f_ddc_b = F_SAMPLE_NOM - opt.f_ddc_b;

			urx->set_rx_freq(1, opt.f_ddc_b);

			/* Set mux for both channels */
			urx->set_mux(0x32103210);

			urx->set_ddc_phase(1, 0);

			if(opt.verbose)
			{
				fprintf(stdout,"DBS-RX B Configuration\n");
				fprintf(stdout,"BW:			%15.2f\n",dbs_rx_b->bw());
				fprintf(stdout,"LO:			%15.2f\n",dbs_rx_b->freq());
				fprintf(stdout,"IF Gain: %15.2f\n",dbs_rx_b->if_gain());
				fprintf(stdout,"RF Gain: %15.2f\n",dbs_rx_b->rf_gain());
				fprintf(stdout,"DDC 0:	 %15.2f\n",urx->rx_freq(0));

			}

		}
	}

	/* Start collecting data */
	fprintf(stdout,"USRP Start\n");

	urx->start();

	return(0);
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
int32_t GPS_Source::Open_GN3S()
{

	int32_t lcv;
	const double mul = 8.1838e6/2.048e6;

	/* Create the object */
	gn3s_a = new gn3s(0);
	if(gn3s_a ==NULL){
		source_success = -1;
		fprintf(stdout,"GN3S sampler not found\n");
		return(-1);
	}

	/* Create decimation lookup table */
	for(lcv = 0; lcv < 10240; lcv++)
	{
		gdec[lcv] = (int32_t)floor((double)lcv * mul);
	}

	/* Everything is super! */
	fprintf(stdout,"GN3S Start\n");

	return(0);
}
/*----------------------------------------------------------------------------------------------*/

/*
 * The file needs to contain no header, just baseband samples.
 * IF = 38.4 kHz, Fs = 2.048 MHz
 * I/Q samples (interleaved), 16-bit precision
 * /
/*----------------------------------------------------------------------------------------------*/
int32_t GPS_Source::Open_File()
{
	struct stat buf;

	fp_a = fopen(gopt.filename, "rb");
	if(fp_a == NULL){
		fprintf(stdout, "File %s not found!\n", gopt.filename);
		source_success = -1;
		return(-1);
	}

	//Get file length
	fstat(fileno(fp_a), &buf);
	file_size = buf.st_size;
	data_size = SAMPS_MS*sizeof(CPX);

	if(opt.verbose)
		fprintf(stdout, "Opened file of length %i bytes\n", file_size);

	fprintf(stdout,"File Start\n");
	return(0);
}
/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
int32_t GPS_Source::Open_Primo()
{
	primo_a = new nslPrimo;

	if (NULL == primo_a)
		return(-1);

	return 0;
}
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/
void GPS_Source::Close_USRP_V1()
{

	if(urx != NULL)
		urx->stop();

	if(dbs_rx_a != NULL)
		delete dbs_rx_a;

	if(dbs_rx_b != NULL)
		delete dbs_rx_b;

	if(opt.verbose)
		fprintf(stdout,"Destructing USRP\n");

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void GPS_Source::Close_GN3S()
{

	if(gn3s_a != NULL)
		delete gn3s_a;

	if(opt.verbose)
		fprintf(stdout,"Destructing GN3S\n");

}
/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void GPS_Source::Close_Primo()
{
	if (NULL != primo_a)
		delete primo_a;

	if(opt.verbose)
		fprintf(stdout,"Destructing nslPrimo object\n");
}
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/
void GPS_Source::Close_File()
{
	if(fp_a != NULL)
		fclose(fp_a);
	
	if(opt.verbose)
		fprintf(stdout,"Destructing File\n");
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void GPS_Source::Read_USRP_V1(ms_packet *_p)
{

	bool overrun;
          
        int32_t lcv,scaling;
        float_t scale=0;
        int32_t DESIRED_MAX = 32;   //16; //16; // 32; // 128;
        int32_t scaling_i, scaling_q;

	/* There may be a packet waiting already, if so copy and return immediately */
	switch(sample_mode)
	{
		case 2:

			if(leftover > 4000)
			{
				Resample_USRP_V1(buff, buff_out);
                         
         /*                   x86_cmax(buff_out, &scaling, SAMPS_MS);	           
                  //  fprintf(stdout,"maxvalue before scaling: %i \n",scaling);

                 if(DESIRED_MAX!=0){
           	      if(scaling>DESIRED_MAX){
			scale = (float_t)(scaling/DESIRED_MAX);
			//fprintf(stdout, "dividing by %lf ", scaling);
			//for(lcv=0; lcv<RESAMPS_BYTES_PER_READ; lcv++){
                        for(lcv=0; lcv<SAMPS_MS; lcv++){      //dushyanth
			       buff_out[lcv].i = buff_out[lcv].i / scale;
			       buff_out[lcv].q = buff_out[lcv].q / scale;
		            }
		    }
		   else{
			scale = (float_t)(DESIRED_MAX/scaling);
			//fprintf(stdout, "multiplying with %lf ", scaling);
			//for(lcv=0; lcv<RESAMPS_BYTES_PER_READ; lcv++){
	                for(lcv=0; lcv<SAMPS_MS; lcv++){    //dushyanth
        	                buff_out[lcv].i = buff_out[lcv].i * scale;
		  		buff_out[lcv].q = buff_out[lcv].q * scale;
	                    }
		        }

                }         */
    
				memcpy(_p->data, buff_out, SAMPS_MS*sizeof(CPX));
				leftover -= 4000;
				memcpy(dbuff, &buff[4000], leftover*sizeof(int32_t));
				memcpy(buff, dbuff, leftover*sizeof(int32_t));
				return;
			}
			break;

		case 3:

			if(leftover > 8000)
			{
				Resample_USRP_V1(buff, buff_out);
				memcpy(_p->data, buff_out, SAMPS_MS*sizeof(CPX));
				leftover -= 8000;
				memcpy(dbuff, &buff[8000], leftover*sizeof(int32_t));
				memcpy(buff, dbuff, leftover*sizeof(int32_t));
				return;
			}
			break;

		default:
			break;
	}

	/* Read data from USRP */
	urx->read(&buff[leftover], BYTES_PER_READ, &overrun);

//	if(overrun && _opt->verbose)
//	{
//		time(&rawtime);
//		timeinfo = localtime (&rawtime);
//		ffprintf(stdout,stdout, "\nUSRP overflow at time %s",asctime (timeinfo));
//		fflush(stdout);
//	}

	/* Now we have SAMPS_PER_READ samps, 4 possible things to do depending on the state:
	 * 0) mode == 0 && f_sample == 65.536e6: This mode is the easiest, 1 ms of data per FIFO node,
	 * hence just call resample() and stuff in the pipe
	 * 1) mode == 1/2 && f_sample == 65.536e6: This mode is also easy, 2 nodes = 1 ms, hence buffer 2 nodes together
	 * and then call resample();
	 * 2) mode == 0 && f_sample == 64.0e6: 1 node is slightly larger (4096 vs 4000) than 1 ms, must first double
	 * buffer the data to extract continuous 4000 sample packets then call resample()
	 * 3) mode == 1/2 && f_sample == 64.0e6: must take 2 nodes, create 8192 sample buffer, and similarly extract 8000
	 * sample packet and call resample(), requires double buffering */
	switch(sample_mode)
	{
		case 0:
			Resample_USRP_V1(buff, buff_out);
			memcpy(_p->data, buff_out, SAMPS_MS*sizeof(CPX));
			leftover = 0;
			break;
		case 1:
			leftover += 2048; leftover %= 4096;
			if(leftover == 0)
			{
				Resample_USRP_V1(buff, buff_out);
				memcpy(_p->data, buff_out, SAMPS_MS*sizeof(CPX));
			}
			break;
		case 2:

			leftover += 96;
			Resample_USRP_V1(buff, buff_out);
                       
            /*            x86_cmax(buff_out, &scaling, SAMPS_MS);	           
                  //   fprintf(stdout,"maxvalue before scaling: %i \n",scaling);

                 if(DESIRED_MAX!=0){
           	      if(scaling>DESIRED_MAX){
			scale = (float_t)(scaling/DESIRED_MAX);
			//fprintf(stdout, "dividing by %lf ", scaling);
			//for(lcv=0; lcv<RESAMPS_BYTES_PER_READ; lcv++){
                        for(lcv=0; lcv<SAMPS_MS; lcv++){      //dushyanth
			       buff_out[lcv].i = buff_out[lcv].i / scale;
			       buff_out[lcv].q = buff_out[lcv].q / scale;
		            }
		    }
		   else{
			scale = (float_t)(DESIRED_MAX/scaling);
			//fprintf(stdout, "multiplying with %lf ", scaling);
			//for(lcv=0; lcv<RESAMPS_BYTES_PER_READ; lcv++){
	                for(lcv=0; lcv<SAMPS_MS; lcv++){    //dushyanth
        	                buff_out[lcv].i = buff_out[lcv].i * scale;
		  		buff_out[lcv].q = buff_out[lcv].q * scale;
	                    }
		        }

                }          */
    
			memcpy(_p->data, buff_out, SAMPS_MS*sizeof(CPX));

			/* Move excess bytes at end of buffer down to the base */
			memcpy(dbuff, &buff[4000], leftover*sizeof(int32_t));
			memcpy(buff, dbuff, leftover*sizeof(int32_t));
			break;

		case 3:

			leftover += 192;
			Resample_USRP_V1(buff, buff_out);
			memcpy(_p->data, buff_out, SAMPS_MS*sizeof(CPX));

			/* Move excess bytes at end of buffer down to the base */
			memcpy(dbuff, &buff[8000], leftover*sizeof(int32_t));
			memcpy(buff, dbuff, leftover*sizeof(int32_t));
			break;

		default:
			break;
	}

        // for(int fk=1000; fk<1090; fk++) {
       //  fprintf(stdout,"_p->data(i):%d\n",(_p->data[fk]).i);              // dushyanth
       // fprintf(stdout,"_p->data(q):%d\n",(_p->data[fk]).q);
       // }

	/*	x86_cavg(_p->data,&scaling_i, &scaling_q, SAMPS_MS);
                fprintf(stdout, "Average I data  %i \n", scaling_i);
		fprintf(stdout, "Average Q data  %i \n", scaling_q);
 		
		x86_cmax(_p->data, &scaling, SAMPS_MS);      //samples_read
               fprintf(stdout, "Max value after scaling %i \n", scaling); // max value is 32;
       */
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void GPS_Source::Read_GN3S(ms_packet *_p)
{

	int32_t bread, started, check;
	bool overrun;
	int32_t ms_mod5;
	int32_t lcv;
	int16_t LUT[4] = {1, -1, 3, -3};
	int16_t *pbuff;

	ms_mod5 = ms_count % 5;

	if(ms_count == 0)
	{
		/* Start transfer */
		while(!started)
		{
			usleep(100);
			started = gn3s_a->usrp_xfer(VRQ_XFER, 1);
		}

		/* Make sure we are reading I0,Q0,I1,Q1,I2,Q2.... etc */
		bread = gn3s_a->read((void*)(&gbuff[0]),1);
		check = (gbuff[0] & 0x3);	 //0 or 1 -> I sample , 2 or 3 -> Q sample
		if(check < 2)
		{
			bread = gn3s_a->read((void*)(&gbuff[0]),1);
		}
	}

	/* Do the GN3S reading */
	if(ms_mod5 == 0)
	{
		pbuff = (int16_t *)&buff[7];

		/* Read 5 ms */
		bread = gn3s_a->read((void *)&gbuff[0], 40919*2);

		/* Convert to +-1 */
		for(lcv = 0; lcv < 40919*2; lcv++)
		{	pbuff[lcv] = LUT[gbuff[lcv] & 0x3];
                	//fprintf(stdout, "pbuff[%i] = %i\n", lcv,pbuff[lcv]);
                }
		/* Filter & decimate the data to regain bit precision */
		Resample_GN3S(&buff[0], &buff_out[0]);

		/* Move last 7 elements to the bottom */
		memcpy(&buff[0], &buff[40919], 7*sizeof(CPX));

		/* Check the overrun */
		overrun = gn3s_a->check_rx_overrun();
		if(overrun && opt.verbose)
		{
			time(&rawtime);
			timeinfo = localtime (&rawtime);
			fprintf(stdout, "GN3S overflow at time %s\n", asctime(timeinfo));
			fflush(stdout);
		}

	}

	/* Move pointer */
	buff_out_p = &buff_out[SAMPS_MS*ms_mod5];

	/* Copy to destination */
	memcpy(_p->data, buff_out_p, SAMPS_MS*sizeof(CPX));

}
/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void GPS_Source::Read_Primo(ms_packet *_p)
{
	int ret, dk, sk, cnt;
	int8_t cpx8v[PRIMO_SAMP_MS/2][2];

	if(ms_count == 0)
		primo_a->Start();

	if ((!(ms_count%1000)) && (opt.verbose))
		fprintf(stderr, ".");

	// Just grab data from USB if there isn't enough on the buffer
	if (primoBuffSize < PRIMO_SAMP_MS) {
		// Grab data from USB
		primo_a->Read(&primoBuff[primoBuffSize]);

		// Update the buffer size and because of this I expect to have enough
		// data now. This is because 1 packet is typically
		// 16KBytes, and the f_S=5.456MHz
		primoBuffSize += primo_a->ReadSize();
	}

	// Reorganise the IF signal in BB
	// -- I don't like this, can be done with SSE2
	for(sk=0; sk < PRIMO_SAMP_MS; sk+=4) {
		cpx8v[0+sk/2][0] = +4*primoBuff[sk+0];
		cpx8v[0+sk/2][1] = +4*primoBuff[sk+1];
		cpx8v[1+sk/2][0] = -4*primoBuff[sk+2];
		cpx8v[1+sk/2][1] = -4*primoBuff[sk+3];
	}

	// Resample to 2.048 Msps, slow...
	for(sk=1024, dk=0; dk<SAMPS_MS; dk++) {
		(_p->data[dk]).i = cpx8v[(sk>>11)][0];
		(_p->data[dk]).q = cpx8v[(sk>>11)][1];
		sk += (PRIMO_SAMP_MS/2);
	}

	// Mark the processed bytes as used
	primoBuffSize -= PRIMO_SAMP_MS;
	// Move the data up (how unefficient...) but keeps code very simple
	memmove(primoBuff, primoBuff+PRIMO_SAMP_MS, primoBuffSize);
}
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/
void GPS_Source::Read_File(ms_packet *_p)
{
	int32_t bytes_read, lcv, scaling;
	int32_t DESIRED_MAX = 32; //128;
        float_t scale=0;
        int32_t scaling_i, scaling_q;


	if((total_read+data_size)>file_size){
		fprintf(stdout, "End of file, quitting\n");
		grun=0;
	}
	else{
		bytes_read = fread(&buff_out[0], 1, data_size, fp_a);
		total_read += bytes_read;
		       
                      x86_cmax(buff_out, &scaling, SAMPS_MS);
                    // fprintf(stdout,"maxvalue before scaling: %i \n",scaling);

                 if(DESIRED_MAX!=0){
           	      if(scaling>DESIRED_MAX){
			scale = (float_t)(scaling/DESIRED_MAX);
			//fprintf(stdout, "dividing by %lf ", scaling);
			//for(lcv=0; lcv<RESAMPS_BYTES_PER_READ; lcv++){
                        for(lcv=0; lcv<SAMPS_MS; lcv++){      //dushyanth
			       buff_out[lcv].i = buff_out[lcv].i / scale;
			       buff_out[lcv].q = buff_out[lcv].q / scale;
		            }
		    }
		   else{
			scale = (float_t)(DESIRED_MAX/scaling);
			//fprintf(stdout, "multiplying with %lf ", scaling);
			//for(lcv=0; lcv<RESAMPS_BYTES_PER_READ; lcv++){
	                for(lcv=0; lcv<SAMPS_MS; lcv++){    //dushyanth
        	                buff_out[lcv].i = buff_out[lcv].i * scale;
		  		buff_out[lcv].q = buff_out[lcv].q * scale;
	                    }
		        }

                }
		if(gopt.realtime)
			usleep(1000);
		memcpy(_p->data, (void *)buff_out, data_size);
	}
      
       // for(int fk=1000; fk<1090; fk++) {
       //  fprintf(stdout,"_p->data(i):%d\n",(_p->data[fk]).i);              // dushyanth
       // fprintf(stdout,"_p->data(q):%d\n",(_p->data[fk]).q);
       // }

	/*	x86_cavg(_p->data,&scaling_i, &scaling_q, SAMPS_MS);
                fprintf(stdout, "Average I data  %i \n", scaling_i);
		fprintf(stdout, "Average Q data  %i \n", scaling_q);
 		
		x86_cmax(_p->data, &scaling, SAMPS_MS);      //samples_read
               fprintf(stdout, "Max value  %i \n", scaling); // max value is 32;
         */
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void GPS_Source::Resample_USRP_V1(CPX *_in, CPX *_out)
{

	CPX buff_a[4096]; /* Max size based on 65.536 or 64 Msps */
	CPX buff_b[4096]; /* Max size based on 65.536 or 64 Msps */
	int32_t *p_a;
	int32_t *p_b;
	int32_t *p_in;
	int32_t *p_out;
	int32_t samps_ms;
	int32_t lcv;

	p_a = (int32_t *)&buff_a[0];
	p_b = (int32_t *)&buff_b[0];
	p_in = (int32_t *)_in;
	p_out = (int32_t *)_out;

	samps_ms = (int32_t)floor(opt.f_sample/opt.decimate/1e3);

	if(opt.mode == 0)
	{
		/* Not much to do, just downsample from either 4.096 or 4.0 to 2.048e6 */
		if(opt.f_sample != 65.536e6)
			downsample(_out, _in, 2.048e6, opt.f_sample/opt.decimate, samps_ms);
		else
		{
			for(lcv = 0; lcv < samps_ms; lcv += 2)
				*p_out++ = p_in[lcv];
		}
	}
	else //!< 2 boards are being used, must first de-interleave data before downsampling
	{

		/* De-interleave */
		for(lcv = 0; lcv < samps_ms; lcv++)
		{
			p_a[lcv] = *p_in++;
			p_b[lcv] = *p_in++;
		}

		/* Downsample (and copy!) into appropriate location */
		downsample(&_out[0],		buff_a, 2.048e6, opt.f_sample/opt.decimate, samps_ms);
		downsample(&_out[2048], buff_b, 2.048e6, opt.f_sample/opt.decimate, samps_ms);

	}

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void GPS_Source::Resample_GN3S(CPX *_in, CPX *_out)
{
	/* Runs specified filter on incoming signal. */
	int32_t lcv, ind;
	int16_t tmp;

	/* Process the array */
	for(lcv = 0; lcv < 10240; lcv++)
	{
		ind = gdec[lcv];

		tmp = 8;
		tmp += _in[ind +	6].i *	3;
		tmp += _in[ind +	5].i * 97;
		tmp += _in[ind +	4].i * 77;
		tmp += _in[ind +	3].i * 86;
		tmp += _in[ind +	2].i * 77;
		tmp += _in[ind +	1].i * 97;
		tmp += _in[ind +	0].i *	3;
		_out[lcv].i = tmp >> 4;

		tmp = 8;
		tmp += _in[ind +	6].q *	3;
		tmp += _in[ind +	5].q * 97;
		tmp += _in[ind +	4].q * 77;
		tmp += _in[ind +	3].q * 86;
		tmp += _in[ind +	2].q * 77;
		tmp += _in[ind +	1].q * 97;
		tmp += _in[ind +	0].q *	3;
		_out[lcv].q = tmp >> 4;

//		tmp = 4;
//		tmp += _in[ind + 12].i *	 3;
//		tmp += _in[ind + 11].i *	-8;
//		tmp += _in[ind + 10].i * -11;
//		tmp += _in[ind +	9].i *	 1;
//		tmp += _in[ind +	8].i *	26;
//		tmp += _in[ind +	7].i *	52;
//		tmp += _in[ind +	6].i *	63;
//		tmp += _in[ind +	5].i *	52;
//		tmp += _in[ind +	4].i *	26;
//		tmp += _in[ind +	3].i *	1;
//		tmp += _in[ind +	2].i * -11;
//		tmp += _in[ind +	1].i *	-8;
//		tmp += _in[ind +	0].i *	 3;
//		_out[lcv].i = tmp >> 3;
//
//		tmp = 4;
//		tmp += _in[ind + 12].q *	 3;
//		tmp += _in[ind + 11].q *	-8;
//		tmp += _in[ind + 10].q * -11;
//		tmp += _in[ind +	9].q *	 1;
//		tmp += _in[ind +	8].q *	26;
//		tmp += _in[ind +	7].q *	52;
//		tmp += _in[ind +	6].q *	63;
//		tmp += _in[ind +	5].q *	52;
//		tmp += _in[ind +	4].q *	26;
//		tmp += _in[ind +	3].q *	1;
//		tmp += _in[ind +	2].q * -11;
//		tmp += _in[ind +	1].q *	-8;
//		tmp += _in[ind +	0].q *	 3;
//		_out[lcv].q = tmp >> 3;

	}

	return;
}
/*----------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------*/           // Added by dushyanth
void GPS_Source::Downsample(CPX *_dest, CPX *_source, double _fdest, double _fsource, int32_t _samps)
{

	int32_t lcv, k;
	uint32_t phase_step;
	uint32_t lphase, phase;

	phase_step = (uint32_t)floor((double)4294967296.0*_fdest/_fsource);

	k = lphase = phase = 0;

	for(lcv = 0; lcv < _samps; lcv++)
	{
		if(phase <= lphase)
		{
			_dest[k] = _source[lcv];
			k++;
		}

		lphase = phase;
		phase += phase_step;
	}

}
/*----------------------------------------------------------------------------------------------*/
