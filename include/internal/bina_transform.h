/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* bina_transform.h - Definition of transform class
*******************************************************************************/

#ifndef BINA_FFT_INTERNAL_BINA_TRANSFORM_H
#define BINA_FFT_INTERNAL_BINA_TRANSFORM_H

/*******************************************************************************
* Data structures
*******************************************************************************/

/* The real transform class, hidden from API users */
struct bina_transform
{
	int (*execute)(bina_transform); /* Execute method */
	int (*free)(bina_transform); /* Deallocator callback */
};

#endif /* ifndef BINA_FFT_INTERNAL_BINA_TRANSFORM_H */

