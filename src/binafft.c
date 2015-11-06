/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* binafft.c - main API implementation -- IN PROGRESS
*******************************************************************************/

#include "binafft.h"
#include "internal/bina_transform.h"

int bina_transform_execute(bina_transform base)
{
	struct bina_transform *self = base;
	if (!self->execute) {
		return -1;
	}
	return self->execute(base);
}

int bina_transform_free(bina_transform base)
{
	struct bina_transform *self = base;
	if (!self->free) {
		return -1;
	}
	return self->free(base);
}
