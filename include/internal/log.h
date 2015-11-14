/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* log.h - Simple functions/macros to log and debug.
*******************************************************************************/

#ifndef BINA_FFT_INTERNAL_LOG_H
#define BINA_FFT_INTERNAL_LOG_H

#include <stdio.h>

/* TODO: Make this better! */
#define log_error(...) fprintf(stderr, __VA_ARGS__)

#define pdebug(...) printf(__VA_ARGS__)

#endif /* BINA_FFT_INTERNAL_LOG_H */

