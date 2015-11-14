/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
*
* revbin.c - Contains function implementation of bit-reversing a binary
* sequence specific to the ARM chip.
*******************************************************************************/
#include "internal/revbin.h"

/* Reverses binary integer from least significant bit first to most significant
 * bit first. This algorithm is copied from Stanford Graphics's bit-hacking
 * page. For more information, see:
 * https://graphics.stanford.edu/~seander/bithacks.html#ReverseParallel
 *
 * Example:
 * 0x00000001 --> 0x80000000
 *
 * Note: for input of zero, the output of this function is also zero.
 *
 * @x The parameter to be reversed
 *
 * @return Reversed bit order of x
 */
uint32_t revbin(uint32_t x)
{
	asm("rbit %1, %0": "=r"(x):"r"(x));
	return x;
}


/* Same as revbin(), but with 64 bit support.
 * @x The parameter to be reversed
 * @return Reversed bit order of x
 */
uint64_t revbin64(uint64_t x)
{
	uint32_t lox = (uint32_t) (x);
	uint32_t hix = (uint32_t) (x >> 32U);

	/* Reverse bits for 32 bit segments */
	lox = revbin(lox);
	hix = revbin(hix);

	/* Swap the 32 bit segments */
	uint64_t out = ((uint64_t)lox << 32U) | ((uint64_t)hix);

	return out;
}

