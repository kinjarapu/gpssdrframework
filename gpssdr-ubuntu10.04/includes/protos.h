/*----------------------------------------------------------------------------------------------*/
/*! \file protos.h
//
// FILENAME: protos.h
//
// DESCRIPTION: Misc function prototypes
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


/* These are all found in Main() */
/*----------------------------------------------------------------------------------------------*/
void Parse_Arguments(int32_t _argc, char* _argv[]); //!< Parse command line arguments to setup functionality
int32_t Hardware_Init(void);              //!< Initialize any hardware (for realtime mode)
int32_t Object_Init(void);                //!< Initialize all threaded objects and global variables
int32_t Pipes_Init(void);               //!< Initialize all pipes
int32_t Thread_Init(void);                //!< Finally start up the threads
void Thread_Shutdown(void);							//!< First step to shutdown, stopping the threads
void Pipes_Shutdown(void);							//!< Close all the pipes
void Object_Shutdown(void);							//!< Delete/free all objects
void Hardware_Shutdown(void);						//!< Shutdown any hardware
/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/
void *Acquisition_Thread(void *_arg);
/*----------------------------------------------------------------------------------------------*/

/* Found in Misc.cpp */
/*----------------------------------------------------------------------------------------------*/
int32_t code_gen(CPX *_dest, int32_t _prn);
void sine_gen(CPX *_dest, double _f, double _fs, int32_t _samps);
void sine_gen(CPX *_dest, double _f, double _fs, int32_t _samps, double _p);
void wipeoff_gen(MIX *_dest, double _f, double _fs, int32_t _samps);
void downsample(CPX *_dest, CPX *_source, double _fdest, double _fsource, int32_t _samps);
void init_agc(CPX *_buff, int32_t _samps, int32_t bits, int32_t *scale);
int32_t run_agc(CPX *_buff, int32_t _samps, int32_t bits, int32_t scale);
int32_t AtanApprox(int32_t y, int32_t x);
int32_t Atan2Approx(int32_t y, int32_t x);
int32_t Invert4x4(double A[4][4], double B[4][4]);
void FormCCSDSPacketHeader(CCSDS_Packet_Header *_p, uint32_t _apid, uint32_t _sf, uint32_t _pl, uint32_t _cm, uint32_t _tic);
void DecodeCCSDSPacketHeader(CCSDS_Decoded_Header *_d, CCSDS_Packet_Header *_p);
uint32_t adler(uint8_t *data, int32_t len);
/*----------------------------------------------------------------------------------------------*/

