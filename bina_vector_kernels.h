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

#ifndef _BINA_FFT_KERNELS_H_
#define _BINA_FFT_KERNELS_H_

static inline __m128 _mm_load_complex_ps(bina_complex a, bina_complex b)
{
	__m128 data = _mm_setzero_ps();
	data = _mm_loadl_pi(data, (__m64*)a);
	data = _mm_loadh_pi(data, (__m64*)b);
	return data;
}
static inline __m256 _mm256_load_complex_ps(bina_complex a, bina_complex b, bina_complex c, bina_complex d)
{
	__m128 dat_lo = _mm_setzero_ps();
	__m256 data   = _mm256_setzero_ps();
	dat_lo = _mm_loadl_pi(dat_lo, (__m64*)a);
	dat_lo = _mm_loadh_pi(dat_lo, (__m64*)b);
	data   = _mm256_insertf128_ps(data, dat_lo, 0);
	dat_lo = _mm_loadl_pi(dat_lo, (__m64*)c);
	dat_lo = _mm_loadh_pi(dat_lo, (__m64*)d);
	data   = _mm256_insertf128_ps(data, dat_lo, 1);
	return data;
}
static inline __m128 _mm_complex_mult_ps(__m128 a, __m128 b)
{
	// Given a vector [ar ai br bi] * [cr ci dr di]
	// ab_real = [ar*cr ai*ci br*dr bi*di]
	__m128 ab_real = _mm_mul_ps(a,b);
	// ab_imag = [ar*ci ai*cr br*di bi*dr]
	__m128 ab_imag = _mm_mul_ps(a, _mm_permute_ps(b, _MM_SHUFFLE(2,3,0,1)));
	ab_real = _mm_sub_ps(ab_real, _mm_permute_ps(ab_real, _MM_SHUFFLE(2,3,0,1)));
	ab_imag = _mm_add_ps(ab_imag, _mm_permute_ps(ab_imag, _MM_SHUFFLE(2,3,0,1)));
	ab_real = _mm_shuffle_ps(ab_real, ab_imag, _MM_SHUFFLE(1,2,0,2));
	return _mm_permute_ps(ab_real, _MM_SHUFFLE(2,0,3,1));
}
static inline __m256 _mm256_complex_mult_ps(__m256 a, __m256 b)
{
	__m256 ab_real = _mm256_mul_ps(a,b);
	__m256 ab_imag = _mm256_mul_ps(a, _mm256_permute_ps(b, _MM_SHUFFLE(2,3,0,1)));
	ab_real = _mm256_sub_ps(ab_real, _mm256_permute_ps(ab_real, _MM_SHUFFLE(2,3,0,1)));
	ab_imag = _mm256_add_ps(ab_imag, _mm256_permute_ps(ab_imag, _MM_SHUFFLE(2,3,0,1)));
	ab_real = _mm256_shuffle_ps(ab_real, ab_imag, _MM_SHUFFLE(1,2,0,2));
	return _mm256_permute_ps(ab_real, _MM_SHUFFLE(2,0,3,1));
}
static inline void _mm256_store_complex2_ps(__m256 x, bina_complex a[2], bina_complex b[2])
{
	__m128 dat_lo = _mm256_extractf128_ps(x, 0);
	__m128 dat_hi = _mm256_extractf128_ps(x, 1);
	_mm_store_ps((float*)a, dat_lo);
	_mm_store_ps((float*)b, dat_hi);
}
static inline void _mm256_store_complex_ps(__m256 x, bina_complex a, bina_complex b, bina_complex c, bina_complex d)
{
	__m128 dat_lo = _mm256_extractf128_ps(x, 0);
	__m128 dat_hi = _mm256_extractf128_ps(x, 1);
	_mm_storel_pi((__m64*)a, dat_lo);
	_mm_storeh_pi((__m64*)b, dat_lo);
	_mm_storel_pi((__m64*)c, dat_hi);
	_mm_storeh_pi((__m64*)d, dat_hi);
	return;	
}
// 
//  FFT Kernels
//  binafft_v1
//  
//  Created by Ayan Shafqat on 2012-03-07.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
// 

static inline __m128 _mm_fft_butterfly(__m128 v)
{
	/*
	How this works:
	by loading two complex number array into one vector
	we get this: v = [ar, ai, br, bi]
	A butterfly is basically a+b and a-b
	or: [ai-bi, ar-br, ai+bi, ar+br]
	by using _mm_addsub_ps(...) we should be able to do:
	[ai-bi, ai+bi, ar-br, ar+br]
	then, we can permute this vector to get:
	[ai-bi, ar-br, ai+bi, ar+br] (done)
	*/
	__m128 result = _mm_addsub_ps(
	_mm_permute_ps(v, _MM_SHUFFLE(1,1,0,0)),
	_mm_permute_ps(v, _MM_SHUFFLE(3,3,2,2)) 
	);
	return _mm_permute_ps(result, _MM_SHUFFLE(2,0,3,1));
}
static inline __m256 _mm256_fft_two_butterflies(__m256 v)
{
	__m256 result = _mm256_addsub_ps(
	_mm256_permute_ps(v, _MM_SHUFFLE(1,1,0,0)),
	_mm256_permute_ps(v, _MM_SHUFFLE(3,3,2,2)) 
	);
	return _mm256_permute_ps(result, _MM_SHUFFLE(2,0,3,1));
}
static inline void _mm_fft_butterfly_times_twiddle(__m128* top_t, __m128* bot_t, __m128 twiddle)
{
	__m128 bot  = *(bot_t); // [b0r b0i b1r b1i]
	__m128 top  = *(top_t); // [t0r t0i t1r t1i]
	bot = _mm_complex_mult_ps(bot, twiddle);
	*(top_t) = _mm_add_ps(top, bot);
	*(bot_t) = _mm_sub_ps(top, bot);
}
static inline void _mm256_fft_butterly_times_twiddle(__m256* top_t, __m256* bot_t, __m256 twiddle)
{
	__m256 bot  = *(bot_t); // [b0r b0i b1r b1i]
	__m256 top  = *(top_t); // [t0r t0i t1r t1i]
	bot = _mm256_complex_mult_ps(bot, twiddle);
	*(top_t) = _mm256_add_ps(top, bot);
	*(bot_t) = _mm256_sub_ps(top, bot);
}

static inline __m256 _mm256_fft_kernel_4(__m256 x)
{
	__m128 hi, lo, temp;
	// First step is to permute the vector
	lo = _mm256_extractf128_ps(x, 0);
	hi = _mm256_extractf128_ps(x, 1);

	temp = _mm_shuffle_ps(lo, hi, _MM_SHUFFLE(1,0,1,0));
	hi   = _mm_shuffle_ps(lo, hi, _MM_SHUFFLE(3,2,3,2));

	// Perform the butterflies for first stage
	lo = _mm_fft_butterfly(temp);
	hi = _mm_fft_butterfly(hi);

	// Now at second stage	
	// Multiplying by the twiddle factor (-j)
	hi = _mm_xor_ps(
		 _mm_permute_ps(hi, _MM_SHUFFLE(2,3,1,0)), 
		 _mm_set_ps(0.f, -0.f, 0.f, 0.f)
		);


	temp = _mm_shuffle_ps(lo, hi, _MM_SHUFFLE(1,0,1,0));
	hi   = _mm_shuffle_ps(lo, hi, _MM_SHUFFLE(3,2,3,2));


	// Do the butterfly for 2nd stage
	lo = _mm_fft_butterfly(temp);
	hi = _mm_fft_butterfly(hi);
	
	// Shuffling them back 
	temp = _mm_shuffle_ps(lo, hi, _MM_SHUFFLE(3,2,1,0));
	hi   = _mm_shuffle_ps(lo, hi, _MM_SHUFFLE(1,0,3,2));
	
	
	// Insert back into large vector
	x = _mm256_insertf128_ps(x, temp, 0);
	x = _mm256_insertf128_ps(x, hi  , 1);
	
	// DONE!
	return x;
}
static inline void _swap_odd_even(bina_complex* arr, size_t length){
  if(length<=1) return;
  size_t i = 1, N = length >> 1, j;
  for(i = 1; i < N; i++){
    //swap i with i*2
	j = i << 1;
    __m64 temp = *(__m64*)arr[i];
    *(__m64*)arr[i] = *(__m64*)arr[j];
    *(__m64*)arr[j] = temp;
  }
  _swap_odd_even(arr+i, length-i);
}
static inline void _mm256_fft_kernel_8(bina_complex v[8], bina_complex twiddle[4])
{
	_swap_odd_even(v, 8); // I am lazy to do this manually
	__m256 eve = _mm256_loadu_ps((float*)v[0]);
	__m256 odd = _mm256_loadu_ps((float*)v[4]);
	__m256 twd = _mm256_loadu_ps((float*)twiddle);
	eve = _mm256_fft_kernel_4(eve);
	odd = _mm256_fft_kernel_4(odd);
	odd = _mm256_complex_mult_ps(odd, twd);
	twd = _mm256_add_ps(eve, odd);
	odd = _mm256_sub_ps(eve, odd);
	_mm256_storeu_ps((float*)v[0], twd);
	_mm256_storeu_ps((float*)v[4], odd);
	return;
}
static inline void dft2(bina_complex a, bina_complex b)
{
	// complex float temp = *(x1);
	// *(x1)  = *(x0) - temp;
	// *(x0)  = *(x0) + temp;
	
	#ifdef _FFT_VECTOR_SUPPORT_ENABLED_
	
	#if defined (__AVX__)
 	__m128 xmm1 = _mm_setzero_ps();
	__m128 xmm0 = _mm_setzero_ps();
	xmm0 = _mm_loadh_pi(xmm0, (__m64*)a);
	xmm0 = _mm_loadl_pi(xmm0, (__m64*)b);
	xmm0 = _mm_permute_ps(xmm0, _MM_SHUFFLE(3, 1, 2, 0));
	xmm1 = _mm_permute_ps(xmm0, _MM_SHUFFLE(2, 3, 0, 1));
	xmm1 = _mm_addsub_ps(xmm1, xmm0);
	xmm1 = _mm_permute_ps(xmm1, _MM_SHUFFLE(3, 1, 2, 0));
	_mm_storeh_pi((__m64*)a, xmm1);
	_mm_storel_pi((__m64*)b, xmm1);
	#else
 	__m128 xmm1 = _mm_setzero_ps();
	__m128 xmm0 = _mm_setzero_ps();
	xmm0 = _mm_loadh_pi(xmm0, (__m64*)a);
	xmm0 = _mm_loadl_pi(xmm0, (__m64*)b);
	xmm0 = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(3, 1, 2, 0));
	xmm1 = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(2, 3, 0, 1));
	xmm1 = _mm_addsub_ps(xmm1, xmm0);
	xmm1 = _mm_shuffle_ps(xmm1, xmm1, _MM_SHUFFLE(3, 1, 2, 0));
	_mm_storeh_pi((__m64*)a, xmm1);
	_mm_storel_pi((__m64*)b, xmm1);
	#endif /* __AVX__ */
	
	#else
	bina_complex temp;
		
	temp[0] = b[0]; 
	temp[1] = b[1];
	
	b[0] = a[0] - temp[0]; 
	b[1] = a[1] - temp[1];
	
	a[0] = a[0] + temp[0];
	a[1] = a[1] + temp[1];
	#endif
	
}
static inline void dft2_times_twiddle(
					bina_complex a, 
					bina_complex b, 
					bina_complex twiddle)
{
	// Optimize here
	// complex float temp = *(x1) * *(twiddle);
	// 	*(x1)  = *(x0) - temp;
	// 	*(x0)  = *(x0) + temp;
	#ifdef _FFT_VECTOR_SUPPORT_ENABLED_
	// Vector code
	/* 
		How this step is done:
		(1) Load three things: a, b, and twiddle factor (t)
		(2) temp = twiddle * b
			temp_r = b_r * twiddle_r - b_i * twiddle_i
			temp_i = b_r * twiddle_i + b_i * twiddle_r
			
		(3) b = b - temp
			b_r = a_r - temp_r
			b_i = a_i - temp_i
			a_r = a_r + temp_r
			a_i = a_i + temp_i
			
			----- A --- BR -- T2 -------- BI ---- T1 ----
			b_r = a_r - b_r * twiddle_r - b_i * twiddle_i
			b_i = a_i - b_r * twiddle_i + b_i * twiddle_r
			a_r = a_r + b_r * twiddle_r - b_i * twiddle_i
			a_i = a_i + b_r * twiddle_i + b_i * twiddle_r
	*/
		#if defined (__AVX__)

		// For some reason, AVX benchmarks are slower :/

		__m128 xmm0 = _mm_setzero_ps();
		__m128 xmm1 = _mm_setzero_ps();
		__m128 xmm2 = _mm_setzero_ps();
		__m128 xmm3 = _mm_setzero_ps();
		__m128 xmm4 = _mm_setzero_ps();
		
		xmm0 = _mm_loadl_pi(xmm0, (__m64*)twiddle);
		xmm2 = _mm_loadl_pi(xmm2, (__m64*)b);
		xmm4 = _mm_loadl_pi(xmm4, (__m64*)a);

		xmm0 = _mm_permute_ps(xmm0, _MM_SHUFFLE(0,1,0,1)); // T1
		xmm1 = _mm_permute_ps(xmm0, _MM_SHUFFLE(0,1,0,1)); // T2
		xmm3 = _mm_permute_ps(xmm2, _MM_SHUFFLE(1,1,1,1)); // BI
		xmm2 = _mm_permute_ps(xmm2, _MM_SHUFFLE(0,0,0,0)); // BR

		xmm3 = _mm_mul_ps(xmm0, xmm3); // BI * T1
		xmm2 = _mm_mul_ps(xmm1, xmm2); // BR * T2

		xmm2 = _mm_addsub_ps(xmm2, xmm3); // (BR*T2) addsub (BI*T1)

		xmm2 = _mm_permute_ps(xmm2, _MM_SHUFFLE(3,1,2,0)); // BR Shuffled
		xmm4 = _mm_permute_ps(xmm4, _MM_SHUFFLE(1,1,0,0)); // A


		xmm4 = _mm_addsub_ps(xmm4, xmm2); // A addsub (BR Shuffled)
		xmm4 = _mm_permute_ps(xmm4, _MM_SHUFFLE(3,1,2,0)); // Shuflle back

		_mm_storeh_pi((__m64*)a, xmm4);
		_mm_storel_pi((__m64*)b, xmm4);

		#else
		
		// SSE4 code
		__m128 xmm0 = _mm_setzero_ps();
		__m128 xmm1 = _mm_setzero_ps();
		__m128 xmm2 = _mm_setzero_ps();
		__m128 xmm3 = _mm_setzero_ps();
		__m128 xmm4 = _mm_setzero_ps();
		xmm0 = _mm_loadl_pi(xmm0, (__m64*)twiddle);
		xmm2 = _mm_loadl_pi(xmm2, (__m64*)b);
		xmm4 = _mm_loadl_pi(xmm4, (__m64*)a);

		xmm0 = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(0,1,0,1)); // T1
		xmm1 = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(0,1,0,1)); // T2
		xmm3 = _mm_shuffle_ps(xmm2, xmm2, _MM_SHUFFLE(1,1,1,1)); // BI
		xmm2 = _mm_shuffle_ps(xmm2, xmm2, _MM_SHUFFLE(0,0,0,0)); // BR

		xmm3 = _mm_mul_ps(xmm0, xmm3); // BI * T1
		xmm2 = _mm_mul_ps(xmm1, xmm2); // BR * T2

		xmm2 = _mm_addsub_ps(xmm2, xmm3); // (BR*T2) addsub (BI*T1)
	
		xmm2 = _mm_shuffle_ps(xmm2, xmm2, _MM_SHUFFLE(3,1,2,0)); // BR Shuffled
		xmm4 = _mm_shuffle_ps(xmm4, xmm4, _MM_SHUFFLE(1,1,0,0)); // A
	
	
		xmm4 = _mm_addsub_ps(xmm4, xmm2); // A addsub (BR Shuffled)
		xmm4 = _mm_shuffle_ps(xmm4, xmm4, _MM_SHUFFLE(3,1,2,0)); // Shuflle back
	
		_mm_storeh_pi((__m64*)a, xmm4);
		_mm_storel_pi((__m64*)b, xmm4);
		#endif /* __AVX__ */
	
	#else
	bina_complex temp;
	temp[0] = b[0]*twiddle[0] - b[1]*twiddle[1];
	temp[1] = b[0]*twiddle[1] + b[1]*twiddle[0];
	
	b[0] = a[0] - temp[0]; 
	b[1] = a[1] - temp[1];
	
	a[0] = a[0] + temp[0];
	a[1] = a[1] + temp[1];
	#endif	
}
#endif /* _BINA_FFT_KERNELS_H_ */