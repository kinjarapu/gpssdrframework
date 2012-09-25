/*! \file sse.cpp
  SIMD functionality, mainly for 32 bit interleaved complex integer type (CPX)
*/

/************************************************************************************************
Copyright 2010 Michele Bavaro

This file is covered by a beerware licence.

http://michelebavaro.blogspot.com
************************************************************************************************/

#include <stdint.h>
#include "includes.h"

void sse_add(int16_t *_A, int16_t *_B, int32_t cnt)
{

  int32_t cnt1;
  int32_t cnt2;

  cnt1 = cnt / 8;
  cnt2 = (cnt - (8*cnt1));

  if(((intptr_t)_A%16) || ((intptr_t)_B%16)) {
     // unaligned version

    __asm__ __volatile__
    (
     "CMP          $0,   %[CNT1]   \n\t"
     "JNZ Z%=                      \n\t"
     "L%=:                         \n\t"
     "MOVUPD   (%[A]),  %%xmm0     \n\t" //Load from A
     "MOVUPD   (%[B]),  %%xmm1     \n\t" //Load from B
     "PADDW    %%xmm1,  %%xmm0     \n\t" //Add A+B
     "MOVUPD   %%xmm0,  (%[A])     \n\t" //Move into A
     "ADD         $16,   %[A]      \n\t"
     "ADD         $16,   %[B]      \n\t"
     "SUB          $1,   %[CNT1]   \n\t"
     "JNZ L%=                      \n\t" //Loop if not done
     "Z%=:                         \n\t"

     "CMP          $0,   %[CNT2]   \n\t"
     "JNZ ZZ%=                     \n\t"
     "LL%=:                        \n\t" //Really finish off loop with non SIMD instructions
     "MOV      (%[A]),  %%ax       \n\t"
     "ADD      (%[B]),  %%ax       \n\t"
     "MOV      %%ax,    (%[A])     \n\t"
     "ADD          $2,   %[A]      \n\t"
     "ADD          $2,   %[B]      \n\t"
     "SUB          $1,   %[CNT2]   \n\t"
     "JNZ LL%=                     \n\t" //Loop if not done
     "ZZ%=:                        \n\t"
     : [A] "+r" (_A), [B] "+r" (_B), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     :: "memory"
    );

  } else {

    __asm__ __volatile__
    (
     "CMP          $0,   %[CNT1]   \n\t"
     "JE Z%=                       \n\t"
     "L%=:                         \n\t"
     "MOVAPD   (%[A]),  %%xmm0     \n\t" //Load from A
     "MOVAPD   (%[B]),  %%xmm1     \n\t" //Load from B
     "PADDW    %%xmm1,  %%xmm0     \n\t" //Add A+B
     "MOVAPD   %%xmm0,  (%[A])     \n\t" //Move into A
     "ADD         $16,   %[A]      \n\t"
     "ADD         $16,   %[B]      \n\t"
     "SUB          $1,   %[CNT1]   \n\t"
     "JNZ L%=                      \n\t" //Loop if not done
     "Z%=:                         \n\t"

     "CMP          $0,   %[CNT2]   \n\t"
     "JE ZZ%=                      \n\t"
     "LL%=:                        \n\t" //Really finish off loop with non SIMD instructions
     "MOV      (%[A]),  %%ax       \n\t"
     "ADD      (%[B]),  %%ax       \n\t"
     "MOV      %%ax,    (%[A])     \n\t"
     "ADD          $2,   %[A]      \n\t"
     "ADD          $2,   %[B]      \n\t"
     "SUB          $1,   %[CNT2]   \n\t"
     "JNZ LL%=                     \n\t" //Loop if not done
     "ZZ%=:                        \n\t"
     : [A] "+r" (_A), [B] "+r" (_B), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     :: "memory"
    );

  }//end if
}


void sse_sub(int16_t *_A, int16_t *_B, int32_t _cnt)
{

  int32_t cnt1;
  int32_t cnt2;

  cnt1 = _cnt / 8;
  cnt2 = (_cnt - (8*cnt1));

 if(((intptr_t)_A%16) || ((intptr_t)_B%16)) {
   // unaligned version
    __asm__ __volatile__
    (
     "CMP          $0,   %[CNT1]   \n\t"
     "JNZ Z%=                      \n\t"
     "L%=:                         \n\t"
     "MOVUPD   (%[A]),  %%xmm0     \n\t" //Load from A
     "MOVUPD   (%[B]),  %%xmm1     \n\t" //Load from B
     "PSUBW    %%xmm1,  %%xmm0     \n\t" //Subtract A-B
     "MOVUPD   %%xmm0,  (%[A])     \n\t" //Move into A
     "ADD         $16,   %[A]      \n\t"
     "ADD         $16,   %[B]      \n\t"
     "SUB          $1,   %[CNT1]   \n\t"
     "JNZ L%=                      \n\t" //Loop if not done
     "Z%=:                         \n\t"

     "CMP          $0,   %[CNT2]   \n\t"
     "JNZ ZZ%=                     \n\t"
     "LL%=:                        \n\t" //Really finish off loop with non SIMD instructions
     "MOV      (%[A]),  %%ax       \n\t"
     "SUB      (%[B]),  %%ax       \n\t"
     "MOV      %%ax,    (%[A])     \n\t"
     "ADD          $2,   %[A]      \n\t"
     "ADD          $2,   %[B]      \n\t"
     "SUB          $1,   %[CNT2]   \n\t"
     "JNZ LL%=                     \n\t" //Loop if not done
     "ZZ%=:                        \n\t"
     : [A] "+r" (_A), [B] "+r" (_B), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     :: "memory"
    );

  } else {

    __asm__ __volatile__
    (
     "CMP          $0,   %[CNT1]   \n\t"
     "JE Z%=                       \n\t"
     "L%=:                         \n\t"
     "MOVAPD   (%[A]),  %%xmm0     \n\t" //Load from A
     "MOVAPD   (%[B]),  %%xmm1     \n\t" //Load from B
     "PSUBW    %%xmm1,  %%xmm0     \n\t" //Subtract A-B
     "MOVAPD   %%xmm0,  (%[A])     \n\t" //Move into A
     "ADD         $16,   %[A]      \n\t"
     "ADD         $16,   %[B]      \n\t"
     "SUB          $1,   %[CNT1]   \n\t"
     "JNZ L%=                      \n\t" //Loop if not done
     "Z%=:                         \n\t"

     "CMP          $0,   %[CNT2]   \n\t"
     "JE ZZ%=                      \n\t"
     "LL%=:                        \n\t" //Really finish off loop with non SIMD instructions
     "MOV      (%[A]),  %%ax       \n\t"
     "SUB      (%[B]),  %%ax       \n\t"
     "MOV      %%ax,    (%[A])     \n\t"
     "ADD          $2,   %[A]      \n\t"
     "ADD          $2,   %[B]      \n\t"
     "SUB          $1,   %[CNT2]   \n\t"
     "JNZ LL%=                     \n\t" //Loop if not done
     "ZZ%=:                        \n\t"
     : [A] "+r" (_A), [B] "+r" (_B), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     :: "memory"
    );

  }//end if
}

void sse_mul(int16_t *_A, int16_t *_B, int32_t cnt)
{

  int32_t cnt1;
  int32_t cnt2;

  cnt1 = cnt / 8;
  cnt2 = (cnt - (8*cnt1));

  if(((intptr_t)_A%16) || ((intptr_t)_B%16)) {
    // unaligned version

    __asm__ __volatile__
    (
     "CMP          $0,   %[CNT1]   \n\t"
     "JNZ Z%=                      \n\t"
     "L%=:                         \n\t"
     "MOVUPD   (%[A]),  %%xmm0     \n\t" //Load from A
     "MOVUPD   (%[B]),  %%xmm1     \n\t" //Load from B
     "PMULLW   %%xmm1,  %%xmm0     \n\t" //Multiply A*B
     "MOVUPD   %%xmm0,  (%[A])     \n\t" //Move into A
     "ADD         $16,   %[A]      \n\t"
     "ADD         $16,   %[B]      \n\t"
     "SUB          $1,   %[CNT1]   \n\t"
     "JNZ L%=                      \n\t" //Loop if not done
     "Z%=:                         \n\t"

     "CMP          $0,   %[CNT2]   \n\t"
     "JNZ ZZ%=                     \n\t"
     "LL%=:                        \n\t" //Really finish off loop with non SIMD instructions
     "MOV      (%[A]),  %%ax       \n\t"
     "IMUL     (%[B]),  %%ax       \n\t"
     "MOV      %%ax,    (%[A])     \n\t"
     "ADD          $2,   %[A]      \n\t"
     "ADD          $2,   %[B]      \n\t"
     "SUB          $1,   %[CNT2]   \n\t"
     "JNZ LL%=                     \n\t" //Loop if not done
     "ZZ%=:                        \n\t"
     : [A] "+r" (_A), [B] "+r" (_B), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     :: "memory"
    );

  } else {

    __asm__ __volatile__
    (
     "CMP          $0,   %[CNT1]   \n\t"
     "JE Z%=                       \n\t"
     "L%=:                         \n\t"
     "MOVAPD   (%[A]),  %%xmm0     \n\t" //Load from A
     "MOVAPD   (%[B]),  %%xmm1     \n\t" //Load from B
     "PMULLW   %%xmm1,  %%xmm0     \n\t" //Multiply A*B
     "MOVAPD   %%xmm0,  (%[A])     \n\t" //Move into A
     "ADD         $16,   %[A]      \n\t"
     "ADD         $16,   %[B]      \n\t"
     "SUB          $1,   %[CNT1]   \n\t"
     "JNZ L%=                      \n\t" //Loop if not done
     "Z%=:                         \n\t"

     "CMP          $0,   %[CNT2]   \n\t"
     "JE ZZ%=                      \n\t"
     "LL%=:                        \n\t" //Really finish off loop with non SIMD instructions
     "MOV      (%[A]),  %%ax       \n\t"
     "IMUL     (%[B]),  %%ax       \n\t"
     "MOV      %%ax,    (%[A])     \n\t"
     "ADD          $2,   %[A]      \n\t"
     "ADD          $2,   %[B]      \n\t"
     "SUB          $1,   %[CNT2]   \n\t"
     "JNZ LL%=                     \n\t" //Loop if not done
     "ZZ%=:                        \n\t"
     : [A] "+r" (_A), [B] "+r" (_B), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     :: "memory"
    );

  }//end if

}


int32_t sse_dot(int16_t *_A, int16_t *_B, int32_t cnt)
{

  int32_t cnt1;
  int32_t cnt2;
  int32_t temp;

  if(((intptr_t)_A%16) || ((intptr_t)_B%16))    //If the memory locations are not 16 byte aligned use slower movupd instruction
    {
      //fprintf(stderr, "non aligned version\n");
      cnt1 = cnt / 24;
      cnt2 = (cnt - (24*cnt1));

    __asm__ __volatile__
    (
     "PXOR      %%xmm0,  %%xmm0     \n\t"
     "CMP           $0,   %[CNT1]   \n\t"
     "JE Z%=                        \n\t"
     "L%=:                          \n\t"
     "MOVUPD    (%[A]),  %%xmm1     \n\t" //Load from A
     "MOVUPD  16(%[A]),  %%xmm2     \n\t" //Load from A
     "MOVUPD  32(%[A]),  %%xmm3     \n\t" //Load from A
     "MOVUPD    (%[B]),  %%xmm4     \n\t" //Load from B
     "MOVUPD  16(%[B]),  %%xmm5     \n\t" //Load from B
     "MOVUPD  32(%[B]),  %%xmm6     \n\t" //Load from B
     "PMADDWD   %%xmm4,  %%xmm1     \n\t" //Multiply and accumulate
     "PMADDWD   %%xmm5,  %%xmm2     \n\t" //Multiply and accumulate
     "PMADDWD   %%xmm6,  %%xmm3     \n\t" //Multiply and accumulate
     "PADDD     %%xmm3,  %%xmm1     \n\t" //Add into accumulator (efficiently)
     "PADDD     %%xmm2,  %%xmm0     \n\t" //
     "PADDD     %%xmm1,  %%xmm0     \n\t" //
     "ADD          $48,   %[A]      \n\t"
     "ADD          $48,   %[B]      \n\t"
     "SUB           $1,   %[CNT1]   \n\t"
     "JNZ L%=                       \n\t" //Loop if not done
     "Z%=:                          \n\t"
     "MOVD      %%xmm0,  %[RES]     \n\t" // First 32 bit
     "PSRLDQ        $4,  %%xmm0     \n\t"
     "MOVD      %%xmm0,  %%eax      \n\t" // Second 32 bit
     "ADD       %%eax ,  %[RES]     \n\t" // half running sum now in eax
     "PSRLDQ        $4,  %%xmm0     \n\t"
     "MOVD      %%xmm0,  %%eax      \n\t" // Third 32 bit
     "ADD       %%eax ,  %[RES]     \n\t" // half running sum now in eax
     "PSRLDQ        $4,  %%xmm0     \n\t"
     "MOVD      %%xmm0,  %%eax      \n\t" // Second 32 bit
     "ADD       %%eax ,  %[RES]     \n\t" // half running sum now in eax
     "CMP           $0,  %[CNT2]    \n\t"
     "JE ZZ%=                       \n\t"
     "LL%=:                         \n\t" //Really finish off loop with non SIMD instructions
     "MOV       (%[A]),  %%bx       \n\t" //Load 16 bit from A
     "MOVSX     %%bx  ,  %%ebx      \n\t" //
     "MOV       (%[B]),  %%ax       \n\t" //Load 16 bit from B
     "MOVSX     %%ax  ,  %%eax      \n\t" //
     "IMUL      %%eax ,  %%ebx      \n\t" //
     "ADD       %%ebx ,  %[RES]     \n\t" //final running sum still in edx
     "ADD           $2,   %[A]      \n\t"
     "ADD           $2,   %[B]      \n\t"
     "SUB           $1,   %[CNT2]   \n\t"
     "JNZ LL%=                      \n\t" //Loop if not done
     "ZZ%=:                         \n\t"
     : [RES] "=g"(temp), [A] "+r" (_A), [B] "+r" (_B), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     :: "%eax", "%ebx", "memory"
     );
    }
  else  //use faster movapd instruction
    {

      cnt1 = cnt / 56;
      cnt2 = (cnt - (56*cnt1));

    __asm__ __volatile__
    (
     "PXOR        %%xmm0,  %%xmm0     \n\t"
     "CMP             $0,   %[CNT1]   \n\t"
     "JE Z%=                          \n\t"
     "L%=:                            \n\t"
     "MOVAPD      (%[A]),  %%xmm1     \n\t" //Load from A
     "MOVAPD    16(%[A]),  %%xmm2     \n\t" //Load from A
     "MOVAPD    32(%[A]),  %%xmm3     \n\t" //Load from A
     "MOVAPD    48(%[A]),  %%xmm4     \n\t" //Load from A
     "MOVAPD    64(%[A]),  %%xmm5     \n\t" //Load from A
     "MOVAPD    80(%[A]),  %%xmm6     \n\t" //Load from A
     "MOVAPD    96(%[A]),  %%xmm7     \n\t" //Load from A
     "PMADDWD     (%[B]),  %%xmm1     \n\t" //Multiply and accumulate
     "PMADDWD   16(%[B]),  %%xmm2     \n\t" //Multiply and accumulate
     "PMADDWD   32(%[B]),  %%xmm3     \n\t" //Multiply and accumulate
     "PMADDWD   48(%[B]),  %%xmm4     \n\t" //Multiply and accumulate
     "PMADDWD   64(%[B]),  %%xmm5     \n\t" //Multiply and accumulate
     "PMADDWD   80(%[B]),  %%xmm6     \n\t" //Multiply and accumulate
     "PMADDWD   96(%[B]),  %%xmm7     \n\t" //Multiply and accumulate
     "PADDD       %%xmm1,  %%xmm0     \n\t" //Add 1 to 0
     "PADDD       %%xmm3,  %%xmm2     \n\t" //Add 3 to 2
     "PADDD       %%xmm5,  %%xmm4     \n\t" //Add 5 to 4
     "PADDD       %%xmm7,  %%xmm6     \n\t" //Add 7 to 6
     "PADDD       %%xmm2,  %%xmm0     \n\t" //Add 2 to 0
     "PADDD       %%xmm6,  %%xmm4     \n\t" //Add 6 to 4
     "PADDD       %%xmm4,  %%xmm0     \n\t" //Add 4 to 0
     "ADD           $112,   %[A]      \n\t"
     "ADD           $112,   %[B]      \n\t"
     "SUB             $1,   %[CNT1]   \n\t"
     "JNZ L%=                         \n\t" //Loop if not done
     "Z%=:                            \n\t"
     "MOVD        %%xmm0,  %[RES]     \n\t" // First 32 bit
     "PSRLDQ          $4,  %%xmm0     \n\t"
     "MOVD        %%xmm0,  %%eax      \n\t" // Second 32 bit
     "ADD         %%eax ,  %[RES]     \n\t" // add to RES
     "PSRLDQ          $4,  %%xmm0     \n\t"
     "MOVD        %%xmm0,  %%eax      \n\t" // Third 32 bit
     "ADD         %%eax ,  %[RES]     \n\t" // add to RES
     "PSRLDQ          $4,  %%xmm0     \n\t"
     "MOVD        %%xmm0,  %%eax      \n\t" // Last 32 bit
     "ADD         %%eax ,  %[RES]     \n\t" // add to RES
     "CMP             $0,  %[CNT2]    \n\t"
     "JE ZZ%=                         \n\t"
     "LL%=:                           \n\t" //Really finish off loop with non SIMD instructions
     "MOV         (%[A]),  %%bx       \n\t" //Load 16 bit from A
     "MOVSX       %%bx  ,  %%ebx      \n\t" //
     "MOV         (%[B]),  %%ax       \n\t" //Load 16 bit from B
     "MOVSX       %%ax  ,  %%eax      \n\t" //
     "IMUL        %%eax ,  %%ebx      \n\t" //
     "ADD         %%ebx ,  %[RES]     \n\t" //final running sum still in edx
     "ADD             $2,   %[A]      \n\t"
     "ADD             $2,   %[B]      \n\t"
     "SUB             $1,   %[CNT2]   \n\t"
     "JNZ LL%=                        \n\t" //Loop if not done
     "ZZ%=:                           \n\t"
     : [RES] "=g"(temp), [A] "+r" (_A), [B] "+r" (_B), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     :: "%eax", "%ebx", "memory"
     );
    }

  return(temp);

}

void sse_conj(CPX *_A, int32_t _cnt)
{

  int32_t cnt1;
  int32_t cnt2;
  int32_t temp = 0xffff0001; //[1, -1]

  cnt1 = _cnt/4;
  cnt2 = _cnt-4*cnt1;

  __asm__ __volatile__
    (
     "MOVD          %[M],   %%mm7      \n\t"
     "PUNPCKLDQ    %%mm7,   %%mm7      \n\t"
     "MOVD          %[M],   %%xmm7     \n\t"
     "PUNPCKLDQ   %%xmm7,   %%xmm7     \n\t"
     "PUNPCKLDQ   %%xmm7,   %%xmm7     \n\t"
     "CMP             $0,  %[CNT1]     \n\t"
     "JE   Z%=                         \n\t"
     "L%=:                             \n\t"
     "  MOVUPD      (%[A]),   %%xmm0     \n\t"
     "  PMULLW      %%xmm7,   %%xmm0     \n\t"
     "  MOVUPD      %%xmm0,   (%[A])     \n\t"
     "  ADD            $16,    %[A]      \n\t"
     "  SUB             $1,   %[CNT1]    \n\t"
     "  JNZ L%=                          \n\t" //Loop if not done
     "Z%=:                             \n\t"
     "CMP             $0,  %[CNT2]     \n\t"
     "JE  ZZ%=                         \n\t"
     "LL%=:                            \n\t"
     "  MOVD        (%[A]),   %%mm0      \n\t"
     "  PMULLW       %%mm7,   %%mm0      \n\t"
     "  MOVD         %%mm0,  (%[A])      \n\t"
     "  ADD             $4,   %[A]       \n\t"
     "  SUB             $1,   %[CNT2]    \n\t"
     "  JNZ LL%=                         \n\t" //Loop if not done
     "ZZ%=:                            \n\t"
     "EMMS                             \n\t"
     : [A] "+r" (_A), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     : [M] "m" (temp)
     : "memory"
     );

}

void sse_cmul(CPX *_A, CPX *_B, int32_t _cnt)
{

  int32_t cnt1;
  int32_t cnt2;

  volatile int32_t mul[4] = {0xffff0001, 0x00010001, 0xffff0001, 0x00010001}; //{1,-1,1,1,1,-1,1,1};

  cnt1 = _cnt / 4;
  cnt2 = _cnt - 4*cnt1;

  __asm__ __volatile__
    (
     "MOVUPD      (%[M]),         %%xmm7       \n\t" // Move the multiply thingie
     "CMP             $0,         %[CNT1]      \n\t"
     "JE   Z%=                                 \n\t"
     "L%=:                                     \n\t"
     "MOVQ         (%[A]),         %%xmm0       \n\t"
     "MOVQ        8(%[A]),         %%xmm1       \n\t"
     "MOVQ         (%[B]),         %%xmm2       \n\t"
     "MOVQ        8(%[B]),         %%xmm3       \n\t"
     "PUNPCKLDQ    %%xmm0,         %%xmm0       \n\t"
     "PUNPCKLDQ    %%xmm1,         %%xmm1       \n\t"
     "PUNPCKLDQ    %%xmm2,         %%xmm2       \n\t"
     "PUNPCKLDQ    %%xmm3,         %%xmm3       \n\t"
     "PSHUFLW      $0x14,  %%xmm2,  %%xmm2     \n\t"
     "PSHUFLW      $0x14,  %%xmm3,  %%xmm3     \n\t"
     "PSHUFHW      $0x14,  %%xmm2,  %%xmm2     \n\t"
     "PSHUFHW      $0x14,  %%xmm3,  %%xmm3     \n\t"
     "PMULLW       %%xmm7,        %%xmm2       \n\t"
     "PMULLW       %%xmm7,        %%xmm3       \n\t"
     "PMADDWD      %%xmm2,        %%xmm0       \n\t"
     "PMADDWD      %%xmm3,        %%xmm1       \n\t"
     "PACKSSDW     %%xmm0,        %%xmm0       \n\t"
     "PACKSSDW     %%xmm1,        %%xmm1       \n\t"
     "MOVQ         %%xmm0,        (%[A])       \n\t"
     "MOVQ         %%xmm1,       8(%[A])       \n\t"
     "ADD             $16,         %[A]        \n\t"
     "ADD             $16,         %[B]        \n\t"
     "SUB              $1,      %[CNT1]        \n\t"
     "JNZ L%=                                  \n\t" //Loop if not done
     "Z%=:                                     \n\t"
     "CMP              $0,   %[CNT2]           \n\t"
     "JE  ZZ%=                                 \n\t"
     "LL%=:                                    \n\t"
     "MOVD          (%[A]),       %%xmm0       \n\t"
     "MOVD          (%[B]),       %%xmm1       \n\t"
     "PUNPCKLDQ     %%xmm0,       %%xmm0       \n\t"
     "PUNPCKLDQ     %%xmm1,       %%xmm1       \n\t"
     "PSHUFLW      $0x14,  %%xmm1,  %%xmm1     \n\t"
     "PMULLW       %%xmm7,        %%xmm1       \n\t"
     "PMADDWD      %%xmm1,        %%xmm0       \n\t"
     "PACKSSDW     %%xmm0,        %%xmm0       \n\t"
     "MOVD         %%xmm0,        (%[A])       \n\t"
     "ADD              $4,         %[A]        \n\t"
     "ADD              $4,         %[B]        \n\t"
     "SUB              $1,      %[CNT2]        \n\t"
     "JNZ LL%=                                 \n\t" //Loop if not done
     "ZZ%=:                                    \n\t"
     : [A] "+r" (_A), [B] "+r" (_B), [CNT1] "+g" (cnt1), [CNT2] "+g" (cnt2)
     : [M] "r" (mul)
     : "%eax", "%ebx", "memory"
     );

}


void sse_cmuls(CPX *_A, CPX *_B, int32_t _cnt, int32_t _shift)
{

  int32_t cnt1;
  int32_t cnt2;
  int32_t round;

  volatile int32_t mul[4] = {0xffff0001, 0x00010001, 0xffff0001, 0x00010001}; //{1,-1,1,1,1,-1,1,1};

  cnt1 = _cnt/4;
  cnt2 = _cnt-4*cnt1;

  round = 1 << (_shift-1);

  __asm__ __volatile__
    (
     "movupd     (%[MUL]),   %%xmm7       \n\t" //Move the multiply thingie
     "movss       %[SH],     %%xmm6       \n\t" //Move the shift thingie
     "movss       %[R],      %%xmm5       \n\t" //Move the round thingie
     "punpckldq   %%xmm5,    %%xmm5       \n\t"
     "punpcklqdq  %%xmm5,    %%xmm5       \n\t"
     "CMP             $0,         %[CNT1]      \n\t"
     "JE   Z%=                                 \n\t"
     "L%=:                                     \n\t"
     "  movlpd      (%[A]),   %%xmm0        \n\t" //Copy from A
     "  movlpd     8(%[A]),   %%xmm1        \n\t" //Copy from A
     "  movlpd      (%[B]),   %%xmm3        \n\t" //Copy from B
     "  movlpd     8(%[B]),   %%xmm4        \n\t" //Copy from B
     "  punpckldq   %%xmm0,   %%xmm0        \n\t" //Copy low 32 bits to high 32 bits
     "  punpckldq   %%xmm1,   %%xmm1        \n\t" //Copy low 32 bits to high 32 bits
     "  punpckldq   %%xmm3,   %%xmm3        \n\t" //Copy low 32 bits to high 32 bits
     "  punpckldq   %%xmm4,   %%xmm4        \n\t" //Copy low 32 bits to high 32 bits
     "  pshuflw     $0x14,    %%xmm3,   %%xmm3  \n\t" //Shuffle Low 64 bits to get [Re Im Im Re]
     "  pshuflw     $0x14,    %%xmm4,   %%xmm4  \n\t" //Shuffle Low 64 bits to get [Re Im Im Re]
     "  pshufhw     $0x14,    %%xmm3,   %%xmm3  \n\t" //Shuffle High 64 bits to get [Re Im Im Re]
     "  pshufhw     $0x14,    %%xmm4,   %%xmm4  \n\t" //Shuffle High 64 bits to get [Re Im Im Re]
     "  pmullw      %%xmm7,   %%xmm3        \n\t" //Multiply to get [Re Im -Im Re]
     "  pmullw      %%xmm7,   %%xmm4        \n\t" //Multiply to get [Re Im -Im Re]
     "  pmaddwd     %%xmm3,   %%xmm0        \n\t" //Complex multiply and add
     "  pmaddwd     %%xmm4,   %%xmm1        \n\t" //Complex multiply and add
     "  paddd       %%xmm5,   %%xmm0        \n\t" //Add in 2^(shift-1)
     "  paddd       %%xmm5,   %%xmm1        \n\t" //Add in 2^(shift-1)
     "  psrad       %%xmm6,   %%xmm0        \n\t" //Shift by X bits
     "  psrad       %%xmm6,   %%xmm1        \n\t" //Shift by X bits
     "  packssdw    %%xmm0,   %%xmm0        \n\t" //Get into low 64 bits
     "  packssdw    %%xmm1,   %%xmm1        \n\t" //Get into low 64 bits
     "  movlpd      %%xmm0,   (%[A])        \n\t" //Move into A
     "  movlpd      %%xmm1,  8(%[A])        \n\t" //Move into A
     "  add            $16,    %[A]         \n\t" //Move in array
     "  add            $16,    %[B]         \n\t" //Move in array
     "SUB              $1,      %[CNT1]        \n\t"
     "JNZ L%=                                  \n\t" //Loop if not done
     "Z%=:                                  \n\t"
     "CMP              $0,   %[CNT2]           \n\t"
     "JE  ZZ%=                                 \n\t"
     "LL%=:                                 \n\t"
     "  movlpd        (%[A]),  %%xmm0       \n\t" //Copy from A
     "  movlpd        (%[B]),  %%xmm1       \n\t" //Copy from B
     "  punpckldq     %%xmm0,  %%xmm0            \n\t" //Copy low 32 bits to high 32 bits
     "  punpckldq     %%xmm1,  %%xmm1            \n\t" //Copy low 32 bits to high 32 bits
     "  pshuflw         $0x14, %%xmm1,  %%xmm1   \n\t" //Shuffle Low 64 bits to get [Re Im Im Re]
     "  pmullw        %%xmm7,  %%xmm1            \n\t" //Multiply to get [Re Im -Im Re]
     "  pmaddwd       %%xmm1,  %%xmm0            \n\t" //Complex multiply and add
     "  paddd         %%xmm5,  %%xmm0       \n\t" //Add in 2^(shift-1)
     "  psrad         %%xmm6,  %%xmm0       \n\t" //Shift by X bits
     "  packssdw      %%xmm0,  %%xmm0       \n\t" //Get into low 32 bits
     "  movd          %%xmm0,  (%[A])       \n\t" //Move into A
     "  add             $4,     %[A]        \n\t" //Move in array
     "  add             $4,     %[B]        \n\t" //Move in array
     "  SUB              $1,      %[CNT2]   \n\t"
     "  JNZ LL%=                            \n\t" //Loop if not done
     "ZZ%=:                                 \n\t"
     : [A] "+r" (_A), [B] "+r" (_B), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     : [SH] "m" (_shift),  [R] "m" (round), [MUL] "r" (mul)
     : "memory"
     );

}

void sse_cmulsc(CPX *_A, CPX *_B, CPX *_C, int32_t _cnt, int32_t _shift)
{

  int32_t cnt1;
  int32_t cnt2;
  int32_t round;

  volatile int32_t mul[4] = {0xffff0001, 0x00010001, 0xffff0001, 0x00010001}; //{1,-1,1,1,1,-1,1,1};

  cnt1 = _cnt/4;
  cnt2 = _cnt - 4*cnt1;

  round = 1 << (_shift-1);

  __asm__ __volatile__
    (
     "movupd     (%[MUL]),    %%xmm7       \n\t" //Move the multiply thingie
     "movss       %[SH],     %%xmm6       \n\t" //Move the shift thingie
     "movss       %[R],      %%xmm5       \n\t" //Move the round thingie
     "punpckldq   %%xmm5,    %%xmm5       \n\t"
     "punpcklqdq  %%xmm5,    %%xmm5       \n\t"
     "CMP             $0,         %[CNT1]      \n\t"
     "JE   Z%=                                 \n\t"
     "L%=:                                     \n\t"
     "  movlpd      (%[A]),   %%xmm0        \n\t" //Copy from A
     "  movlpd     8(%[A]),   %%xmm1        \n\t" //Copy from A
     "  movlpd      (%[B]),   %%xmm3        \n\t" //Copy from B
     "  movlpd     8(%[B]),   %%xmm4        \n\t" //Copy from B
     "  punpckldq   %%xmm0,   %%xmm0        \n\t" //Copy low 32 bits to high 32 bits
     "  punpckldq   %%xmm1,   %%xmm1        \n\t" //Copy low 32 bits to high 32 bits
     "  punpckldq   %%xmm3,   %%xmm3        \n\t" //Copy low 32 bits to high 32 bits
     "  punpckldq   %%xmm4,   %%xmm4        \n\t" //Copy low 32 bits to high 32 bits
     "  pshuflw     $0x14,    %%xmm3,   %%xmm3  \n\t" //Shuffle Low 64 bits to get [Re Im Im Re]
     "  pshuflw     $0x14,    %%xmm4,   %%xmm4  \n\t" //Shuffle Low 64 bits to get [Re Im Im Re]
     "  pshufhw     $0x14,    %%xmm3,   %%xmm3  \n\t" //Shuffle High 64 bits to get [Re Im Im Re]
     "  pshufhw     $0x14,    %%xmm4,   %%xmm4  \n\t" //Shuffle High 64 bits to get [Re Im Im Re]
     "  pmullw      %%xmm7,   %%xmm3        \n\t" //Multiply to get [Re Im -Im Re]
     "  pmullw      %%xmm7,   %%xmm4        \n\t" //Multiply to get [Re Im -Im Re]
     "  pmaddwd     %%xmm3,   %%xmm0        \n\t" //Complex multiply and add
     "  pmaddwd     %%xmm4,   %%xmm1        \n\t" //Complex multiply and add
     "  paddd       %%xmm5,   %%xmm0        \n\t" //Add in 2^(shift-1)
     "  paddd       %%xmm5,   %%xmm1        \n\t" //Add in 2^(shift-1)
     "  psrad       %%xmm6,   %%xmm0        \n\t" //Shift by X bits
     "  psrad       %%xmm6,   %%xmm1        \n\t" //Shift by X bits
     "  packssdw    %%xmm0,   %%xmm0        \n\t" //Get into low 64 bits
     "  packssdw    %%xmm1,   %%xmm1        \n\t" //Get into low 64 bits
     "  movlpd      %%xmm0,   (%[C])        \n\t" //Move into C
     "  movlpd      %%xmm1,  8(%[C])        \n\t" //Move into C
     "  add            $16,    %[A]         \n\t" //Move in array
     "  add            $16,    %[B]         \n\t" //Move in array
     "  add            $16,    %[C]         \n\t" //Move in array
     "SUB              $1,      %[CNT1]        \n\t"
     "JNZ L%=                                  \n\t" //Loop if not done
     "Z%=:                                  \n\t"
     "CMP              $0,   %[CNT2]           \n\t"
     "JE  ZZ%=                                 \n\t"
     "LL%=:                                 \n\t"
     "  movlpd        (%[A]),  %%xmm0       \n\t" //Copy from A
     "  movlpd        (%[B]),  %%xmm1       \n\t" //Copy from B
     "  punpckldq     %%xmm0,  %%xmm0       \n\t" //Copy low 32 bits to high 32 bits
     "  punpckldq     %%xmm1,  %%xmm1       \n\t" //Copy low 32 bits to high 32 bits
     "  pshuflw         $0x14, %%xmm1,  %%xmm1 \n\t" //Shuffle Low 64 bits to get [Re Im Im Re]
     "  pmullw        %%xmm7,  %%xmm1       \n\t" //Multiply to get [Re Im -Im Re]
     "  pmaddwd       %%xmm1,  %%xmm0       \n\t" //Complex multiply and add
     "  paddd         %%xmm5,  %%xmm0       \n\t" //Add in 2^(shift-1)
     "  psrad         %%xmm6,  %%xmm0       \n\t" //Shift by X bits
     "  packssdw      %%xmm0,  %%xmm0       \n\t" //Get into low 32 bits
     "  movd          %%xmm0,  (%[C])       \n\t" //Move into C
     "  add             $4,     %[A]        \n\t" //Move in array
     "  add             $4,     %[B]        \n\t" //Move in array
     "  add             $4,     %[C]        \n\t" //Move in array
     "  SUB              $1,      %[CNT2]   \n\t"
     "  JNZ LL%=                            \n\t" //Loop if not done
     "ZZ%=:                                 \n\t"
     : [A] "+r" (_A), [B] "+r" (_B), [C] "+r" (_C), [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     : [SH] "m" (_shift), [R] "m" (round), [MUL] "r" (mul)
     : "memory"
     );
}


void sse_cacc(CPX *_A, MIX *_B, int32_t _cnt, int32_t *_iaccum, int32_t *_baccum)
{

  int32_t cnt1;
  int32_t cnt2;

  if(((intptr_t)_A%16) || ((intptr_t)_B%16)) {

      cnt1 = _cnt / 6;
      cnt2 = (_cnt - (6*cnt1));

      __asm__ __volatile__
    (
     "pxor                %%xmm0,     %%xmm0         \n\t" //Clear the running sum
     "pxor                %%mm0,      %%mm0          \n\t" //Clear the running sum
     "CMP             $0,         %[CNT1]            \n\t"
     "JE   Z%=                                       \n\t"
     "L%=:                                           \n\t"
     "  movlpd            (%[A]),     %%xmm1         \n\t" //load IF data
     "  movlpd           8(%[A]),     %%xmm2         \n\t" //load IF data
     "  movlpd          16(%[A]),     %%xmm3         \n\t" //load IF data
     "  movupd            (%[B]),     %%xmm4         \n\t" //load IF data
     "  movupd          16(%[B]),     %%xmm5         \n\t" //load IF data
     "  movupd          32(%[B]),     %%xmm6         \n\t" //load IF data
     "  punpckldq         %%xmm1,     %%xmm1         \n\t" //copies bits 0..31 to 32..63 and bits 32..63 to 64..95 and 65..127
     "  punpckldq         %%xmm2,     %%xmm2         \n\t" //copies bits 0..63 to 64..127
     "  punpckldq         %%xmm3,     %%xmm3         \n\t" //copies bits 0..63 to 64..127
     "  pmaddwd           %%xmm4,     %%xmm1         \n\t" //multiply and add, result in xmm1
     "  pmaddwd           %%xmm5,     %%xmm2         \n\t" //multiply and add, result in xmm2
     "  pmaddwd           %%xmm6,     %%xmm3         \n\t" //multiply and add, result in xmm3
     "  paddd             %%xmm3,     %%xmm0         \n\t" //Add into accumulator (efficiently)
     "  paddd             %%xmm2,     %%xmm1         \n\t"
     "  paddd             %%xmm1,     %%xmm0         \n\t"
     "  add      $24,      %[A]                      \n\t" //move in complex sine by 24 bytes
     "  add      $48,      %[B]                      \n\t" //move in IF array by 48 bytes
     "SUB              $1,      %[CNT1]              \n\t"
     "JNZ L%=                                        \n\t" //Loop if not done
     "Z%=:                                           \n\t"
     "CMP              $0,   %[CNT2]                 \n\t"
     "JE  ZZ%=                                       \n\t"
     "LL%=:                                          \n\t"
     "  movd              (%[A]),     %%mm1          \n\t" //load IF data
     "  movq              (%[B]),     %%mm2          \n\t"
     "  punpckldq          %%mm1,     %%mm1          \n\t" //copy bottom 32 bits of IF data into high 32 bits
     "  pmaddwd            %%mm2,     %%mm1          \n\t" //perform mmx complex multiply
     "  paddd              %%mm1,     %%mm0          \n\t" //add into accumulator
     "  add         $4,     %[A]                     \n\t" //move in complex sine by 4 bytes
     "  add         $8,     %[B]                     \n\t" //move in complex sine by 4 bytes
     "  SUB              $1,      %[CNT2]            \n\t"
     "  JNZ LL%=                                     \n\t" //Loop if not done
     "ZZ%=:                                          \n\t"
     "movdq2q             %%xmm0,     %%mm1          \n\t"
     "punpckhqdq          %%xmm0,     %%xmm0         \n\t" //move bits 64..127 of xmm0 into 0..63 of xmm0
     "movdq2q             %%xmm0,     %%mm2          \n\t"
     "paddd                %%mm1,     %%mm0          \n\t" //add together
     "paddd                %%mm2,     %%mm0          \n\t" //add"    punpckldq xmm1, xmm1        \n\t" //copies bits 0..31 to 32..63 and bits 32..63 to 64..95 and 65..127 together
     "movd                 %%mm0,     (%[I])         \n\t"
     "punpckhdq            %%mm0,     %%mm0          \n\t"
     "movd                 %%mm0,     (%[Q])         \n\t"
     "EMMS                          \n\t"
     : [A] "+r"(_A), [B] "+r"(_B), [I] "+r"(_iaccum), [Q] "+r"(_baccum),
     [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     :
     :"memory"
     );
    }
  else
    {

      cnt1 = _cnt / 12;
      cnt2 = (_cnt - (12*cnt1));

      __asm__ __volatile__
    (
     "pxor                %%xmm0,     %%xmm0         \n\t" //Clear the running sum
     "pxor                %%mm0,      %%mm0          \n\t" //Clear the running sum
     "CMP             $0,         %[CNT1]            \n\t"
     "JE   AZ%=                                       \n\t"
     "AL%=:                                           \n\t"
     "  movlpd            (%[A]),     %%xmm1         \n\t" //load IF data
     "  movlpd           8(%[A]),     %%xmm2         \n\t" //load IF data
     "  movlpd          16(%[A]),     %%xmm3         \n\t" //load IF data
     "  movlpd          24(%[A]),     %%xmm4         \n\t" //load IF data
     "  movlpd          32(%[A]),     %%xmm5         \n\t" //load IF data
     "  movlpd          40(%[A]),     %%xmm6         \n\t" //load IF data
     "  punpckldq         %%xmm1,     %%xmm1         \n\t" //copies bits 0..31 to 32..63 and bits 32..63 to 64..95 and 65..127
     "  punpckldq         %%xmm2,     %%xmm2         \n\t" //copies bits 0..63 to 64..127
     "  punpckldq         %%xmm3,     %%xmm3         \n\t" //copies bits 0..63 to 64..127
     "  punpckldq         %%xmm4,     %%xmm4         \n\t" //copies bits 0..63 to 64..127
     "  punpckldq         %%xmm5,     %%xmm5         \n\t" //copies bits 0..63 to 64..127
     "  punpckldq         %%xmm6,     %%xmm6         \n\t" //copies bits 0..63 to 64..127
     "  pmaddwd           (%[B]),     %%xmm1         \n\t" //multiply and add, result in xmm1
     "  pmaddwd         16(%[B]),     %%xmm2         \n\t" //multiply and add, result in xmm2
     "  pmaddwd         32(%[B]),     %%xmm3         \n\t" //multiply and add, result in xmm3
     "  pmaddwd         48(%[B]),     %%xmm4         \n\t" //multiply and add, result in xmm4
     "  pmaddwd         64(%[B]),     %%xmm5         \n\t" //multiply and add, result in xmm5
     "  pmaddwd         80(%[B]),     %%xmm6         \n\t" //multiply and add, result in xmm6
     "  paddd             %%xmm2,     %%xmm1         \n\t" //Add into accumulator (efficiently)
     "  paddd             %%xmm4,     %%xmm3         \n\t"
     "  paddd             %%xmm6,     %%xmm5         \n\t"
     "  paddd             %%xmm3,     %%xmm1         \n\t"
     "  paddd             %%xmm5,     %%xmm0         \n\t"
     "  paddd             %%xmm1,     %%xmm0         \n\t"
     "  ADD        $48,     %[A]                     \n\t" //move in complex sine by 4 bytes
     "  ADD        $96,     %[B]                     \n\t" //move in complex sine by 4 bytes
     "SUB              $1,      %[CNT1]              \n\t"
     "JNZ AL%=                                       \n\t" //Loop if not done
     "AZ%=:                                          \n\t"
     "CMP              $0,      %[CNT2]              \n\t"
     "JE  AZZ%=                                      \n\t"
     "ALL%=:                                         \n\t"
     "  movq              (%[A]),     %%mm1          \n\t" //load IF data
     "  punpckldq          %%mm1,     %%mm1          \n\t" //copy bottom 32 bits of IF data into high 32 bits
     "  pmaddwd           (%[B]),     %%mm1          \n\t" //perform mmx complex multiply
     "  paddd              %%mm1,     %%mm0          \n\t" //add into accumulator
     "  add         $4,     %[A]                     \n\t" //move in complex sine by 4 bytes
     "  add         $8,     %[B]                     \n\t" //move in complex sine by 4 bytes
     "  SUB              $1,      %[CNT2]            \n\t"
     "  JNZ ALL%=                                    \n\t" //Loop if not done
     "AZZ%=:                                         \n\t"
     "movdq2q             %%xmm0,     %%mm1          \n\t"
     "punpckhqdq          %%xmm0,     %%xmm0         \n\t" //move bits 64..127 of xmm0 into 0..63 of xmm0
     "movdq2q             %%xmm0,     %%mm2          \n\t"
     "paddd                %%mm1,     %%mm0          \n\t" //add together
     "paddd                %%mm2,     %%mm0          \n\t" //add"    punpckldq xmm1, xmm1        \n\t" //copies bits 0..31 to 32..63 and bits 32..63 to 64..95 and 65..127 together
     "movd                 %%mm0,     (%[I])         \n\t"
     "punpckhdq            %%mm0,     %%mm0          \n\t"
     "movd                 %%mm0,     (%[Q])         \n\t"
     "EMMS                                           \n\t"
     : [A] "+r"(_A), [B] "+r"(_B), [I] "+r"(_iaccum), [Q] "+r"(_baccum),
     [CNT1] "+r" (cnt1), [CNT2] "+r" (cnt2)
     ::"memory"
     );

    }//end if

}


//!< A must hold baseband data, E,P,L must hold PRN data
void sse_prn_accum_new(CPX *_A, MIX *_E, MIX *_P, MIX *_L, int32_t _cnt, CPX_ACCUM *_accum)
{

  __asm__ __volatile__
    (
     "pxor             %%mm5,       %%mm5           \n\t" //Clear the running sum for E
     "pxor             %%mm6,       %%mm6           \n\t" //Clear the running sum for P
     "pxor             %%mm7,       %%mm7           \n\t" //Clear the running sum for L
     "CMP               $0,          %[CNT]         \n\t"
     "JE   Z%=                                      \n\t"
     "L%=:                                          \n\t"
     "  movd          (%[A]),      %%mm0           \n\t" //load IF data
     "  movq          (%[E]),      %%mm1           \n\t" //load E data
     "  movq          (%[P]),      %%mm2           \n\t" //load P data
     "  movq          (%[L]),      %%mm3           \n\t" //load L data
     "  punpckldq      %%mm0,       %%mm0           \n\t" //copy low 32 bits to high 32 pits
     "  pmaddwd        %%mm0,       %%mm1           \n\t" //complex multiply E by IF
     "  pmaddwd        %%mm0,       %%mm2           \n\t" //complex multiply P by IF
     "  pmaddwd        %%mm0,       %%mm3           \n\t" //complex multiply L by IF
     "  paddd          %%mm1,       %%mm5           \n\t" //add into E accumulator
     "  paddd          %%mm2,       %%mm6           \n\t" //add into E accumulator
     "  paddd          %%mm3,       %%mm7           \n\t" //add into E accumulator
     "  add   $4,       %[A]                        \n\t" //move in baseband data by one sample (4 bytes)
     "  add   $8,       %[E]                        \n\t" //move in PRN-E array by one sample (8 bytes)
     "  add   $8,       %[P]                        \n\t" //move in PRN-P array by one sample (8 bytes)
     "  add   $8,       %[L]                        \n\t" //move in PRN-L array by one sample (8 bytes)
     "SUB     $1,       %[CNT]                      \n\t"
     "JNZ L%=                                       \n\t" //Loop if not done
     "Z%=:                                          \n\t"
     "movq             %%mm5,     (%[ACC])          \n\t"
     "add     $8,      %[ACC]                       \n\t"
     "movq             %%mm6,     (%[ACC])          \n\t"
     "add     $8,      %[ACC]                       \n\t"
     "movq             %%mm7,     (%[ACC])          \n\t"
     "EMMS                                          \n\t"
     : [A] "+g" (_A), [E] "+g" (_E), [P] "+g" (_P), [L]"+g" (_L),
     [CNT] "+r" (_cnt), [ACC] "+r" (_accum)
     :
     : "memory"
     );//end __asm

}
