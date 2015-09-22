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

void print_complex_array(bina_complex * x, int N, const char *msg);
float rand_float(float a, float b);
double get_time();
void benchmark_i(uint8_t start, uint8_t end, size_t iterations);
void test_code(void);

int main(void)
{
	printf("%s", bina_fft_logo());
	printf
	    ("Enter the number of iteration (-1 to skip to default value of 8000): ");
	int iterations;
	scanf(" %d", &iterations);
	if (iterations == (-1))
		iterations = 8000;
	benchmark_i(4, 18, iterations);

	printf("\nDo you want to run a test sample? (y/n) ");
	char yes;
	scanf(" %c", &yes);
	if (!(yes - 'y'))
		test_code();
	printf("Thanks for evaluating my program! (Ayan Shafqat)\n");
	return 0;
}

double get_time()
{
	struct timeval t;
	struct timezone tzp;
	gettimeofday(&t, &tzp);
	return t.tv_sec + t.tv_usec * 1e-6;
}

float rand_float(float a, float b)
{
	return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

void print_complex_array(bina_complex * x, int N, const char *msg)
{
#define printf_macro    "% 7.4g "
#define printf_macro_i  "%7.4gi"
	int i;
	printf("%s\n--------------------\n", msg);
	for (i = 0; i < N; i++) {
		if (x[i][0] == 0.0f && x[i][1] == 0.0f) {
			printf("0");
		} else if (x[i][0] == 0.0f) {
			printf(printf_macro_i, x[i][1]);
		} else if (x[i][1] == 0.0f) {
			printf(printf_macro, x[i][0]);
		} else {
			printf(printf_macro, x[i][0]);
			if (x[i][1] < 0.0f) {
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
	for (log2N = start; log2N <= end; log2N++) {
		fft_calculate_twiddle(log2N, twiddle);
		it = iterations;
		N = 0x1 << log2N;
		t_av = 0.f;
		while (it--) {
			n = N;
			// Filling up test array
			while (n--) {
				input[n][0] = rand_float(-1.0f, 1.0f);
				input[n][1] = rand_float(-1.0f, 1.0f);
			}

			t0 = get_time();
			bina_radix2_fft(log2N, twiddle, input);
			t1 = get_time();

			t_av += ((double)t1 - (double)t0);
		}
		t_av /= (double)iterations;
		printf("FFT ( BinaFFT ) of size %7lu took %e seconds\n", N,
		       t_av);
	}
}

void test_code(void)
{
	printf("\n==[ Running Test Code (cfft)]================\n");
	size_t log2N = 4, N = 0x1 << log2N;
	bina_complex *in = bina_aligned_malloc(N * sizeof(bina_complex), 32);
	bina_complex *twiddle = bina_create_twiddle_lookup(log2N);

	for (int i = N; i--;) {
		in[i][0] = rand_float(-1.f, 1.f);
		in[i][1] = rand_float(-1.f, 1.f);
	}
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
