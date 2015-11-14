/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* TODO: Write documentation
*
*******************************************************************************/

#include "internal/bitmagic.h"

/* Returns floor(log(n)/log(2))
 * @n any unsigned integer
 * @return nonzero if n is a power of two
 */
int ilog2(unsigned int n)
{
#       if defined(__GNUC__) || defined(__clang__)
        return (n) ? (31 - __builtin_clz(n)) : (n);
#       else
        int y = 0;
        while (n >>= 1) ++y;
        return y;
#       endif
}


