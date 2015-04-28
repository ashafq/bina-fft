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
#include <math.h>
#include <string.h>
#include "bina_malloc.h"
#include "bit_reverser.h"


#ifndef _BINA_FFT_H_
#define _BINA_FFT_H_

/*===============================================================================================*/
  #define bina_fft(x)     	bina_radix2_fft (log2i(sizeof(x)/sizeof(bina_complex)), NULL, x)
  #define bina_ifft(x)     	bina_radix2_ifft(log2i(sizeof(x)/sizeof(bina_complex)), NULL, x)
/*===============================================================================================
// Makes it really friendly to use! Just create an array of bina_complex and pass it through
// Usage example:
// 		int N = 32;
// 		bina_complex fftBuffer[N]; 
//      //or bina_complex *input = (bina_complex*)malloc(N*sizeof(bina_complex));
//      //.... fill it up with some data .....
//      bina_fft(fftBuffer); // The result is stored in input (in place)
//      // To inverse: 
//		bina_ifft(fftBuffer); // Simple!
// Note: This is a radix-2 fft algorithm. The size of array should be a power of two to be
// properly utilized. If the array is not a power of two, it will round down to power of two.
// Suppose SIZE = 523 it will compute the FFT of size 512.
//===============================================================================================*/

/*===============================================================================================*/
  #define bina_fft_radix2(x, log2N)     bina_radix2_fft(log2N, NULL, x)
  #define bina_ifft_radix2(x, log2N)    bina_radix2_ifft(log2N, NULL, x)
/*===============================================================================================
// If you really want to specify the radix, then this routine can be utilized. It wont be the
// most optimal, since you do not have twiddle factor lookup table. For an optimal routine,
// use bina_fft_radix2_twiddle() 
//===============================================================================================*/


/*===============================================================================================*/
  #define bina_fft_radix2_twiddle(x, twiddle, log2N)    bina_radix2_fft(log2N, twiddle, x)
  #define bina_ifft_radix2_twiddle(x, twiddle, log2N)   bina_radix2_ifft(log2N, twiddle, x)
/*===============================================================================================
// This routine will make this FFT shine! :3
// Allocate twiddle factor lookup table by calling: bina_create_twiddle_lookup(log2N)
// Then, ... watch it fly?
// This routine can be useful when you are computing the same length FFT over and over (like for
// Short Time Fourier Transform).
// Usage example:
// 		int N = 1024, log2N = log2i(N);
// 		bina_complex fftBuffer = malloc(sizeof(bina_complex)*N);
// 		bina_complex twiddleFactors = bina_create_twiddle_lookup(log2N);
// 		// ... fill up buffer with samples ...
//      bina_fft_radix2_twiddle(x, twiddleFactors, log2N);
//      // ... Do something with the fft data ...
//      free(fftBuffer); free(twiddleFactors);
//===============================================================================================*/

uint8_t log2i(uint32_t N);
// Calculates the log base 2 of N for integer values, result is usually floored down
bina_complex* bina_create_twiddle_lookup(uint8_t log2N);
const char* bina_fft_logo(void);
void fft_calculate_twiddle(
	uint8_t log2N,       // log2(fft_size) (prevents radix check)
	bina_complex* twiddle);  // Real and imaginary twiddle factor coefs (CONSTANTS)
int bina_radix2_fft(
	uint8_t log2N,       // log2(fft_size) (prevents radix check)
	bina_complex* twiddle,  // Real and imaginary twiddle factor coefs (CONSTANTS)
	bina_complex* y);       // Real and imaginary coefficients	
int bina_radix2_ifft(
	uint8_t log2N,       // log2(fft_size) (prevents radix check)
	bina_complex* twiddle,  // Real and imaginary twiddle factor coefs (CONSTANTS)
	bina_complex* y);       // Real and imaginary coefficients
#endif /* _BINA_FFT_H_ */