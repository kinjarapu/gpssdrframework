/*! \file CPUID.cpp
  Functions which probe the CPU to discover what SIMD functionality is present
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

#include "includes.h"

bool CPU_MMX()
{
  int ret;
  asm (
    "mov $1, %%eax       \t\n" // a into eax
    "cpuid               \t\n"
    "sar $0x17, %%edx    \t\n"
    "and $0x1,  %%edx    \t\n"
    "mov %%edx, %[R]     \t\n" // eax into b
    :[R] "=r"(ret)
    ::"%eax", "%edx" /* clobbered register */
  );
  return ret;
}


bool CPU_SSE()
{
  int ret;
  asm (
    "mov $1, %%eax       \t\n" // a into eax
    "cpuid               \t\n"
    "sar $0x19, %%edx    \t\n"
    "and $0x1,  %%edx    \t\n"
    "mov %%edx, %[R]     \t\n" // eax into b
    :[R] "=r"(ret)
    ::"%eax", "%edx" /* clobbered register */
  );
  return ret;
}


bool CPU_SSE2()
{
  int ret;
  asm (
    "mov $1, %%eax       \t\n" // a into eax
    "cpuid               \t\n"
    "sar $0x1A, %%edx    \t\n"
    "and $0x1,  %%edx    \t\n"
    "mov %%edx, %[R]     \t\n" // eax into b
    :[R] "=r"(ret)
    ::"%eax", "%edx" /* clobbered register */
  );
  return ret;
}


bool CPU_SSE3()
{
  int ret;
  asm (
    "mov $1, %%eax       \t\n" // a into eax
    "cpuid               \t\n"
    "and $0x1,  %%ecx    \t\n"
    "mov %%ecx, %[R]     \t\n" // eax into b
    :[R] "=r"(ret)
    ::"%eax", "%ecx" /* clobbered register */
  );
  return ret;
}

bool CPU_SSSE3()
{
  int ret;
  asm (
    "mov $1, %%eax       \t\n" // a into eax
    "cpuid               \t\n"
    "sar $0x09, %%ecx    \t\n"
    "and $0x1,  %%ecx    \t\n"
    "mov %%ecx, %[R]     \t\n" // eax into b
    :[R] "=r"(ret)
    ::"%eax", "%ecx" /* clobbered register */
  );
  return ret;
}

bool CPU_SSE41()
{
  int ret;
  asm (
    "mov $1, %%eax       \t\n" // a into eax
    "cpuid               \t\n"
    "sar $0x13, %%ecx    \t\n"
    "and $0x1,  %%ecx    \t\n"
    "mov %%ecx, %[R]     \t\n" // eax into b
    :[R] "=r"(ret)
    ::"%eax", "%ecx" /* clobbered register */
  );
  return ret;
}


bool CPU_SSE42()
{
  int ret;
  asm (
    "mov $1, %%eax       \t\n" // a into eax
    "cpuid               \t\n"
    "sar $0x14, %%ecx    \t\n"
    "and $0x1,  %%ecx    \t\n"
    "mov %%ecx, %[R]     \t\n" // eax into b
    :[R] "=r"(ret)
    ::"%eax", "%ecx" /* clobbered register */
  );
  return ret;
}


void Init_SIMD()
{

//  if(CPU_SSE3())
//  {
//    simd_add = &sse_add;
//    simd_sub = &sse_sub;
//    simd_mul = &sse_mul;
//    simd_dot = &sse_dot;
//
//    simd_conj = &sse_conj;
//    simd_cacc = &sse_cacc;
//    simd_cmul = &sse_cmul;
//    simd_cmuls = &sse_cmuls;
//    simd_cmulsc = &sse_cmulsc;
//  }
//  else
//  {
//    simd_add = &x86_add;
//    simd_sub = &x86_sub;
//    simd_mul = &x86_mul;
//    simd_dot = &x86_dot;
//
//    simd_conj = &x86_conj;
//    simd_cacc = &x86_cacc;
//    simd_cmul = &x86_cmul;
//    simd_cmuls = &x86_cmuls;
//    simd_cmulsc = &x86_cmulsc;
//
//  }
//
//  simd_cmag = &x86_cmag;
//  simd_max = &x86_max;


}

