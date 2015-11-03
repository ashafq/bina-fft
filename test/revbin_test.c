/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* revbin_test.c - Unit test functions for bit-reversal functions
*******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "internal/check.h"
#include "internal/revbin.h"

static void revbin_test(void);
static void revbin64_test(void);

int main(int argc, char *argv[])
{
	revbin_test();
	revbin64_test();
	return 0;
}

/* Testing revbin - 32 bit */
static void revbin_test(void)
{
	puts("revbin_test");


	CHECK(revbin(0) == 0);
	CHECK(revbin(0x00000001) == 0x80000000);
	CHECK(revbin(0x00000002) == 0x40000000);
	CHECK(revbin(0x00000004) == 0x20000000);
	CHECK(revbin(0x00000008) == 0x10000000);
	CHECK(revbin(0x00000010) == 0x08000000);
	CHECK(revbin(0x00000020) == 0x04000000);
	CHECK(revbin(0x00000040) == 0x02000000);
	CHECK(revbin(0x00000080) == 0x01000000);
	CHECK(revbin(0x00000100) == 0x00800000);
	CHECK(revbin(0x00000200) == 0x00400000);
	CHECK(revbin(0x00000400) == 0x00200000);
	CHECK(revbin(0x00000800) == 0x00100000);
	CHECK(revbin(0x00001000) == 0x00080000);
	CHECK(revbin(0x00002000) == 0x00040000);
	CHECK(revbin(0x00004000) == 0x00020000);
	CHECK(revbin(0x00008000) == 0x00010000);
	CHECK(revbin(0x00010000) == 0x00008000);
	CHECK(revbin(0x00020000) == 0x00004000);
	CHECK(revbin(0x00040000) == 0x00002000);
	CHECK(revbin(0x00080000) == 0x00001000);
	CHECK(revbin(0x00100000) == 0x00000800);
	CHECK(revbin(0x00200000) == 0x00000400);
	CHECK(revbin(0x00400000) == 0x00000200);
	CHECK(revbin(0x00800000) == 0x00000100);
	CHECK(revbin(0x01000000) == 0x00000080);
	CHECK(revbin(0x02000000) == 0x00000040);
	CHECK(revbin(0x04000000) == 0x00000020);
	CHECK(revbin(0x08000000) == 0x00000010);
	CHECK(revbin(0x10000000) == 0x00000008);
	CHECK(revbin(0x20000000) == 0x00000004);
	CHECK(revbin(0x40000000) == 0x00000002);
	CHECK(revbin(0x80000000) == 0x00000001);

	return;
}

/* Testing revbin - 64 bit */
static void revbin64_test(void)
{
	uint64_t in  = UINT64_C(0x0000000000000001);
	uint64_t out = UINT64_C(0x8000000000000000);
	int i = 0;
	int n = sizeof(uint64_t) * CHAR_BIT;

	puts("revbin64_test");

	CHECK(revbin64(0) == 0);

	for(i = 0; i <= n; i++)
	{
		CHECK(revbin64(in) == out);
		in  <<= 1;
		out >>= 1;
	}

	return;
}
