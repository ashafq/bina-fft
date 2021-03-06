#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "internal/check.h"
#include "internal/aligned_malloc.h"

static int is_aligned(void *ptr, size_t alignment);
#define ALIGNMENT (16)

int main()
{
	size_t size = rand() & 0xFFFF;
	size_t rows = rand() & 0xFFFF;
	size_t colums = rand() & 0xFF;
	size_t k = 0;
	char *a = aligned_calloc(ALIGNMENT, size, sizeof(char));
	float **b = aligned_2d_calloc(ALIGNMENT, rows, colums, sizeof(float));

	/* Testing `a' */
	assert(a != NULL);
	memset(a, 0xffU, size * sizeof(char));
	CHECK(is_aligned(a, ALIGNMENT));

	/* Testing `b' */
	assert(b != NULL);
	for (k = 0; k < colums; k++) {
		memset(b[k], 0xffu, size * sizeof(float));
		CHECK(is_aligned(b[k], ALIGNMENT));
	}

	aligned_free(a);
	aligned_free(b);
	return 0;
}

static int is_aligned(void *ptr, size_t alignment)
{
	uintptr_t address = (uintptr_t) ptr;
	return ((address % alignment) == 0);
}
