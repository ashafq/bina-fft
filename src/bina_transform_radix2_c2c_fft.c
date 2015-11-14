/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* bina_transform_radix2_c2c.c - Contains a basic implementation of a radix 2
* Fast Fourier Transform.
*******************************************************************************/

#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binafft.h"
#include "internal/aligned_malloc.h"
#include "internal/bina_transform.h"
#include "internal/bitmagic.h"
#include "internal/log.h"

#ifndef BINA_FFT_ALIGNMENT
#define BINA_FFT_ALIGNMENT (16U)
#endif

#ifndef M_PI
#define M_PI   (3.14159265358979323846264338327950288)
#endif

/*******************************************************************************
* Data structure
*******************************************************************************/
struct radix2_c2c_fft {
	struct bina_transform type;     /* Base class */

	unsigned int fft_length;        /* Number of points of FFT */

	unsigned int radix;             /* lg2(n) points of FFT */

	bina_complex *in;               /* Pointer to input buffer */

	bina_complex *out;              /* Pointer to output buffer */

	bina_complex *temp;             /* Pointer to temporary buffer */

	bina_complex *twiddle;          /* Pointer to twiddle factor buffer */

	unsigned int *permutation;      /* FFT leads the results in bit
					 * reversed order, this vector
					 * is needed to place them back to
					 * original order.
					 */
};

/*******************************************************************************
* Functions
*******************************************************************************/

static bina_complex *calculate_twiddle(int);

static unsigned int *calculate_permutation_vector(int);

static void permute_buffer(const unsigned int *pvec, bina_complex *in,
		bina_complex *out, int length);

static void radix2_c2c_fft_butterflies(bina_complex *in,
		bina_complex *out,
		const bina_complex *twiddle,
		int num_dft,
		int num_butterflies);

static struct radix2_c2c_fft *allocate_class(void);

static int free_class(bina_transform);

static int radix2_c2c_fft_exec(bina_transform);

/*******************************************************************************
* Implementations
*******************************************************************************/

/* Allocates a radix-2 complex-to-complex FFT transform class.
 *
 * @in: Pointer to input buffer
 * @out: Pointer to output buffer
 * @fft_length: Length of the problem (MUST be power of two)
 * @flags: Extra flags.
 *
 * @returns A new transform class
 */
bina_transform bina_transform_create_radix2_c2c_fft(bina_complex *in,
		bina_complex *out, int fft_length, int flags)
{
	struct radix2_c2c_fft *self = NULL;
	bina_complex *twiddle = NULL;
	bina_complex *temp_buffer = NULL;
	unsigned int *perm = NULL;

	if (!ispowtwo(fft_length)) {
		log_error("Length is not power of two\n");
		return NULL;
	}


	if ((twiddle = calculate_twiddle(fft_length)) == NULL) {
		log_error("Allocating twiddle factor buffer\n");
		return NULL;
	}

	if ((perm = calculate_permutation_vector(fft_length)) == NULL) {
		log_error("Allocating permutation vector buffer\n");

		/* Clean up other things */
		aligned_free(twiddle);

		return NULL;
	}

	if ((temp_buffer = aligned_calloc(BINA_FFT_ALIGNMENT, fft_length,
			sizeof(bina_complex))) == NULL) {
		log_error("Allocating temporary buffer\n");

		/* Clean up other things */
		aligned_free(twiddle);
		aligned_free(perm);

		return NULL;
	}


	if ((self = allocate_class()) == NULL) {
		log_error("Allocating tranform object instance\n");

		/* Clean up other things */
		aligned_free(twiddle);
		aligned_free(perm);
		aligned_free(temp_buffer);

		return NULL;
	}

	/* Setting members */

	self->fft_length = fft_length;
	self->radix = ilog2(fft_length);
	self->twiddle = twiddle;
	self->permutation = perm;
	self->in = in;
	self->out = out;
	self->temp = temp_buffer;

	return self;
}

/* Helper function to allocate the class and set up callbacks
 *
 * @returns A new transform class
 */
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

/* Calculate twiddle factor table depending on problem size, and reshuffles
 * them to radix-2 DIF FFT.
 *
 * @fft_length: Length of FFT.
 *
 * @return Twiddle factor table.
 */
static bina_complex *calculate_twiddle(int fft_length)
{
	bina_complex *twiddle = aligned_calloc(BINA_FFT_ALIGNMENT, fft_length,
			sizeof(bina_complex));

	if (twiddle == NULL) {
		return NULL;
	}

	/* Start populating the twiddle factor table */

	bina_complex *tptr = twiddle;
	const bina_complex omega = 2.0 * M_PI / fft_length * _Complex_I;
	int tlen = fft_length / 2;
	int radix = ilog2(fft_length);

	for (int r = 0; r < radix; r++) {

		if(r == 0) {

			/* For the first half of the twiddle table, calculate
			 * half of the unit circle [0, pi).
			 */
			for (int i = 0; i < tlen; i++) {
				*tptr++ = cexp(omega * i);
			}

		} else {

			/* Take the first segment and copy it over */
			int skip = 1 << r;
			bina_complex *tptr2 = twiddle;

			for (int i = 0; i < tlen; i++) {
				*tptr++ = *tptr2;
				tptr2 += skip;
			}

		}

		tlen = tlen / 2;
	}

	return twiddle;
}

/* Calculate bit-reversed lookup table for an FFT of a specific length.
 *
 * @fft_length: Length of FFT.
 *
 * @return Bit-reversed table of length n/2.
 */
static unsigned int *calculate_permutation_vector(int fft_length)
{
	unsigned int *perm = aligned_calloc(BINA_FFT_ALIGNMENT, fft_length/2,
			sizeof(unsigned int));

	if (perm == NULL) {
		return NULL;
	}

	/* Calculate all even index */
	unsigned int *p = perm;
	unsigned int rshift = 31 - ilog2(fft_length);

	for (unsigned int i = 0; i < fft_length; i+=2) {
		*p++ = revbin(i) >> rshift;
	}

	return perm;
}

/* Function to execute radix-2 complex-to-complex FFT class.
 *
 * @base: Pointer to instance of transform object.
 *
 * @return Nonzero value is returned upon failure, and zero upon success.
 *
 */
static int radix2_c2c_fft_exec(bina_transform base)
{
	/* Get self from base class */
	struct radix2_c2c_fft *self = (struct radix2_c2c_fft *) base;
	int fft_length = self->fft_length;

	/* There are log2(n) stages in FFT */
	int lg2n = self->radix;
	/* Initially there are n/2 butterflies */
	int num_butterflies = fft_length / 2;
	/* Initially the stage DFT length is 2 */
	int num_stage_dft = 1;

	/* Do the first stage and copy the result into temp buffer */
	bina_complex *in = self->in;
	bina_complex *out = self->temp;
	const bina_complex *tw = self->twiddle;

	radix2_c2c_fft_butterflies(in, out, tw, num_stage_dft, num_butterflies);

	/* Update in/out pointers */
	in = self->temp;
	out = self->out;

	/* Updating pointer to next set of twiddle factors */
	tw += num_butterflies;
	/* Next stage has twice the number of DFT */
	num_stage_dft *= 2;
	/* Next stage has half the amount of butterflies */
	num_butterflies /= 2;


	for (int stage = 1; stage < lg2n; stage++) {

		radix2_c2c_fft_butterflies(in, out, tw, num_stage_dft,
				num_butterflies);

		/* Updating pointer to next set of twiddle factors */
		tw += num_butterflies;
		/* Next stage has twice the number of DFT */
		num_stage_dft *= 2;
		/* Next stage has half the amount of butterflies */
		num_butterflies /= 2;

		/* Update in/out pointers */
		bina_complex *temp = in;
		in = out;
		out = temp;
	}

	/* The output should be in bit-reversed order now, and needs to be
	 * reshuffled.
	 */
	const unsigned int *perm = self->permutation;
	permute_buffer(perm, in, out, fft_length);

	/* Copy to output buffer if out is pointing to temporary buffer
	 *
	 * TODO: Find a better way of doing this, or investigate
	 *       the likelyhood of hitting this branch.
	 */
	if(out == self->temp) {
		in = self->temp;
		out = self->out;
		memcpy(out, in, fft_length * sizeof(bina_complex));
	}

	return 0;
}

/*
 * Calculates butterflies in stage.
 *
 * @in Pointer to input buffer.
 * @out Pointer to output buffer.
 * @twiddle Pointer to twiddle factor buffer.
 * @num_dft Number of DFT to perform in current stage.
 * @num_butterflies Number of butterflies in current stage.
 *
 * @return None
 */
static void radix2_c2c_fft_butterflies(bina_complex *in,
		bina_complex *out,
		const bina_complex *twiddle,
		int num_dft,
		int num_butterflies)
{
	for (int j = 0; j < num_dft; j++) {

		int offset = (j * num_butterflies) * 2;

		for (int i = 0; i < num_butterflies; i++) {

			int top = i + offset;
			int bot = i + offset + num_butterflies;

			bina_complex yt = (in[top] + in[bot]);
			bina_complex yb = (in[top] - in[bot]) * twiddle[i];

			out[top] = yt;
			out[bot] = yb;
		}

	}

}

/* Reshuffles FFT output in the bit-reversed order that is precomputed
 * in lookup table.
 *
 * @pvec Bit-reversed look up table.
 * @in Pointer to input buffer.
 * @out Pointer to output buffer.
 * @length Self explanatory.
 *
 * @returns Nothing.
 */
static void permute_buffer(const unsigned int *pvec, bina_complex *in,
		bina_complex *out, int length)
{
	int pvec_length = length / 2;
	for (int i = 0; i < pvec_length; i++) {
		int even_index = 2 * i;
		int odd_index = 2 * i + 1;
		int even_index_rev = pvec[i];
		int odd_index_rev = even_index | 0x00000001;

		out[even_index] = in[even_index_rev];
		out[odd_index] = in[odd_index_rev];
	}
}

/* Helper function to free the transform class.
 *
 * @base: Pointer to instance of transform object.
 *
 * @return Nonzero value is returned upon failure, and zero upon success.
 */
static int free_class(bina_transform base)
{
	struct radix2_c2c_fft *self = (struct radix2_c2c_fft *) base;
	aligned_free(self->twiddle);
	aligned_free(self->permutation);
	aligned_free(self->temp);
	free(self);
	return 0;
}

