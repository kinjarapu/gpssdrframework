/*----------------------------------------------------------------------------------------------*/
/*! \file gps_source.h
//
// FILENAME: gps_source.h
//
// DESCRIPTION: Defines the GPS_Source class.
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
//				DOXYGEN and are utilized for automated document extraction
//
// Reference:
*/
/*----------------------------------------------------------------------------------------------*/

#ifndef GPS_SOURCE_H_
#define GPS_SOURCE_H_

#include "includes.h"
#include "db_dbs_rx.h"
#include "gn3s.h"
#include "primo.h"

enum GPS_SOURCE_TYPE
{
	SOURCE_USRP_V1,
	SOURCE_USRP_V2,
	SOURCE_SIGE_GN3S,
	SOURCE_DISK_FILE,
	SOURCE_NSL_PRIMO
};

/*! \ingroup CLASSES
 *
 */
class GPS_Source
{

	private:

		/* Options */
		Options_S opt;			//!< Options

		/* Generic variables */
		int32_t source_type;	//!< Source type
		int32_t sample_mode;	//!< Sample mode
		int32_t leftover;		//!< Leftover bytes for USRP double buffering
		int32_t bwrite;			//!< Bytes somthing something?
		int32_t ms_count;		//!< Count the numbers of ms processed
		int32_t file_size;		//!< File length in bytes
		int32_t total_read;		//!< Number of bytes already read from file
		int32_t data_size;		//!< Number of bytes to read each time

		/* Tag overflows */
		time_t rawtime;
		struct tm * timeinfo;

		/* AGC Values */
		int32_t agc_scale;		//!< To do the AGC
		int32_t overflw;		//!< Overflow counter
		int32_t soverflw;	 //!< Overflow counter

		/* Data buffers */
		int8 gbuff[40919*2]; 	//!< Byte buffer for GN3S
		CPX buff[40932]; 		//!< Base buffer for GN3S/USRP
		CPX buff_out[10240]; 	//!< Output buffer @ 2.048 Msps
		CPX *buff_out_p; 		//!< Pointer to a spot in buff_out
		CPX dbuff[16384]; 		//!< Buffer for double buffering
		MIX gn3s_mix[10240];	//!< Mix GN3S to the same IF frequency
		int32_t gdec[10240];		//!< Index array to filter & resample GN3S data to 2.048 Msps
		uint8_t primoBuff[1<<15]; //!< To manage PRIMO resampling
		int primoBuffSize;

		/* USRP V1 Handles */
		usrp_standard_rx_sptr urx;
		db_dbs_rx *dbs_rx_a;
		db_dbs_rx *dbs_rx_b;

		/* USRP V2 Handles */

		/* SOURCE_SIGE_GN3S Handles */
		gn3s *gn3s_a;
		gn3s *gn3s_b;

		/* SOURCE_NSL_PRIMO Handles */
		nslPrimo *primo_a;
		nslPrimo *primo_b;

		/* File handles */
		FILE *fp_a;
		FILE *fp_b;

	private:

		int32_t Open_USRP_V1();		//!< Open the USRP Version 1
		int32_t Open_USRP_V2();		//!< Open the USRP Version 2
		int32_t Open_GN3S();			//!< Open the SparkFun GN3S Sampler
		int32_t Open_Primo();			//!< Open the NSL PRIMO data grabber
		int32_t Open_File();			//!< Open the file

		void Close_USRP_V1();		//!< Close the USRP Version 1
		void Close_USRP_V2();		//!< Close the USRP Version 2
		void Close_GN3S();			//!< Close the SparkFun GN3S Sampler
		void Close_Primo();			//!< Close the NSL PRIMO data grabber
		void Close_File();			//!< Close the file

		void Read_USRP_V1(ms_packet *_p);	//!< Read from the USRP Version 1
		void Read_USRP_V2(ms_packet *_p);	//!< Read from the USRP Version 2
		void Read_GN3S(ms_packet *_p);		//!< Read from the SparkFun GN3S Sampler
		void Read_Primo(ms_packet *_p);		//!< Read from the PRIMO grabber
		void Read_File(ms_packet *_p);		//!< Read from a file

		void Resample_USRP_V1(CPX *_in, CPX *_out);
		void Resample_GN3S(CPX *_in, CPX *_out);
                void Downsample(CPX *_dest, CPX *_source, double _fdest, double _fsource, int32_t _samps);

	public:

		GPS_Source(Options_S *_opt);	//!< Create the GPS source with the proper hardware type
		~GPS_Source();					//!< Kill the object
		void Read(ms_packet *_p);		//!< Read in a single ms of data
		int32_t getScale(){return(agc_scale);}
		int32_t getOvrflw(){return(overflw);}

};

#endif /* GPS_SOURCE_H_ */
