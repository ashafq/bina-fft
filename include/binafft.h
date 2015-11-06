/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* binafft.h - main API header
*******************************************************************************/

#ifndef BINA_FFT_BINAFFT_H
#define BINA_FFT_BINAFFT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void *bina_transform;

/* Use C99 Complex if complex.h is included prior to this file */
#if defined(_Complex_I) && defined(complex) && defined(I)

/* Eff it, I am going to meta-program the sheet out of this */

#	define BINA_DECL_TYPE(type, suffix) \
	typedef type bina_real##suffix; \
	typedef _Complex type bina_complex##suffix;

#else /* Use bit-compatible complex type */

#	define BINA_DECL_TYPE(type, suffix) \
	typedef type bina_real##suffix; \
	typedef type bina_complex##suffix[2];
#endif

/* Data types */

BINA_DECL_TYPE(double, );

int bina_transform_execute(bina_transform);
int bina_transform_free(bina_transform);

/* Transform types */
bina_transform bina_transform_create_radix2_c2c_fft
		(bina_complex *in, bina_complex *out,
		int fft_length, int flags);

#undef BINA_DECL_TYPE

#ifdef __cplusplus
}				/* extern "C" */
#endif

#endif				/* BINA_FFT_BINAFFT_H */
