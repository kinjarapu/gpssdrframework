/*! \file SIMD.h
	Contains prototpyes for all the SIMD functions
*/

/************************************************************************************************
Copyright 2008 Gregory W Heckler

This file is part of the GPS Software Defined Radio (GPS-SDR)

The GPS-SDR is free software; you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GPS-SDR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along with GPS-SDR; if not,
write to the:

Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
************************************************************************************************/


#ifndef SIMD_H_
#define SIMD_H_


#ifdef GLOBALS_HERE
	#define EXTERN
#else
	#define EXTERN extern
#endif


/* Found in CPUID.cpp */
/*----------------------------------------------------------------------------------------------*/
bool CPU_MMX();		//!< Does the CPU support MMX?
bool CPU_SSE();		//!< Does the CPU support SSE?
bool CPU_SSE2();	//!< Does the CPU support SSE2?
bool CPU_SSE3();	//!< Does the CPU support SSE3?
bool CPU_SSSE3();	//!< Does the CPU support SSSE3? No thats not a typo!
bool CPU_SSE41();	//!< Does the CPU support SSE4.1?
bool CPU_SSE42();	//!< Does the CPU support SSE4.2?
void Init_SIMD();	//!< Initialize the global function pointers
/*----------------------------------------------------------------------------------------------*/

/* Found in SSE.cpp */
/*----------------------------------------------------------------------------------------------*/
void  sse_add(int16_t *A, int16_t *B, int32_t cnt) __attribute__ ((noinline));  //!< Pointwise vector addition
void  sse_sub(int16_t *A, int16_t *B, int32_t cnt) __attribute__ ((noinline));  //!< Pointwise vector difference
void  sse_mul(int16_t *A, int16_t *B, int32_t cnt) __attribute__ ((noinline));  //!< Pointwise vector multiply
int32_t sse_dot(int16_t *A, int16_t *B, int32_t cnt) __attribute__ ((noinline));  //!< Compute vector dot product

void  sse_conj(CPX *A, int32_t cnt) __attribute__ ((noinline));                     //!< Pointwise vector conjugate
void  sse_cacc(CPX *A, MIX *B, int32_t cnt, int32_t *iaccum, int32_t *baccum) __attribute__ ((noinline));   //!< Compute dot product of cpx and a mix vector
void  sse_cmul(CPX *A, CPX *B, int32_t cnt) __attribute__ ((noinline));                 //!< Pointwise vector multiply
void  sse_cmuls(CPX *A, CPX *B, int32_t cnt, int32_t shift) __attribute__ ((noinline));           //!< Pointwise vector multiply with shift
void  sse_cmulsc(CPX *A, CPX *B, CPX *C, int32_t cnt, int32_t shift) __attribute__ ((noinline));      //!< Pointwise vector multiply with shift, dump results into C
void  sse_prn_accum(CPX *A, CPX *E, CPX *P, CPX *L, int32_t cnt, CPX *accum) __attribute__ ((noinline));  //!< This is a long story
void  sse_prn_accum_new(CPX *A, MIX *E, MIX *P, MIX *L, int32_t cnt, CPX_ACCUM *accum) __attribute__ ((noinline));  //!< This is a long story
void  sse_max(int32_t *_A, int32_t *_index, int32_t *_magt, int32_t _cnt) __attribute__ ((noinline));
/*----------------------------------------------------------------------------------------------*/

/* Found in x86.cpp */
/*----------------------------------------------------------------------------------------------*/
void  x86_add(int16_t *A, int16_t *B, int32_t cnt); //!< Pointwise vector addition
void  x86_sub(int16_t *A, int16_t *B, int32_t cnt); //!< Pointwise vector difference
void  x86_mul(int16_t *A, int16_t *B, int32_t cnt); //!< Pointwise vector multiply
void  x86_muls(int16_t *A, int16_t *B, int32_t cnt, int32_t shift); //!< Pointwise vector multiply
int32_t x86_dot(int16_t *A, int16_t *B, int32_t cnt); //!< Compute vector dot product

void  x86_conj(CPX *A, int32_t cnt);                      //!< Pointwise vector conjugate
void  x86_cacc(CPX *A, MIX *B, int32_t cnt, int32_t *iaccum, int32_t *baccum);  //!< Compute dot product of cpx and a mix vector
void  x86_cmul(CPX *A, CPX *B, int32_t cnt);                  //!< Pointwise vector multiply
void  x86_cmuls(CPX *A, CPX *B, int32_t cnt, int32_t shift);          //!< Pointwise complex multiply with shift
void  x86_cmulsc(CPX *A, CPX *B, CPX *C, int32_t cnt, int32_t shift);     //!< Pointwise vector multiply with shift, dump results into C
void  x86_cmag(CPX *A, int32_t cnt);                      //!< Convert from complex to a power
void  x86_prn_accum(CPX *A, CPX *E, CPX *P, CPX *L, int32_t cnt, CPX *accum);  //!< This is a long story
void  x86_prn_accum_new(CPX *A, MIX *E, MIX *P, MIX *L, int32_t cnt, CPX_ACCUM *accum);  //!< This is a long story
void  x86_max(int32_t *_A, int32_t *_index, int32_t *_magt, int32_t _cnt);

void x86_cavg(CPX *_A, int32_t *_magt_i, int32_t *_magt_q, int32_t _cnt) ; // added by fuwei
void x86_cmax(CPX *_A, int32_t *_magt, int32_t _cnt); // added by fuwei


void  x86_cmag_new(CPX *_A, int32_t *_B, int32_t _cnt);
/*----------------------------------------------------------------------------------------------*/


#endif /*SIMD_H_*/
