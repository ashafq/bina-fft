/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* bina_transform_radix2_c2c.c - Contains a basic implementation of a radix 2
* Fast Fourier Transform.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <complex.h>

#include "binafft.h"
#include "internal/bina_transform.h"

/*******************************************************************************
* Data structure
*******************************************************************************/
struct radix2_c2c_fft {
	struct bina_transform type;
	bina_complex *in;
	bina_complex *out;
	bina_complex *twiddle;
};

/*******************************************************************************
* Local functions
*******************************************************************************/
static bool is_power_of_two(int);
static struct radix2_c2c_fft *allocate_class(void);
static int free_class(bina_transform);
static int radix2_c2c_fft_exec(bina_transform);

/*******************************************************************************
* Implementations
*******************************************************************************/

bina_transform bina_transform_create_radix2_c2c_fft
    (bina_complex * in, bina_complex * out, int fft_length, int flags) {
	struct radix2_c2c_fft *self = NULL;

	puts("Called bina_transform_create_radix2_c2c_fft");

	self = allocate_class();

	return self;
}

static struct radix2_c2c_fft *allocate_class(void)
{
	struct radix2_c2c_fft *self = NULL;

	/* Set up pointers */
	self = malloc(sizeof(struct radix2_c2c_fft));

	if (self == NULL) {
		return NULL;
	}

	/* Set up callbacks */
	self->type.execute = &(radix2_c2c_fft_exec);
	self->type.free = &(free_class);

	return self;
}

static int free_class(bina_transform base)
{
	struct radix2_c2c_fft *self = base;
	puts("Called radix2_fft_free");
	free(self);
	return 0;
}

static int radix2_c2c_fft_exec(bina_transform base)
{
	puts("Called radix2_c2c_fft_exec");
	return 0;
}

/* Returns true if n is a power of two
 * @n is any integer
 */
static bool is_power_of_two(int n)
{
	/* n is a power of two if an only if n is
	 * (1) Greater than zero
	 * (2) n & (n - 1) == 0
	 */
	return (n) && !(n & (n - 1));
}
