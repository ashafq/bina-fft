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
#include "BinaFFT.h"
#ifdef _BINA_FFT_H_

#if defined (__AVX__)
#include "bina_vector_kernels.h"
#endif

uint8_t log2i(uint32_t N)
{
	if (!N)
		return 0;
#if defined (__GNUC__)

	uint32_t v = __builtin_clz(N);

#else				// User portable code

	uint32_t v = -1;
	while (N) {
		v++;
		N >>= 1;
	}

#endif				/* FFT__USE_COMPILER_CODE */
	return v;
}

const char *bina_fft_logo(void)
{
	static const char data[] =
	    "\t ______     _                   ________  ________  _________\n"
	    "\t|_   _ \\   (_)                 |_   __  ||_   __  ||  _   _  |\n"
	    "\t  | |_) |  __   _ .--.   ,--.    | |_ \\_|  | |_ \\_||_/ | | \\_|\n"
	    "\t  |  __'. [  | [ `.-. | `'_\\ :   |  _|     |  _|       | |\n"
	    "\t _| |__) ) | |  | | | | // | |, _| |_     _| |_       _| |_\n"
	    "\t|_______/ [___][___||__]\'-;__/ |_____|   |_____|     |_____|\n"
	    "\t\n"
	    "\t   Bina FFT: An AVX Intrinsics Based Radix-2 FFT Routine\n"
	    "\t  Copyright (C) 2012 - Ayan Shafqat, All rights reserved.\n";
	return data;
}

bina_complex *bina_create_twiddle_lookup(uint8_t log2N)
{
#if defined (__AVX__)
	size_t memory_alignment = 32;
#else
	size_t memory_alignment = 16
#endif
	    size_t fft_size = 0x1 << log2N;
	static bina_complex *twiddle;
	if ((twiddle =
	     (bina_complex *) bina_aligned_malloc((fft_size >> 1) *
						  sizeof(bina_complex),
						  memory_alignment)) == NULL) {
		return NULL;	// Error allocating memory for twiddle factor
	}
	fft_calculate_twiddle(log2N, twiddle);
	return twiddle;
}

void fft_calculate_twiddle(uint8_t log2N, bina_complex * twiddle)
{
	size_t fft_size = 0x01 << log2N;
	size_t twd_size = (fft_size >> 1);
	const long double TWO_PI =
	    2.0 *
	    3.14159265358979323851280895940618620443274267017841339111328125l;
	long double kth, temp;
	size_t n;
	for (n = 0; n < twd_size; n++) {
		kth = TWO_PI * n / (double)fft_size;
		// Preventing precission error for calculating twiddle factors
		temp = +cos(kth);
		twiddle[n][0] = temp;
		temp = -sin(kth);
		twiddle[n][1] = temp;
	}
}

#if defined (__AVX__)
int bina_radix2_fft(uint8_t log2N,	// log2(fft_size) (prevents radix check)
		    bina_complex * twiddle,	// Real and imaginary twiddle factor coefs (CONSTANTS)
		    bina_complex * y)	// Real and imaginary coefficients      
{
	// Some local variables
	uint8_t twiddle_flag = 0;
	size_t fft_size = 0x1 << log2N;

	if (fft_size == 1) {
		// FFT of a scaler is the scaler itself
		return 0;	// Done with FFT
	}

	if (fft_size == 2) {
		// A two point fft is simply a butterfly. Therefore, we do not
		// need to do the actual routine. Instead, we can use a kernel
		// function ( __m128 _mm_fft_butterfly(__m128) )
		*(__m128 *) y = _mm_fft_butterfly(*(__m128 *) y);
		return 0;	// No errors: Completed successfully
	}
	if (fft_size == 4) {
		// In order to optimize it for AVX, we also need to consider
		// case of when fft_size is 4. This kernel uses a __m256 data
		// and does the fft operation with 1 load and 1 store.
		*(__m256 *) y = _mm256_fft_kernel_4(*(__m256 *) y);
		return 0;	// No errors: Completed successfully
	}
	// In order to get the most from the fft routine, twiddle factor can be calculated
	// into a lookup table by calling the function bina_create_twiddle_lookup.
	// If not calculated, this portion allocates and computes twiddle factors.
	if (twiddle == NULL) {
		if ((twiddle = bina_create_twiddle_lookup(log2N)) == NULL) {
			return -1;	// Error: allocating memory for twiddle factor
		}
		twiddle_flag = 1;
	}

	if (fft_size == 8) {
		// Since we will be doing 8 butterflies by using AVX, there 
		// will be some alignment problems if we have to do an 8 point fft
		// Therefore, a custom kernel is used here
		_mm256_fft_kernel_8(y, twiddle);
		if (twiddle_flag)
			free(twiddle);
		return 0;
	}
	// Here we use the regular FFT routine, but with some AVX pizzaz.
	size_t Nblock, blocks, Nb, even, odd, k;

	// Bit-reversing the sequence
	_fft_bitreverse_swap_complex_i(y, log2N);

	// Doing the first pass with AVX
	for (k = 0; k < (fft_size); k += 4) {
		*(__m256 *) y[k] = _mm256_fft_two_butterflies(*(__m256 *) y[k]);
	}

	// Doing the 2nd pass of the FFT using SSE4/AVX code
	blocks = fft_size >> 2;
	Nblock = 4;		// Number of blocks
	Nb = 2;			// Number of butterflies
	for (even = 0; even < fft_size; even += Nblock) {
		odd = even + Nb;	// Calculate the odd index
		for (k = 0; k < Nb; k += 2) {
			// This portion use sse4 code  
			__m128 twd =
			    _mm_load_complex_ps(twiddle[k * blocks],
						twiddle[(k + 1) * blocks]);
			__m128 *top = (__m128 *) y[even + k];
			__m128 *bot = (__m128 *) y[odd + k];
			_mm_fft_butterfly_times_twiddle(top, bot, twd);

		}
	}			// End of block loop

	// Doing the rest of the stages using AVX :) yeay!
	Nblock <<= 1;		// Number of blocks
	Nb <<= 1;		// Number of butterflies
	// Start calculating FFT
	for (blocks = fft_size >> 3; blocks > 0; blocks >>= 1) {
		for (even = 0; even < fft_size; even += Nblock) {
			odd = even + Nb;	// Calculate the odd index
			for (k = 0; k < Nb; k += 4) {
				// This portion use AVX to calculate 4 butterflies at a time
				__m256 twd =
				    _mm256_load_complex_ps(twiddle
							   [(k + 0) * blocks],
							   twiddle[(k +
								    1) *
								   blocks],
							   twiddle[(k +
								    2) *
								   blocks],
							   twiddle[(k +
								    3) *
								   blocks]);
				__m256 *top = (__m256 *) y[even + k];
				__m256 *bot = (__m256 *) y[odd + k];
				_mm256_fft_butterly_times_twiddle(top, bot,
								  twd);

			}
		}		// End of block loop
		Nblock <<= 1;	// Next stage will have twice the number of blocks
		Nb <<= 1;	// Next stage will also have twice the number of butterflies
	}			// End of pass loop

	// Freeing up twiddle factor
	if (twiddle_flag)
		bina_aligned_free(twiddle);
	return 0;		// No errors
}

// 
//  BinaFFT.c
//  Inverse FFT Routine
//  
//  Created by Ayan Shafqat on 2012-03-07.
// 
int bina_radix2_ifft(uint8_t log2N,	// log2(fft_size) (prevents radix check)
		     bina_complex * twiddle,	// Real and imaginary twiddle factor coefs (CONSTANTS)
		     bina_complex * y)	// Real and imaginary coefficients      
{
	// Some local variables
	uint8_t twiddle_flag = 0;
	size_t fft_size = 0x1 << log2N;

	if (fft_size == 1) {
		// iFFT of a scaler is the scaler itself
		return 0;	// Done with FFT
	}

	if (fft_size == 2) {
		// A two point fft is simply a butterfly. Therefore, we do not
		// need to do the actual routine. Instead, we can use a kernel
		// function ( __m128 _mm_fft_butterfly(__m128) )
		// NOTE: IFFT(X)=1Nconj(FFT(conj(X)))
		__m128 scale = _mm_set1_ps(0.5f);
		__m128 conjugator = _mm_set_ps(-0.f, 0.f, -0.f, 0.f);
		__m128 vector = _mm_xor_ps(conjugator, *(__m128 *) y);
		vector = _mm_fft_butterfly(vector);
		vector = _mm_xor_ps(conjugator, vector);
		*(__m128 *) y = _mm_mul_ps(vector, scale);

		return 0;	// No errors: Completed successfully
	}
	if (fft_size == 4) {
		// In order to optimize it for AVX, we also need to consider
		// case of when fft_size is 4. This kernel uses a __m256 data
		// and does the fft operation with 1 load and 1 store.
		__m256 scale = _mm256_set1_ps(0.25f);
		__m256 conjugator =
		    _mm256_set_ps(-0.f, 0.f, -0.f, 0.f, -0.f, 0.f, -0.f, 0.f);
		__m256 vector = _mm256_xor_ps(conjugator, *(__m256 *) y);
		vector = _mm256_fft_kernel_4(vector);
		vector = _mm256_xor_ps(conjugator, vector);
		*(__m256 *) y = _mm256_mul_ps(vector, scale);
		return 0;	// No errors: Completed successfully
	}
	// In order to get the most from the fft routine, twiddle factor can be calculated
	// into a lookup table by calling the function bina_create_twiddle_lookup.
	// If not calculated, this portion allocates and computes twiddle factors.
	if (twiddle == NULL) {
		if ((twiddle = bina_create_twiddle_lookup(log2N)) == NULL) {
			return -1;	// Error: allocating memory for twiddle factor
		}
		twiddle_flag = 1;
	}

	if (fft_size == 8) {
		// Since we will be doing 8 butterflies by using AVX, there 
		// will be some alignment problems if we have to do an 8 point fft
		// Therefore, a custom kernel is used here

		__m256 scale = _mm256_set1_ps(0.125f);
		__m256 conjugator =
		    _mm256_set_ps(-0.f, 0.f, -0.f, 0.f, -0.f, 0.f, -0.f, 0.f);

		*(__m256 *) y[0] = _mm256_xor_ps(conjugator, *(__m256 *) y[0]);
		*(__m256 *) y[4] = _mm256_xor_ps(conjugator, *(__m256 *) y[4]);

		_mm256_fft_kernel_8(y, twiddle);

		*(__m256 *) y[0] =
		    _mm256_mul_ps(_mm256_xor_ps(conjugator, *(__m256 *) y[0]),
				  scale);
		*(__m256 *) y[4] =
		    _mm256_mul_ps(_mm256_xor_ps(conjugator, *(__m256 *) y[4]),
				  scale);

		if (twiddle_flag)
			free(twiddle);
		return 0;
	}
	// Here we use the regular FFT routine, but with some AVX pizzaz.
	size_t Nblock, blocks, Nb, even, odd, k;

	__m128 scale128 = _mm_set1_ps(0.5f);
	__m128 conju128 = _mm_set_ps(-0.f, 0.f, -0.f, 0.f);
	__m256 scale256 = _mm256_set1_ps(0.5f);
	__m256 conju256 =
	    _mm256_set_ps(-0.f, 0.f, -0.f, 0.f, -0.f, 0.f, -0.f, 0.f);

	// Bit-reversing the sequence
	_fft_bitreverse_swap_complex_i(y, log2N);

	// Doing the first pass with AVX
	for (k = 0; k < (fft_size); k += 4) {
		*(__m256 *) y[k] =
		    _mm256_mul_ps(_mm256_fft_two_butterflies(*(__m256 *) y[k]),
				  scale256);
	}

	// Doing the 2nd pass of the FFT using SSE4/AVX code
	blocks = fft_size >> 2;
	Nblock = 4;		// Number of blocks
	Nb = 2;			// Number of butterflies
	for (even = 0; even < fft_size; even += Nblock) {
		odd = even + Nb;	// Calculate the odd index
		for (k = 0; k < Nb; k += 2) {
			// This portion use sse4 code  
			__m128 twd =
			    _mm_load_complex_ps(twiddle[k * blocks],
						twiddle[(k + 1) * blocks]);
			twd = _mm_xor_ps(conju128, twd);
			__m128 *top = (__m128 *) y[even + k];
			__m128 *bot = (__m128 *) y[odd + k];
			_mm_fft_butterfly_times_twiddle(top, bot, twd);
			*top = _mm_mul_ps(scale128, *top);
			*bot = _mm_mul_ps(scale128, *bot);

		}
	}			// End of block loop

	// Doing the rest of the stages using AVX :) yeay!
	Nblock <<= 1;		// Number of blocks
	Nb <<= 1;		// Number of butterflies
	// Start calculating FFT
	for (blocks = fft_size >> 3; blocks > 0; blocks >>= 1) {
		for (even = 0; even < fft_size; even += Nblock) {
			odd = even + Nb;	// Calculate the odd index
			for (k = 0; k < Nb; k += 4) {
				// This portion use AVX to calculate 4 butterflies at a time
				__m256 twd =
				    _mm256_load_complex_ps(twiddle
							   [(k + 0) * blocks],
							   twiddle[(k +
								    1) *
								   blocks],
							   twiddle[(k +
								    2) *
								   blocks],
							   twiddle[(k +
								    3) *
								   blocks]);
				twd = _mm256_xor_ps(conju256, twd);
				__m256 *top = (__m256 *) y[even + k];
				__m256 *bot = (__m256 *) y[odd + k];
				_mm256_fft_butterly_times_twiddle(top, bot,
								  twd);

				*top = _mm256_mul_ps(scale256, *top);
				*bot = _mm256_mul_ps(scale256, *bot);
			}
		}		// End of block loop
		Nblock <<= 1;	// Next stage will have twice the number of blocks
		Nb <<= 1;	// Next stage will also have twice the number of butterflies
	}			// End of pass loop

	// Freeing up twiddle factor
	if (twiddle_flag)
		bina_aligned_free(twiddle);
	return 0;		// No errors
}
#else				// Use portable code
// 
//  BinaFFT.c
//  binafft_v1
//  Portable code for other architectures
//  Created by Ayan Shafqat on 2012-03-07.
// 
int bina_radix2_fft(uint8_t log2N,	// log2(fft_size) (prevents radix check)
		    bina_complex * twiddle,	// Real and imaginary twiddle factor coefs (CONSTANTS)
		    bina_complex * y)	// Real and imaginary coefficients      
{
	// Some local variables
	size_t fft_size = 0x1 << log2N;
	uint8_t twiddle_flag = 0;
	// In order to get the most from the fft routine, twiddle factor can be calculated
	// into a lookup table by calling the function bina_create_twiddle_lookup.
	// If not calculated, this portion allocates and computes twiddle factors.
	if (twiddle == NULL) {
		if ((twiddle = bina_create_twiddle_lookup(log2N)) == NULL) {
			return -1;	// Error: allocating memory for twiddle factor
		}
		twiddle_flag = 1;
	}
	if (fft_size == 1)
		return 0;	// DFT of a constant is the constant itself

	size_t Nblock, blocks, Nb, even, odd;
	size_t k, W, B, T;

	bina_complex temp;

	// Bit-reversing the sequence
	_fft_bitreverse_swap_complex_i(y, log2N);

	Nblock = 2;		// Number of blocks
	Nb = 1;			// Number of butterflies
	// Start calculating FFT
	for (blocks = fft_size >> 1; blocks > 0; blocks >>= 1) {
		for (even = 0; even < fft_size; even += Nblock) {
			odd = even + Nb;	// Calculate the odd index
			for (k = 0; k < Nb; k++) {
				W = k * blocks;
				T = even + k;
				B = odd + k;
				if (W) {
					temp[0] =
					    y[B][0] * twiddle[W][0] -
					    y[B][1] * twiddle[W][1];
					temp[1] =
					    y[B][0] * twiddle[W][1] +
					    y[B][1] * twiddle[W][0];
				} else {
					temp[0] = y[B][0];
					temp[1] = y[B][1];
				}

				y[B][0] = y[T][0] - temp[0];
				y[B][1] = y[T][1] - temp[1];

				y[T][0] = y[T][0] + temp[0];
				y[T][1] = y[T][1] + temp[1];

			}
		}		// End of block loop
		Nblock <<= 1;	// Next stage will have twice the number of blocks
		Nb <<= 1;	// Next stage will also have twice the number of butterflies
	}			// End of pass loop
	// Freeing up twiddle factor
	if (twiddle_flag)
		bina_aligned_free(twiddle);
	return 0;
}

int bina_radix2_ifft(uint8_t log2N,	// log2(fft_size) (prevents radix check)
		     bina_complex * twiddle,	// Real and imaginary twiddle factor coefs (CONSTANTS)
		     bina_complex * y)	// Real and imaginary coefficients      
{
	// Some local variables
	size_t fft_size = 0x1 << log2N;
	uint8_t twiddle_flag = 0;
	// In order to get the most from the fft routine, twiddle factor can be calculated
	// into a lookup table by calling the function bina_create_twiddle_lookup.
	// If not calculated, this portion allocates and computes twiddle factors.
	if (twiddle == NULL) {
		if ((twiddle = bina_create_twiddle_lookup(log2N)) == NULL) {
			return -1;	// Error: allocating memory for twiddle factor
		}
		twiddle_flag = 1;
	}
	if (fft_size == 1)
		return 0;	// DFT of a constant is the constant itself

	size_t Nblock, blocks, Nb, even, odd;
	size_t k, W, B, T;
	bina_complex temp;

	// Bit-reversing the sequence
	_fft_bitreverse_swap_complex_i(y, log2N);

	Nblock = 2;		// Number of blocks
	Nb = 1;			// Number of butterflies
	// Start calculating FFT
	for (blocks = fft_size >> 1; blocks > 0; blocks >>= 1) {
		for (even = 0; even < fft_size; even += Nblock) {
			odd = even + Nb;	// Calculate the odd index
			for (k = 0; k < Nb; k++) {
				W = k * blocks;
				T = even + k;
				B = odd + k;
				if (W) {
					temp[0] =
					    y[B][0] * twiddle[W][0] +
					    y[B][1] * twiddle[W][1];
					temp[1] =
					    y[B][1] * twiddle[W][0] -
					    y[B][0] * twiddle[W][1];
				} else {
					temp[0] = y[B][0];
					temp[1] = y[B][1];
				}

				y[B][0] = (y[T][0] - temp[0]) * 0.5f;
				y[B][1] = (y[T][1] - temp[1]) * 0.5f;

				y[T][0] = (y[T][0] + temp[0]) * 0.5f;
				y[T][1] = (y[T][1] + temp[1]) * 0.5f;

			}
		}		// End of block loop
		Nblock <<= 1;	// Next stage will have twice the number of blocks
		Nb <<= 1;	// Next stage will also have twice the number of butterflies
	}			// End of pass loop
	// Freeing up twiddle factor
	if (twiddle_flag)
		bina_aligned_free(twiddle);
	return 0;
}
#endif				/* __AVX__ */

#endif				/* _BINA_FFT_H_ */
