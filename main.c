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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <BinaFFT.h>

void print_complex_array(bina_complex* x, int N, const char* msg);
float rand_float(float a, float b);
double get_time();
void benchmark_i(uint8_t start, uint8_t end, size_t iterations);
void test_code(void);

int main(void)
{
	printf("%s", bina_fft_logo());
	printf("Enter the number of iteration (-1 to skip to default value of 8000): ");
	int iterations;
	scanf(" %d", &iterations);
	if(iterations == (-1)) iterations = 8000;
	benchmark_i(4, 18, iterations);
	
	printf("\nDo you want to run a test sample? (y/n) ");
	char yes;
	scanf(" %c", &yes);
	if(!(yes - 'y'))
		test_code();
	printf("Thanks for evaluating my program! (Ayan Shafqat)\n");
	return 0;
}

double get_time()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}
float rand_float(float a, float b)
{
	return ((b-a)*((float)rand()/RAND_MAX))+a;
}
void print_complex_array(bina_complex* x, int N, const char* msg)
{
	#define printf_macro    "% 7.4g "
	#define printf_macro_i  "%7.4gi"
	int i;
	printf("%s\n--------------------\n", msg);
	for(i = 0; i < N; i++){
			if(x[i][0] == 0.0f  && x[i][1] == 0.0f){
				printf("0");			
			}else if(x[i][0] == 0.0f){
				printf(printf_macro_i, x[i][1]);
			} else if(x[i][1] == 0.0f){
				printf(printf_macro, x[i][0]);
			} else {
				printf(printf_macro, x[i][0]);
				if(x[i][1] < 0.0f){
					printf(" - ");
				} else {
					printf(" + ");
				}
				printf(printf_macro_i, fabs(x[i][1]));
			}
			printf("\n");
	}
	printf("--------------------\n");
	#undef printf_macro
	#undef printf_macro_i
}
void benchmark_i(uint8_t start, uint8_t end, size_t iterations)
{
	printf("\n==[ Benchmark Testing (cfft)]================\n");
	const size_t max_size = (0x1 << end);
	uint8_t log2N;
	size_t it, N, n;
	_mem_align(32) bina_complex input[max_size];
	_mem_align(32) bina_complex twiddle[max_size >> 1];
	double t0, t1, t_av;
	for(log2N = start; log2N <= end; log2N++){
		fft_calculate_twiddle(log2N, twiddle);
		it = iterations;
		N = 0x1 << log2N;
		t_av = 0.f;
		while(it--){
			n = N;
			// Filling up test array
			while(n--) {
				input[n][0] = rand_float(-1.0f, 1.0f);
				input[n][1] = rand_float(-1.0f, 1.0f);
			}


			t0 = get_time();
			bina_radix2_fft(log2N, twiddle, input);
			t1 = get_time();

			t_av += ((double)t1 - (double)t0);
		}
		t_av /= (double)iterations;
		printf("FFT ( BinaFFT ) of size %7lu took %e seconds\n", N, t_av);
	}
}
void test_code(void){
	printf("\n==[ Running Test Code (cfft)]================\n");
	size_t log2N = 4, N = 0x1 << log2N;
	bina_complex* in = bina_aligned_malloc(N*sizeof(bina_complex), 32);
	bina_complex* twiddle = bina_create_twiddle_lookup(log2N);
	
	for(int i = N; i--; ) { in[i][0] = rand_float(-1.f,1.f); in[i][1] = rand_float(-1.f,1.f); }
	print_complex_array(in, N, "in");

	print_complex_array(twiddle, N >> 1, "Twiddle factors");
	bina_fft(in);
	print_complex_array(in, N, "fft");	
	bina_ifft(in);
	print_complex_array(in, N, "ifft");		

	// Cleaning up
	bina_aligned_free(in);
	bina_aligned_free(twiddle);
	return;
}
#if 0
// DEBUG TEST CODES Mar 6, 2012
#include "fft_kernels.h"
void debug_avx(__m256 v, const char* msg){
	printf("+------------------------------------------------+\n");
	printf("%c%c%c |", msg[0],msg[1],msg[2]);
	typedef union {
		__m256 V;
		float  f[8];
	} weirdtype;
	weirdtype temp;
	temp.V = v;
	float *x = temp.f;
	unsigned char i;
	for(i = 8; i--; ){
		if(i == 3) printf("\n");
		printf("  %5.2e", x[i]);
	}
	printf("  |\n");
	
	printf("+------------------------------------------------+\n");
}
static inline __m128 _mm_complex_mult_ps_2(__m128 a, __m128 b)
{
	// Given a vector [ar ai br bi] * [cr ci dr di]
	// ab_real = [ar*cr ai*ci br*dr bi*di]
	__m128 ab_real = _mm_mul_ps(a,b);
	print_complex_array((bina_complex*)&ab_real, 2, "mul-real");
	// ab_imag = [ar*ci ai*cr br*di bi*dr]
	__m128 ab_imag = _mm_mul_ps(a, _mm_permute_ps(b, _MM_SHUFFLE(2,3,0,1)));
	print_complex_array((bina_complex*)&ab_imag, 2, "mul-imag");
	ab_real = _mm_sub_ps(ab_real, _mm_permute_ps(ab_real, _MM_SHUFFLE(2,3,0,1)));
	print_complex_array((bina_complex*)&ab_real, 2, "mul-real_2");
	ab_imag = _mm_add_ps(ab_imag, _mm_permute_ps(ab_imag, _MM_SHUFFLE(2,3,0,1)));
	print_complex_array((bina_complex*)&ab_imag, 2, "mul-imag_2");
	ab_real = _mm_shuffle_ps(ab_real, ab_imag, _MM_SHUFFLE(1,2,0,2));
	print_complex_array((bina_complex*)&ab_real, 2, "putting them together");
	return _mm_permute_ps(ab_real, _MM_SHUFFLE(2,0,3,1));
}
int main (void)
{
	_mem_align(16) bina_complex top[2] = {{2,1}, {1,3}};
	_mem_align(16) bina_complex bot[2] = {{1,3}, {6,-1}};
	_mem_align(16) bina_complex TTT[2] = {{1,1}, {1,1}};
	print_complex_array(top, 2, "top");
	print_complex_array(bot, 2, "bot");
	*(__m128*)TTT = _mm_complex_mult_ps_2(*(__m128*)top, *(__m128*)bot);
	print_complex_array(TTT, 2, "twiddle");
	

	return 0;
}
int ma2in(void)
{
	const size_t log2N = 3, N = 0x1 << log2N;
	_mem_align(16) bina_complex in[N];
	_mem_align(16) bina_complex twiddle[N];
	//fft_calculate_twiddle(log2N, twiddle);
	for(size_t i = 0; i < N>>1; i++) {
		printf("%lu %lu\n", i, i + N>>1); 
		in[i][0] = (float)i + 0.555f; 
		in[i][1] = (float)i + 0.111; 
		in[i + (N>>1)][0] = (float)i + 0.555f; 
		in[i + (N>>1)][1] = (float)i + 0.111; 
		twiddle[i][0] = -1.f; twiddle[i][1] = -1.f;
	}
	print_complex_array(in, N, "input");
	print_complex_array(twiddle, N>>2, "twiddle");
	_mm_fft_butterfly_times_twiddle((__m128*)in[0], (__m128*)in[N>>1], *(__m128*)twiddle);
	print_complex_array(in, N, "out");
	return 0;
}
#endif

