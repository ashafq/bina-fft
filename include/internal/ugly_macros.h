/********************************************************************************************

	oooooooooo.   o8o                        oooooooooooo oooooooooooo  ooooooooooooo 
	`888'   `Y8b  `"'                        `888'     `8 `888'     `8  8'   888   `8 
	 888     888 oooo  ooo. .oo.    .oooo.    888          888               888      
	 888oooo888' `888  `888P"Y88b  `P  )88b   888oooo8     888oooo8          888      
	 888    `88b  888   888   888   .oP"888   888    "     888    "          888      
	 888    .88P  888   888   888  d8(  888   888          888               888      
	o888bood8P'  o888o o888o o888o `Y888""8o o888o        o888o             o888o
                                                                   
	Bina FFT: An AVX Intrinsics Based Radix-2 FFT Routine
	Copyright (C) 2012 - Ayan Shafqat, All rights reserved.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	 /\_/\    <~~~~~~~~~~~~~~~>
	( o.o ) <      ENJOY!     |
	 > ^ <    ^~~~~~~~~~~~~~~~^

********************************************************************************************/
#ifndef _FFT_DATA_TYPES_H_
#define _FFT_DATA_TYPES_H_
/* In order to use uint8_t uint16_t and other types*/
#include <stdint.h>
#include <stdlib.h>

//#define _FFT_DISABLE_VECTOR_ // To disable SIMD vector support, uncomment this line
#ifndef _FFT_DISABLE_VECTOR_
#if defined (__SSE4__) || (__SSE3__) || (__SSE2__) || (__SSE__) || (__AVX__)
#include <immintrin.h>
#define _FFT_VECTOR_SUPPORT_ENABLED_ 0x1
#else
#warning "No vector support enabled\n"
#endif /* _FFT_VECTOR_SUPPORT_ENABLED_ */
#endif /* _FFT_DISABLE_VECTOR_ */

// Determine if CPU has a 64-bit accumulator
#if UINTPTR_MAX == 0xffffffffffffffff || defined (__X86_64__)
#define _FFT_64_BIT_
#endif

/* Multi-Compiler Alignment */
#ifdef __GNUC__
 #define _mem_align(n) __attribute__ ((aligned(n)))
#elif defined(_MSC_VER) || defined(__POCC__)
 #define _mem_align(n) __declspec(align(n))
#else
 #define _mem_align(n)
#endif

#if defined (__AVX__)
typedef union {
	__m128 vector;
	_mem_align(16) float element[4];
} vector_4_ps;

typedef union {
	__m256 vector;
	_mem_align(32) float element[8];
} vector_8_ps;
#endif
// Complex interleaved data type
typedef float _mem_align(8) bina_complex[2];
#define c_real( x ) x[0]
#define c_imag( x ) x[1]


#endif /* _FFT_DATA_TYPES_H_ */

