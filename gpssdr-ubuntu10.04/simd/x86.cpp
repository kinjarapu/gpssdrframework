/*! \file x86.cpp
	Emulate SIMD functionality with plain x86 crap, for older processors
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

Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA	02110-1301	USA
************************************************************************************************/


#include "includes.h"


/*----------------------------------------------------------------------------------------------*/
void x86_add(int16_t *_A, int16_t *_B, int32_t _cnt)
{

	int16_t *a = _A;
	int16_t *b = _B;
	int32_t lcv;

	for(lcv = 0; lcv < _cnt; lcv++)
		a[lcv] += b[lcv];

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_sub(int16_t *_A, int16_t *_B, int32_t _cnt)
{

	int16_t *a = _A;
	int16_t *b = _B;
	int32_t lcv;

	for(lcv = 0; lcv < _cnt; lcv++)
		a[lcv] -= b[lcv];
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_mul(int16_t *_A, int16_t *_B, int32_t _cnt)
{

	int16_t *a = _A;
	int16_t *b = _B;
	int32_t lcv;

	for(lcv = 0; lcv < _cnt; lcv++)
		a[lcv] *= b[lcv];
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_muls(int16_t *_A, int16_t *_B, int32_t _cnt, int32_t _shift)
{

	int32_t lcv;

	for(lcv = 0; lcv < _cnt; lcv++)
	{
		_A[lcv] *= *_B;
		_A[lcv] >>= _shift;
	}
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
int32_t x86_dot(int16_t *_A, int16_t *_B, int32_t _cnt)
{

	int32_t accum = 0;
	int16_t *a = _A;
	int16_t *b = _B;
	int32_t lcv;

	for(lcv = 0; lcv < _cnt; lcv++)
		accum += (int32_t)a[lcv]*(int32_t)b[lcv];

	return(accum);
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_conj(CPX *_A, int32_t _cnt)
{
	int32_t lcv;

	for(lcv = 0; lcv < _cnt; lcv++)
		_A[lcv].q = -_A[lcv].q;

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_cmul(CPX *_A, CPX *_B, int32_t _cnt)
{

	int32_t lcv;
	int32_t ai, aq;
	int32_t bi, bq;
	int32_t ti, tq;

	for(lcv = 0; lcv < _cnt; lcv++)
	{

		ai = _A[lcv].i;
		aq = _A[lcv].q;
		bi = _B[lcv].i;
		bq = _B[lcv].q;

		ti = ai*bi-aq*bq;
		tq = ai*bq+aq*bi;

		_A[lcv].i = (int16_t)(ti);
		_A[lcv].q = (int16_t)(tq);
	}

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_cmuls(CPX *_A, CPX *_B, int32_t _cnt, int32_t _shift)
{

	int32_t lcv;
	int32_t ai, aq;
	int32_t bi, bq;
	int32_t ti, tq;
	int32_t shift;
	int32_t round;

	shift = _shift;
	round = 1 << (shift-1);

	for(lcv = 0; lcv < _cnt; lcv++)
	{

		ai = _A[lcv].i;
		aq = _A[lcv].q;
		bi = _B[lcv].i;
		bq = _B[lcv].q;

		ti = ai*bi-aq*bq;
		tq = ai*bq+aq*bi;

		ti += round;
		tq += round;

		ti >>= shift;
		tq >>= shift;

		_A[lcv].i = (int16_t)ti;
		_A[lcv].q = (int16_t)tq;
	}

}
/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/
void x86_cmulsc(CPX *_A, CPX *_B, CPX *_C, int32_t _cnt, int32_t _shift)
{

	int32_t lcv;
	int32_t ai, aq;
	int32_t bi, bq;
	int32_t ti, tq;
	int32_t shift;
	int32_t round;

	shift = _shift;
	round = 1 << (shift-1);

	for(lcv = 0; lcv < _cnt; lcv++)
	{

		ai = _A[lcv].i;
		aq = _A[lcv].q;
		bi = _B[lcv].i;
		bq = _B[lcv].q;

		ti = ai*bi-aq*bq;
		tq = ai*bq+aq*bi;

		ti += round;
		tq += round;

		ti >>= shift;
		tq >>= shift;

		_C[lcv].i = (int16_t)ti;
		_C[lcv].q = (int16_t)tq;
	}

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_cacc(CPX *_A, MIX *_B, int32_t _cnt, int32_t *_iaccum, int32_t *_qaccum)
{


	int32_t lcv;
	int32_t ai, aq;
	int32_t bi, bq;
	int32_t ti, tq;
	int32_t iaccum, qaccum;
	int64_t value;

	iaccum = qaccum = 0;

	for(lcv = 0; lcv < _cnt; lcv++)
	{

		ai = _A[lcv].i;
		aq = _A[lcv].q;

		ti = ai*_B[lcv].i+aq*_B[lcv].nq;
		tq = ai*_B[lcv].q+aq*_B[lcv].ni;

		iaccum += ti;
		qaccum += tq;

	}

	*_iaccum = iaccum;
	*_qaccum = qaccum;

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_cmag(CPX *_A, int32_t _cnt)
{
	int32_t lcv, *p;

	p = (int32_t *)_A;

	for(lcv = 0; lcv < _cnt; lcv++)
	{
		*p = (int32_t) (_A[lcv].i*_A[lcv].i + _A[lcv].q*_A[lcv].q);
		p++;
	}
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_cmag_new(CPX *_A, int32_t *_B, int32_t _cnt)
{
	for(int32_t lcv = 0; lcv < _cnt; lcv++)
	{
		*_B = (int32_t) (_A[lcv].i*_A[lcv].i + _A[lcv].q*_A[lcv].q);
		_B++;
	}
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_max(int32_t *_A, int32_t *_index, int32_t *_magt, int32_t _cnt)
{

	int32_t lcv, mag, index;

	mag = index = 0;

	for(lcv = 0; lcv < _cnt; lcv++)
	{
		if(_A[lcv] > mag)
		{
			index = lcv;
			mag = _A[lcv];
		}
	}

	*_index = index;
	*_magt = mag;

}
/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/
void x86_cavg(CPX *_A, int32_t *_magt_i, int32_t *_magt_q, int32_t _cnt)
{

	int32_t lcv,mag_i,mag_q,avg_i,avg_q;


	mag_i = 0;avg_i = 0;
        mag_q = 0;avg_q = 0;

	for(lcv = 0; lcv < _cnt; lcv++)
	{
		if(_A[lcv].i > 0)
			mag_i += (_A[lcv].i);
                else
			mag_i += -1*(_A[lcv].i);

                if(_A[lcv].q > 0)
			mag_q += _A[lcv].q;
		else
			mag_q += -1*(_A[lcv].q);
	}

        avg_i = floor(mag_i/_cnt);
        avg_q = floor(mag_q/_cnt);

	*_magt_i = avg_i;
        *_magt_q = avg_q;
}
/*----------------------------------------------------------------------------------------------*/






/*----------------------------------------------------------------------------------------------*/
void x86_cmax(CPX *_A, int32_t *_magt, int32_t _cnt)
{

	int32_t lcv, mag;

	mag = 0;

	for(lcv = 0; lcv < _cnt; lcv++)
	{
		if(_A[lcv].i > mag)
			mag = _A[lcv].i;
		if(_A[lcv].q > mag)
			mag = _A[lcv].q;
	}

	*_magt = mag;

}
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
void x86_prn_accum(CPX *A, CPX *E, CPX *P, CPX *L, int32_t cnt, CPX *accum)	//!< This is a long story
{

	CPX Ea, Pa, La;
	int32_t lcv;

	Ea.i = 0;
	Ea.q = 0;
	Pa.i = 0;
	Pa.q = 0;
	La.i = 0;
	La.q = 0;

	for(lcv = 0; lcv < cnt; lcv++)
	{
		if(E[lcv].i < 0)
		{
			Ea.i -= A[lcv].i;
			Ea.q -= A[lcv].q;
		}
		else
		{
			Ea.i += A[lcv].i;
			Ea.q += A[lcv].q;
		}

		if(P[lcv].i < 0)
		{
			Pa.i -= A[lcv].i;
			Pa.q -= A[lcv].q;
		}
		else
		{
			Pa.i += A[lcv].i;
			Pa.q += A[lcv].q;
		}

		if(L[lcv].i < 0)
		{
			La.i -= A[lcv].i;
			La.q -= A[lcv].q;
		}
		else
		{
			La.i += A[lcv].i;
			La.q += A[lcv].q;
		}
	}

	accum[0].i = Ea.i;
	accum[0].q = Ea.q;
	accum[1].i = Pa.i;
	accum[1].q = Pa.q;
	accum[2].i = La.i;
	accum[2].q = La.q;

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void x86_prn_accum_new(CPX *A, MIX *E, MIX *P, MIX *L, int32_t cnt, CPX_ACCUM *accum)
{

	CPX_ACCUM Ea, Pa, La;
	int32_t lcv;

	Ea.i = 0;	Ea.q = 0;
	Pa.i = 0;	Pa.q = 0;
	La.i = 0;	La.q = 0;

	for(lcv = 0; lcv < cnt; lcv++)
	{
		Ea.i += A[lcv].i*E[lcv].i;
		Ea.q += A[lcv].q*E[lcv].ni;
		Pa.i += A[lcv].i*P[lcv].i;
		Pa.q += A[lcv].q*P[lcv].ni;
		La.i += A[lcv].i*L[lcv].i;
		La.q += A[lcv].q*L[lcv].ni;
	}

	accum[0].i = Ea.i;
	accum[0].q = Ea.q;
	accum[1].i = Pa.i;
	accum[1].q = Pa.q;
	accum[2].i = La.i;
	accum[2].q = La.q;

}
/*----------------------------------------------------------------------------------------------*/
