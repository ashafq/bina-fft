/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
*
* revbin.c - Contains function implementation of bit-reversing a binary
* sequence
*******************************************************************************/
#include "internal/revbin.h"

#ifdef __cplusplus
#error This file needs to be compiled with a C compiler
#endif

/* A helper macro to make code more readable. This really hurts my eyes. */
#define SWAP_BITS(b, m, n) \
	((((b) >> (n)) & (m)) | (((b) & (m)) << (n)))

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
#	ifdef __arm__

	asm("rbit %1, %0": "=r"(x):"r"(x));
	return x;

#	else /* portable version */

	const uint32_t BIT    = UINT32_C(0x55555555);
	const uint32_t PAIR   = UINT32_C(0x33333333);
	const uint32_t NIBBLE = UINT32_C(0x0f0f0f0f);
	const uint32_t BYTE   = UINT32_C(0x00ff00ff);

	x = SWAP_BITS(x, BIT,     1);
	x = SWAP_BITS(x, PAIR,    2);
	x = SWAP_BITS(x, NIBBLE,  4);
	x = SWAP_BITS(x, BYTE,    8);
	x = (x >> 16) | (x << 16);

	return x;

#	endif
}


/* Same as revbin(), but with 64 bit support.
 * @x The parameter to be reversed
 * @return Reversed bit order of x
 */
uint64_t revbin64(uint64_t x)
{
	const uint64_t BIT     = UINT64_C(0x5555555555555555);
	const uint64_t PAIR    = UINT64_C(0x3333333333333333);
	const uint64_t NIBBLE  = UINT64_C(0x0f0f0f0f0f0f0f0f);
	const uint64_t BYTE    = UINT64_C(0x00ff00ff00ff00ff);
	const uint64_t SWORD   = UINT64_C(0x0000ffff0000ffff);

	x = SWAP_BITS(x, BIT,        1);
	x = SWAP_BITS(x, PAIR,       2);
	x = SWAP_BITS(x, NIBBLE,     4);
	x = SWAP_BITS(x, BYTE,       8);
	x = SWAP_BITS(x, SWORD,     16);
	x = (x >> 32) | (x << 32);

	return x;
}

