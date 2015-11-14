/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* TODO: Write documentation
*
*******************************************************************************/

#include "internal/bitmagic.h"

/* Returns true if n is a power of two
 * @n is any integer
 * @return nonzero if n is a power of two
 */
int ispowtwo(int n)
{
	/* n is a power of two if an only if n is
	 * (1) Nonzero
	 * (2) n & (n - 1) == 0
	 */
	return (n) && (!(n & (n - 1)));
}

