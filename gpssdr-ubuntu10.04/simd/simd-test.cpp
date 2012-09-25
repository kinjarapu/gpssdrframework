/*! \file Main.cpp
	Entry point of program
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

#define GLOBALS_HERE

#include "includes.h"
#include "simd_test.h"

#define VECTSIZE (10000)
#define REPEATS  (100)
#define VERBOSE  (0)

int main(int32 argc, char* argv[])
{
	fprintf(stdout,"SIMD_Test\n");

	testvecta = new CPX[VECTSIZE];
	testvectb = new CPX[VECTSIZE];
	testvectc = new CPX[VECTSIZE];
	testvectd = new CPX[VECTSIZE];
	testvecte = new CPX[VECTSIZE];

	testvectf = new MIX[VECTSIZE];
	testvectg = new MIX[VECTSIZE];
	testvecth = new MIX[VECTSIZE];

	test_add();
	test_sub();
	test_mul();
	test_dot();
	test_conj();
	test_cmul();
	test_cmuls();
	test_cmulsc();
	test_cacc();
	test_prn_accum_new();

	delete [] testvecta;
	delete [] testvectb;
	delete [] testvectc;
	delete [] testvectd;
	delete [] testvecte;
	delete [] testvectf;
	delete [] testvectg;
	delete [] testvecth;

	return(1);
}


/* SIMD ADD */
/*----------------------------------------------------------------------------------------------*/
void test_add(){
	int32 err, lcv, lcv2;
	err = 0;

	for(lcv = 0; lcv < REPEATS; lcv++)
	{
		/* generate two different random vectors */
		CPX *longvect;
		longvect = new CPX[2*VECTSIZE];
		fill_vect(longvect, 2*VECTSIZE);
		memcpy(testvecta, longvect, VECTSIZE*sizeof(CPX));
		memcpy(testvectb, &longvect[VECTSIZE], VECTSIZE*sizeof(CPX));
		
#if VERBOSE
		fprintf(stdout, "A:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "B:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectb[lcv2].i, 
				(testvectb[lcv2].q<0?'-':'+'), abs(testvectb[lcv2].q));
		fprintf(stdout, "\n");
#endif

		memcpy(testvectc, testvecta, VECTSIZE*sizeof(CPX));

		x86_add((int16 *)testvecta, (int16 *)testvectb, VECTSIZE*2);
#if VERBOSE
		fprintf(stdout, "x86:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d+j%d\t", testvecta[lcv2].i,testvecta[lcv2].q);
		fprintf(stdout, "\n");
#endif
		
		sse_add((int16 *)testvectc, (int16 *)testvectb, VECTSIZE*2);
#if VERBOSE
		fprintf(stdout, "sse2:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d+j%d\t",testvectc[lcv2].i,testvectc[lcv2].q);
		fprintf(stdout, "\n");
#endif

		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			if((testvecta[lcv2].i != testvectc[lcv2].i)||(testvecta[lcv2].q != testvectc[lcv2].q))
					err++;
	}

	if(err)
		fprintf(stdout,"INT16 ADD \t\t\tFAILED: %d errors\n",err);
	else
		fprintf(stdout,"INT16 ADD \t\t\tPASSED\n");
}
	/*----------------------------------------------------------------------------------------------*/


/* SIMD SUB */
/*----------------------------------------------------------------------------------------------*/
void test_sub(){
	int32 err, lcv, lcv2;
	err = 0;

	for(lcv = 0; lcv < REPEATS; lcv++)
	{
		/* generate two different random vectors */
		CPX *longvect;
		longvect = new CPX[2*VECTSIZE];
		fill_vect(longvect, 2*VECTSIZE);
		memcpy(testvecta, longvect, VECTSIZE*sizeof(CPX));
		memcpy(testvectb, &longvect[VECTSIZE], VECTSIZE*sizeof(CPX));

#if VERBOSE
		fprintf(stdout, "A:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "B:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectb[lcv2].i, 
				(testvectb[lcv2].q<0?'-':'+'), abs(testvectb[lcv2].q));
		fprintf(stdout, "\n");
#endif

		memcpy(testvectc, testvecta, VECTSIZE*sizeof(CPX));

		x86_sub((int16 *)testvecta, (int16 *)testvectb, VECTSIZE*2);
#if VERBOSE
		fprintf(stdout, "x86:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d+j%d\t", testvecta[lcv2].i,testvecta[lcv2].q);
		fprintf(stdout, "\n");
#endif

		sse_sub((int16 *)testvectc, (int16 *)testvectb, VECTSIZE*2);
#if VERBOSE
		fprintf(stdout, "sse2:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d+j%d\t",testvectc[lcv2].i,testvectc[lcv2].q);
		fprintf(stdout, "\n");
#endif

		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			if((testvecta[lcv2].i != testvectc[lcv2].i)||(testvecta[lcv2].q != testvectc[lcv2].q))
					err++;
	}

	if(err)
		fprintf(stdout,"INT16 SUB \t\t\tFAILED: %d errors\n",err);
	else
		fprintf(stdout,"INT16 SUB \t\t\tPASSED\n");
}
/*----------------------------------------------------------------------------------------------*/


/* SIMD MUL */
/*----------------------------------------------------------------------------------------------*/
void test_mul(){
	int32 err, lcv, lcv2;
	err = 0;

	for(lcv = 0; lcv < REPEATS; lcv++)
	{
		/* generate two different random vectors */
		CPX *longvect;
		longvect = new CPX[2*VECTSIZE];
		fill_vect(longvect, 2*VECTSIZE);
		memcpy(testvecta, longvect, VECTSIZE*sizeof(CPX));
		memcpy(testvectb, &longvect[VECTSIZE], VECTSIZE*sizeof(CPX));

#if VERBOSE
		fprintf(stdout, "A:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "B:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectb[lcv2].i, 
				(testvectb[lcv2].q<0?'-':'+'), abs(testvectb[lcv2].q));
		fprintf(stdout, "\n");
#endif

		memcpy(testvectc, testvecta, VECTSIZE*sizeof(CPX));

		x86_mul((int16 *)testvecta, (int16 *)testvectb, VECTSIZE*2);
#if VERBOSE
		fprintf(stdout, "x86:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d+j%d\t", testvecta[lcv2].i,testvecta[lcv2].q);
		fprintf(stdout, "\n");
#endif

		sse_mul((int16 *)testvectc, (int16 *)testvectb, VECTSIZE*2);
#if VERBOSE
		fprintf(stdout, "sse2:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d+j%d\t",testvectc[lcv2].i,testvectc[lcv2].q);
		fprintf(stdout, "\n");
#endif

		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			if((testvecta[lcv2].i != testvectc[lcv2].i)||(testvecta[lcv2].q != testvectc[lcv2].q))
					err++;
	}

	if(err)
		fprintf(stdout,"INT16 MUL \t\t\tFAILED: %d errors\n",err);
	else
		fprintf(stdout,"INT16 MUL \t\t\tPASSED\n");
}
/*----------------------------------------------------------------------------------------------*/


/* SIMD DOT */
/*----------------------------------------------------------------------------------------------*/
void test_dot(){
	int32 err, lcv, lcv2;
	int32 val1, val2;
	
	err = 0;

	for(lcv = 0; lcv < REPEATS; lcv++)
	{
		/* generate two different random vectors */
		CPX *longvect;
		longvect = new CPX[2*VECTSIZE];
		fill_vect(longvect, 2*VECTSIZE);
		memcpy(testvecta, longvect, VECTSIZE*sizeof(CPX));
		memcpy(testvectb, &longvect[VECTSIZE], VECTSIZE*sizeof(CPX));

#if VERBOSE
		fprintf(stdout, "A:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "B:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectb[lcv2].i, 
				(testvectb[lcv2].q<0?'-':'+'), abs(testvectb[lcv2].q));
		fprintf(stdout, "\n");
#endif

		memcpy(testvectc, testvecta, VECTSIZE*sizeof(CPX));

		val1 = x86_dot((int16 *)testvecta, (int16 *)testvectb, VECTSIZE*2);
#if VERBOSE
		fprintf(stdout, "x86: %d\n", val1);
#endif

		val2 = sse_dot((int16 *)testvecta, (int16 *)testvectb, VECTSIZE*2);
#if VERBOSE
		fprintf(stdout, "sse2: %d\n", val2);
#endif

		if(val1 != val2)
			err++;
	}

	if(err)
		fprintf(stdout,"INT16 DOT \t\t\tFAILED: %d errors\n",err);
	else
		fprintf(stdout,"INT16 DOT \t\t\tPASSED\n");
}
/*----------------------------------------------------------------------------------------------*/


/* SIMD CONJ */
/*----------------------------------------------------------------------------------------------*/
void test_conj(){
	int32 err, lcv, lcv2;
	err = 0;

	for(lcv = 0; lcv < REPEATS; lcv++)
	{
		fill_vect(testvecta, VECTSIZE);
		memcpy(testvectb, testvecta, VECTSIZE*sizeof(CPX));
		
#if VERBOSE
		fprintf(stdout, "A:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
#endif

		x86_conj(testvecta, VECTSIZE);
#if VERBOSE
		fprintf(stdout, "x86:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d+j%d\t", testvecta[lcv2].i,testvecta[lcv2].q);
		fprintf(stdout, "\n");
#endif

		sse_conj(testvectb, VECTSIZE);
#if VERBOSE
		fprintf(stdout, "sse2:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d+j%d\t",testvectb[lcv2].i,testvectb[lcv2].q);
		fprintf(stdout, "\n");
#endif

		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			if((testvecta[lcv2].i != testvectb[lcv2].i)||(testvecta[lcv2].q != testvectb[lcv2].q))
					err++;
	}

	if(err)
		fprintf(stdout,"CPX CONJ \t\t\tFAILED: %d errors\n",err);
	else
		fprintf(stdout,"CPX CONJ \t\t\tPASSED\n");
}
/*----------------------------------------------------------------------------------------------*/


/* SIMD CMUL */
/*----------------------------------------------------------------------------------------------*/
void test_cmul(){
	int32 err, lcv, lcv2;
	err = 0;

	for(lcv = 0; lcv < REPEATS; lcv++)
	{
		/* generate two different random vectors */
		CPX *longvect;
		longvect = new CPX[2*VECTSIZE];
		fill_vect(longvect, 2*VECTSIZE);
		memcpy(testvecta, longvect, VECTSIZE*sizeof(CPX));
		memcpy(testvectb, &longvect[VECTSIZE], VECTSIZE*sizeof(CPX));

#if VERBOSE
		fprintf(stdout, "A:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "B:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectb[lcv2].i, 
				(testvectb[lcv2].q<0?'-':'+'), abs(testvectb[lcv2].q));
		fprintf(stdout, "\n");
#endif

		memcpy(testvectc, testvecta, VECTSIZE*sizeof(CPX));

		x86_cmul(testvecta, testvectb, VECTSIZE);
#if VERBOSE
		fprintf(stdout, "x86:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d+j%d\t", testvecta[lcv2].i,testvecta[lcv2].q);
		fprintf(stdout, "\n");
#endif

		sse_cmul(testvectc, testvectb, VECTSIZE);
#if VERBOSE
		fprintf(stdout, "sse2:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d+j%d\t",testvectc[lcv2].i,testvectc[lcv2].q);
		fprintf(stdout, "\n");
#endif

		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			if((testvecta[lcv2].i != testvectc[lcv2].i)||(testvecta[lcv2].q != testvectc[lcv2].q))
					err++;
	}

	if(err)
		fprintf(stdout,"CPX MUL \t\t\tFAILED: %d errors\n",err);
	else
		fprintf(stdout,"CPX MUL \t\t\tPASSED\n");
}
/*----------------------------------------------------------------------------------------------*/


/* SIMD CMUL w SHIFT*/
/*----------------------------------------------------------------------------------------------*/
void test_cmuls(){
	int32 err, lcv, lcv2;
	int32 shift=3;
	err = 0;

	for(lcv = 0; lcv < REPEATS; lcv++)
	{
		/* generate two different random vectors */
		CPX *longvect;
		longvect = new CPX[2*VECTSIZE];
		fill_vect(longvect, 2*VECTSIZE);
		memcpy(testvecta, longvect, VECTSIZE*sizeof(CPX));
		memcpy(testvectb, &longvect[VECTSIZE], VECTSIZE*sizeof(CPX));

#if VERBOSE
		fprintf(stdout, "A:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "B:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectb[lcv2].i, 
				(testvectb[lcv2].q<0?'-':'+'), abs(testvectb[lcv2].q));
		fprintf(stdout, "\n");
#endif

		memcpy(testvectc, testvecta, VECTSIZE*sizeof(CPX));

		x86_cmuls(testvecta, testvectb, VECTSIZE, shift);
#if VERBOSE
		fprintf(stdout, "x86:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
#endif

		sse_cmuls(testvectc, testvectb, VECTSIZE, shift);
#if VERBOSE
		fprintf(stdout, "sse2:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectc[lcv2].i, 
				(testvectc[lcv2].q<0?'-':'+'), abs(testvectc[lcv2].q));
		fprintf(stdout, "\n");
#endif

		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			if((testvecta[lcv2].i != testvectc[lcv2].i)||(testvecta[lcv2].q != testvectc[lcv2].q))
					err++;
	}

	if(err)
		fprintf(stdout,"CPX MUL SHIFT \t\t\tFAILED: %d errors\n",err);
	else
		fprintf(stdout,"CPX MUL SHIFT \t\t\tPASSED\n");
}
/*----------------------------------------------------------------------------------------------*/


/* SIMD CMUL w SHIFT and Preserve */
/*----------------------------------------------------------------------------------------------*/
void test_cmulsc(){
	int32 err, lcv, lcv2;
	int32 shift=1;
	err = 0;

	for(lcv = 0; lcv < REPEATS; lcv++)
	{
		/* generate two different random vectors */
		CPX *longvect;
		longvect = new CPX[2*VECTSIZE];
		fill_vect(longvect, 2*VECTSIZE);
		memcpy(testvecta, longvect, VECTSIZE*sizeof(CPX));
		memcpy(testvectb, &longvect[VECTSIZE], VECTSIZE*sizeof(CPX));

#if VERBOSE
		fprintf(stdout, "A:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "B:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectb[lcv2].i, 
				(testvectb[lcv2].q<0?'-':'+'), abs(testvectb[lcv2].q));
		fprintf(stdout, "\n");
#endif

		x86_cmulsc(testvecta, testvectb, testvectc, VECTSIZE, shift);
#if VERBOSE
		fprintf(stdout, "x86:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectc[lcv2].i, 
				(testvectc[lcv2].q<0?'-':'+'), abs(testvectc[lcv2].q));
		fprintf(stdout, "\n");
#endif

		sse_cmulsc(testvecta, testvectb, testvectd, VECTSIZE, shift);
#if VERBOSE
		fprintf(stdout, "sse2:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectd[lcv2].i,
				(testvectd[lcv2].q<0?'-':'+'), abs(testvectd[lcv2].q));
		fprintf(stdout, "\n");
#endif

		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			if((testvectc[lcv2].i != testvectd[lcv2].i)||(testvectc[lcv2].q != testvectd[lcv2].q))
					err++;
	}

	if(err)
		fprintf(stdout,"CPX MUL SHIFT PRESERVE \t\tFAILED: %d errors\n",err);
	else
		fprintf(stdout,"CPX MUL SHIFT PRESERVE \t\tPASSED\n");
}
/*----------------------------------------------------------------------------------------------*/


/* SIMD CACC */
/*----------------------------------------------------------------------------------------------*/
void test_cacc(){
	int32 err, lcv, lcv2;
	int32 ai1, aq1;
	int32 ai2, aq2;

	err = 0;

	for(lcv = 0; lcv < REPEATS; lcv++)
	{
		/* generate two different random vectors */
		CPX *longvect;
		longvect = new CPX[3*VECTSIZE];
		fill_vect(longvect, 3*VECTSIZE);
		memcpy(testvecta, longvect, VECTSIZE*sizeof(CPX));
		memcpy(testvectf, &longvect[VECTSIZE], VECTSIZE*sizeof(MIX));

#if VERBOSE
		fprintf(stdout, "A:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "B:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d, %d%cj%d\t", testvectf[lcv2].i, 
				(testvectf[lcv2].q<0?'-':'+'), abs(testvectf[lcv2].q),
				testvectf[lcv2].ni, (testvectf[lcv2].nq<0?'-':'+'), 
				abs(testvectf[lcv2].nq));
		fprintf(stdout, "\n");
#endif

		memcpy(testvectc, testvecta, VECTSIZE*sizeof(CPX));

		x86_cacc(testvecta, testvectf, VECTSIZE/2, &ai1, &aq1);
		sse_cacc(testvecta, testvectf, VECTSIZE/2, &ai2, &aq2);
#if VERBOSE
		fprintf(stdout,"Low32: %8x,%8x\n",ai1, ai2);
		fprintf(stdout,"High32: %8x,%8x\n",aq1, aq2);
#endif

		if(ai1 != ai2)
			err++;
		if(aq1 != aq2)
			err++;
	}

	if(err)
		fprintf(stdout,"CPX ACCUM \t\t\tFAILED: %d errors\n",err);
	else
		fprintf(stdout,"CPX ACCUM \t\t\tPASSED\n");
}
/*----------------------------------------------------------------------------------------------*/


/* SIMD x86_prn_accum new */
/*----------------------------------------------------------------------------------------------*/
void test_prn_accum_new(){
	int32 err, lcv, lcv2;
	err = 0;

	for(lcv = 0; lcv < REPEATS; lcv++)
	{
		CPX_ACCUM caccuma[3];
		CPX_ACCUM caccumb[3];

		fill_vect(testvecta, VECTSIZE);
		fill_prn_new(testvectf, VECTSIZE);
		fill_prn_new(testvectg, VECTSIZE);
		fill_prn_new(testvecth, VECTSIZE);
#if VERBOSE
		fprintf(stdout, "A:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecta[lcv2].i, 
				(testvecta[lcv2].q<0?'-':'+'), abs(testvecta[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "E:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectf[lcv2].i, 
				(testvectf[lcv2].q<0?'-':'+'), abs(testvectf[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "P:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvectg[lcv2].i, 
				(testvectg[lcv2].q<0?'-':'+'), abs(testvectg[lcv2].q));
		fprintf(stdout, "\n");
		fprintf(stdout, "L:\t");
		for(lcv2 = 0; lcv2 < VECTSIZE; lcv2++)
			fprintf(stdout, "%d%cj%d\t", testvecth[lcv2].i, 
				(testvecth[lcv2].q<0?'-':'+'), abs(testvecth[lcv2].q));
		fprintf(stdout, "\n");
#endif

		x86_prn_accum_new(testvecta, testvectf, testvectg, testvecth, VECTSIZE, &caccuma[0]);
		sse_prn_accum_new(testvecta, testvectf, testvectg, testvecth, VECTSIZE, &caccumb[0]);

		for(lcv2 = 0; lcv2 < 3; lcv2++)
			if((caccuma[lcv2].i != caccumb[lcv2].i)||(caccuma[lcv2].q != caccumb[lcv2].q))
				err++;

		if(err){
			fprintf(stdout,"E(x86,sse): %d%cj%d,%d%cj%d\n", caccuma[0].i, 
				(caccuma[0].q<0?'-':'+'), abs(caccuma[0].q), caccumb[0].i, 
				(caccumb[0].q<0?'-':'+'), abs(caccumb[0].q));
			fprintf(stdout,"P(x86,sse): %d%cj%d,%d%cj%d\n",  caccuma[1].i, 
				(caccuma[1].q<0?'-':'+'), abs(caccuma[1].q), caccumb[1].i, 
				(caccumb[1].q<0?'-':'+'), abs(caccumb[1].q));
			fprintf(stdout,"L(x86,sse): %d%cj%d,%d%cj%d\n",  caccuma[2].i, 
				(caccuma[2].q<0?'-':'+'), abs(caccuma[2].q), caccumb[2].i, 
				(caccumb[2].q<0?'-':'+'), abs(caccumb[2].q));
		}
	}

	if(err)
		fprintf(stdout,"CPX PRN ACCUM NEW \t\tFAILED: %d errors\n",err);
	else
		fprintf(stdout,"CPX PRN ACCUM NEW\t\tPASSED\n");
}
/*----------------------------------------------------------------------------------------------*/


/* HELPING FUNCTIONS */
/*----------------------------------------------------------------------------------------------*/
void fill_vect(CPX *_vect, int32 _samps)
{
	int32 lcv;

	/* initialize random seed: */
  	srand(time(NULL));

	/* Fill with values capped to +- 2^15 */
	for(lcv = 0; lcv < _samps; lcv++)
	{
//		_vect[lcv].i = (int16)((rand() & 0x7fff) - 0x4000);
//		_vect[lcv].q = (int16)((rand() & 0x7fff) - 0x4000);

		_vect[lcv].i = (int16)((rand() % 32) - 16);
		_vect[lcv].q = (int16)((rand() % 32) - 16);
	}
}


void fill_prn(CPX *_vect, int32 _samps)
{
	int32 lcv;

	/* initialize random seed: */
  	//srand(time(NULL));

	/* Fill with values 0 or -1 */
	for(lcv = 0; lcv < _samps; lcv++)
	{
		if((rand() & 0x1) == 0)
			_vect[lcv].i = _vect[lcv].q = 0xffff;
		else
			_vect[lcv].i = _vect[lcv].q = 0x0000;
	}
}


void fill_prn_new(MIX *_vect, int32 _samps)
{
	int32 lcv;

	/* initialize random seed: */
  	//srand(time(NULL));

	/* Fill with values 0 or -1 */
	for(lcv = 0; lcv < _samps; lcv++)
	{
		if(rand() & 0x1)
			_vect[lcv].i = _vect[lcv].ni = 0x0001;
		else
			_vect[lcv].i = _vect[lcv].ni = 0xffff;
		_vect[lcv].q =  _vect[lcv].nq = 0;
	}
}
