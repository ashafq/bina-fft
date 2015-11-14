#include <stdio.h>
#include <stdlib.h>
#include <complex.h>

#include "binafft.h"
#include "internal/aligned_malloc.h"

#define bina_complex_alloc(n) aligned_calloc(16, n, sizeof(bina_complex))
#define bina_complex_free(ptr) aligned_free(ptr)

static void printc(bina_complex *arr, size_t len);

int main()
{
	int fft_len = 16;
	int flags = 0;

	bina_complex *in = bina_complex_alloc(fft_len);
	bina_complex *out = bina_complex_alloc(fft_len);
	bina_transform transform = bina_transform_create_radix2_c2c_fft(in, out, fft_len, 0);

	in[0] = 0.0 + 1.0*I;
	bina_transform_execute(transform);

	printc(in, fft_len);
	printc(out, fft_len);

	bina_transform_free(transform);
	bina_complex_free(in);
	bina_complex_free(out);
	return 0;
}

static void printc(bina_complex *arr, size_t len)
{
	const char *line = "-------------------------";

	puts(line);

	for(size_t i = 0; i < len; i++) {
		printf("(%e, %e)\n", creal(arr[i]), cimag(arr[i]));
	}

	puts(line);
}

