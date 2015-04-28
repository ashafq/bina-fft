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
#include "bit_reverser.h"
#ifdef _FFT_BIT_REVERSER_H_
// Subroutine for bit-reversal
static inline size_t _bitreverse8(size_t v, uint8_t log2N)
{
	v = (uint8_t)v;
	v = ((v >> 1) & 0x55) | ((v & 0x55) << 1);
	v = ((v >> 2) & 0x33) | ((v & 0x33) << 2);
	v = (uint8_t)((v >> 4) | (v << 4));
	v >>= (8 - log2N);
	return v;
}
static inline size_t _bitreverse16(size_t v, uint8_t log2N)
{
	v = (uint16_t)v;
	v = ((v >> 1) & 0x5555) | ((v & 0x5555) << 1);
	v = ((v >> 2) & 0x3333) | ((v & 0x3333) << 2);
	v = ((v >> 4) & 0x0F0F) | ((v & 0x0F0F) << 4);
	v = (uint16_t)((v >> 8) | (v << 8));
	v >>= (16 - log2N);
	return v;
} 
static inline size_t _bitreverse32(size_t v, uint8_t log2N)
{
	v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
	v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
	v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
	v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
	return (uint32_t)(((v >> 16) | (v << 16)) >> (32 - log2N));
}
#if defined (_FFT_64_BIT_)
static inline size_t _bitreverse64(size_t v, uint8_t log2N)
{
	v = ((v >> 1) & 0x5555555555555555ULL) | ((v & 0x5555555555555555ULL) << 1);
	v = ((v >> 2) & 0x3333333333333333ULL) | ((v & 0x3333333333333333ULL) << 2);
	v = ((v >> 4) & 0x0F0F0F0F0F0F0F0FULL) | ((v & 0x0F0F0F0F0F0F0F0FULL) << 4);
	v = ((v >> 8) & 0x00FF00FF00FF00FFULL) | ((v & 0x00FF00FF00FF00FFULL) << 8);
	v = ((v >>16) & 0x0000FFFF0000FFFFULL) | ((v & 0x0000FFFF0000FFFFULL) <<16);
	return (((v >> 32) | (v << 32)) >> (64 - log2N));
}
#endif
// Selection multiplexer
FFT_BITREVERSER fft_bitreverse_select(uint8_t log2N)
{
	static FFT_BITREVERSER reverser;
	if(log2N <= 8) reverser = _bitreverse8;
	else if(log2N >  8 && log2N <= 16) reverser = _bitreverse16;
	#if defined (_FFT_64_BIT_)
	else if(log2N > 16 && log2N <= 32) reverser = _bitreverse32;
	else reverser = _bitreverse64;
	#else
	else reverser = _bitreverse32;
	#endif
	return reverser;
}

static inline void _fft_swap_complex_i(bina_complex toswap_0, bina_complex toswap_1)
{
	#if defined (_FFT_64_BIT_)
	
	// Since bina_complex is 64 bit, we can use a 64 bit register to swap
	// Instead of two operations
	// NOTE: This will only work on 64-bit mode (sorry 32-bit machines :( )
	uint64_t temp;
	temp = *(uint64_t*)(toswap_0);
	*(uint64_t*)(toswap_0) = *(uint64_t*)(toswap_1);
	*(uint64_t*)(toswap_1) = temp;

	#else
	
	// Use Portable code
	bina_complex temp;
	temp[0] = toswap_0[0]; 
	temp[1] = toswap_0[1];
	
	toswap_0[0] = toswap_1[0]; 
	toswap_0[1] = toswap_1[1];
	
	toswap_1[0] = temp[0]; 
	toswap_1[1] = temp[1];
		
	#endif
}
void _fft_bitreverse_swap_complex_i(
				bina_complex* x,
				uint8_t log2N)
{
	size_t N = (0x01 << log2N);
	size_t half_N = N >> 0x1;
	const size_t mask = (N - 1);
	size_t n, n_rev, n_not, n_not_rev;

	// Selecting from the delegate
	FFT_BITREVERSER bit_reverse = fft_bitreverse_select(log2N);
	
	for(n = 0x0000; n < half_N; n++){
		
		n_rev = bit_reverse(n, log2N);
		
		if(n < n_rev){
			
			_fft_swap_complex_i(x[n],x[n_rev]);
			
			n_not = mask ^ n;
			n_not_rev = mask ^ n_rev;
			
			_fft_swap_complex_i(x[n_not], x[n_not_rev]);
		}
		
		n++; // Need an extra increment here
		n_rev |= half_N;
		
		_fft_swap_complex_i(x[n], x[n_rev]);		
	}	
}

#endif /*_FFT_BIT_REVERSER_H_*/