/*----------------------------------------------------------------------------------------------*/
/*! \file acquisition.cpp
//
// FILENAME: acquisition.cpp
//
// DESCRIPTION: Defines the Acquisition class.
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

#ifndef ACQUISITION_H_
#define ACQUISITION_H_

#include "includes.h"
#include "sv_select.h"
#include "fifo.h"
#include "fft.h"

/*! @ingroup CLASSES
	@brief /xyzzy */
class Acquisition : public Threaded_Object
{

	typedef struct Acq_Result{
	int32	code_phase;
		int32	doppler;
		uint32	magnitude;
		double_t relmagn;
	} Acq_Result;

	private:

		pthread_t thread;
		CPX *fft_codes[MAX_SV];					//!< Store the FFTd Codes;

		ms_packet packet;						//!< Get IF data
		CPX *buff;								//!< Result after mixing the buffer to baseband
		CPX *msbuff;							//!< Random buffer for 1 ms stuff
		int32_t *cmags;							//!< Magnitudes of the FFT-Corr-iFFT'd samples
		CPX *baseband;							//!< Result after mixing the buffer to baseband
		CPX *baseband_shift;					//!< Result after mixing the buffer to baseband, used for the "circular shifts"
		CPX **baseband_rows;					//!< Row pointer
		CPX *coherent;							//!< Used for the 10 ms coherent integration
		CPX *_000Hzwipeoff;						//!< Sinusoid used to perform mix to baseband
		CPX	*_250Hzwipeoff;						//!< Sinusoid to mix by Fif - 250 Hz
		CPX	*_500Hzwipeoff;						//!< Sinusoid to mix by Fif - 500 Hz
		CPX	*_750Hzwipeoff;						//!< Sinusoid to mix by Fif - 750 Hz
		CPX *rotate;							//!< Buffer used for circular rotation of vector
		CPX *power;
		MIX *dft;								//!< Used for the post correlation DFT
		MIX **dft_rows;							//!< Used for the post correlation DFT

		FFT *pFFT;								//!< The FFT used to perform correlation
		FFT *piFFT;								//!< The FFT used to perform correlation

		int32_t sv;								//!< Search for this SV
		int32_t state;							//!< Search using this state (STRONG, MEDIUM, or WEAK)
		int32_t corr;							//!< This correlator requested an acquisition
		double_t REL_THRESHOLD;					//!< Acquisition discriminator threshold
		Acq_Command_S request;					//!< Acquisition transaction
		Acq_Command_S results[MAX_SV];			//!< Where to store the results
		Acq_Result acqstrong[2];

	public:

		Acquisition();																		//!< Create and initialize object, need _fsample as a necessary argument
		~Acquisition();																		//!< Shutdown gracefully
		void doAcqStrong(int32_t _sv, int32_t _doppmin, int32_t _doppmax, int32 _which);	//!< Look for this sv in this doppler range using a 1 ms correlation (_buff must be 1 ms long)
		void doAcqMedium(int32_t _sv, int32_t _doppmin, int32_t _doppmax);					//!< Look for this sv in this doppler range using a 10 ms correlation (_buff must be 20 ms long)
		void doAcqWeak(int32_t _sv, int32_t _doppmin, int32_t _doppmax); 					//!< Look for this sv in this doppler range using a 10 ms correlation and 15 incoherent integrations (_buff must be 310 ms long)
		void doPrepIF(int32_t _type, CPX *_buff);											//!< Prep the IF (done once if detecting multiple SVs in same data set)
		void Import();																		//!< Get a chuck of data to operate on
		void Export();																		//!< Dump results
		void Acquire();																		//!< Acquire with respect to current state
		void Discriminator(int32_t _sv);
		void Start();

};

#endif /*ACQUISITION_H_*/
