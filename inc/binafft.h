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

/* This is how to namespace in C :) */
#define NMSP(X) bina_ ##X

/* A generic container for all transforms */
typedef struct transform *NMSP(transform);


#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* BINA_FFT_BINAFFT_H */

