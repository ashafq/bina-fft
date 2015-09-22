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
	/*
	 * Single precision types
	 */
	typedef float bina_realf_t;

	/* Use C99 Complex if complex.h is included prior to this file */
#	if defined(_Complex_I) && defined(complex) && defined(I)
	typedef _Complex float bina_cmplxf_t;
#	else			/* use bit-equavalent complex type */
	typedef float bina_cmplxf_t[2];
#	endif			/* _Complex_I */

	/*
	 * Double precision types
	 */
	typedef double bina_real_t;

	/* Use C99 Complex if complex.h is included prior to this file */
#	if defined(_Complex_I) && defined(complex) && defined(I)
	typedef _Complex double bina_cmplx_t;
#	else			/* use bit-equavalent complex type */
	typedef double bina_cmplx_t[2];
#	endif			/* _Complex_I */

	/*
	 * Long precision types
	 */
#	ifdef BINA_FFT_HAS_LONG_DOUBLE
	typedef long double bina_reall_t;

	/* Use C99 Complex if complex.h is included prior to this file */
#	if defined(_Complex_I) && defined(complex) && defined(I)
	typedef _Complex long double bina_cmplxl_t;
#	else			/* use bit-equavalent complex type */
	typedef long double bina_cmplxl_t[2];
#	endif			/* _Complex_I */
#	endif			/* ifndef BINA_FFT_HAS_LONG_DOUBLE */

#ifdef __cplusplus
};				/* extern "C" */
#endif

#endif				/* BINA_FFT_BINAFFT_H */
