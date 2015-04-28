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


	The MIT License:
	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
	OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


	 /\_/\    <~~~~~~~~~~~~~~~>
	( o.o ) <      ENJOY!     |
	 > ^ <    ^~~~~~~~~~~~~~~~^

********************************************************************************************/
#include "bina_types.h"
#ifndef _FFT_BIT_REVERSER_H_
#define _FFT_BIT_REVERSER_H_

typedef size_t (*FFT_BITREVERSER)(size_t, uint8_t);
// Bit-reversal routine for 8-bit
static inline size_t _bitreverse8(size_t , uint8_t);
// Bit-reversal routine for 16-bit
static inline size_t _bitreverse16(size_t , uint8_t);
// Bit-reversal routine for 32-bit
static inline size_t _bitreverse32(size_t , uint8_t);

#ifdef _FFT_64_BIT_
// Bit-reversal routine for 64-bit
static inline size_t _bitreverse64(size_t , uint8_t);
#endif /* _FFT_64_BIT_ */
// Selection multiplexer
FFT_BITREVERSER fft_bitreverse_select(uint8_t);

// Swap two complex values
static inline void _fft_swap_complex_i(bina_complex, bina_complex);

// Bit-reverse permute of an array of complex numbers
void _fft_bitreverse_swap_complex_i(bina_complex*,uint8_t);

#endif /* _FFT_BIT_REVERSER_H_ */