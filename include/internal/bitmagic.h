/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
*
* revbin.h - Contains functions to bit-reverse a binary sequence.
*******************************************************************************/

#ifndef BINA_FFT_INTERNAL_BITMAGIC_H
#define BINA_FFT_INTERNAL_BITMAGIC_H

#include <stdint.h>

uint32_t revbin(uint32_t);
uint64_t revbin64(uint64_t);
int ispowtwo(int);
int ilog2(uint32_t);

#endif /* BINA_FFT_REVBIN_H */

