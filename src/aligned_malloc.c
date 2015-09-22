/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* aligned_malloc.c - Aligned memory allocator
*******************************************************************************/

#include <stdint.h>
#include <stdlib.h>

#include "aligned_malloc.h"

/* Some local typedefs */
typedef unsigned char *pointer_t;
typedef unsigned char byte_t;
typedef uintptr_t address_t;
typedef size_t offset_t;

void *aligned_calloc(size_t alignment, size_t size)
{
	/* Local variables */
	pointer_t head = NULL;
	pointer_t origin = NULL;
	pointer_t origin_store = NULL;
	address_t offset = 0;
	size_t total_size = 0;

	/**
	* Checking if the alignment factor is a power of two.  If not, the
	* procedure will return a NULL.
	*/
	if ((alignment & (alignment - 1)) != 0 || alignment == 0) {
		return NULL;
	}

	/**
	* Need to allocate enough storage for the requested bytes, some
	* book-keeping (to store the location returned by malloc) and some
	* extra padding to allow us to find an aligned byte.  I am not entirely
	* sure if 2 time alignment is enough here, its just a guess.
	*/
	total_size = size + (2 * alignment) + sizeof(address_t);

	/* allocate the data */
	head = calloc(total_size, sizeof(byte_t));

	/* If calloc succeeds, then continue inside */
	if (head != NULL) {
		/* Store the original pointer */
		origin = head;

		/* Dedicate enough space from head to store a memory  address */
		head += sizeof(address_t);

		/**
		 * Calculate the number of offsets in order to get an alignment
		 * of the alignment factor.
		 */
		offset = alignment - (((address_t) head) & (alignment - 1));

		/* Hop to aligned pointer */
		head += offset;

		/* Store the original pointer */
		origin_store = (pointer_t) (head - sizeof(address_t));
		*((address_t *) origin_store) = (address_t) origin;
	}

	return head;
}

void aligned_free(void *ptr)
{
	pointer_t p = (pointer_t) ptr;
	pointer_t ptr_to_free = NULL;
	if (p != NULL) {
		p -= sizeof(address_t);
		ptr_to_free = (pointer_t) (*((address_t *) p));
		free(ptr_to_free);
	}
}

static pointer_t get_aligned_pointer_up(pointer_t ptr, size_t alignment);

/****
* Description of memory-map
*
* +----+
* | // | := Padding memory
* +----+
*
* +--> origin: returned by calloc()
* |
* +--+--------+------+------+-----+----+---------+-----+----+---------+------+---------+----+
* |//| origin | v[0] | v[1] | ... | // | v[0][0] | ... | // | v[1][0] | .... | v[m][n] | // |
* +--+--------+------+------+-----+----+---------+-----+----+---------+------+---------+----+
*    |        |                        |                                     |
*    |        |                        |                   Last element <----+
*    |        |                        |
*    |        |                        +---> COL_START: Start of the buffer (type*, aligned)
*    |        |
*    |        |     HEAD: Start of the indirect pointer to buffer (type**)
*    |        +---> This pointer gets returned to caller
*    |              Note: These are sizeof(address_t) aligned, and does
*    |                not need to be aligned by alignment factor.
*    |
*    +---> The original pointer that was retrieved by calling malloc
*/
void *aligned_2d_calloc(size_t alignment, size_t rows, size_t columns,
			size_t element_size)
{
	size_t bytes_needed_row;
	size_t bytes_needed_col;
	size_t bytes_needed_tot;
	size_t c;
	pointer_t head;
	pointer_t origin;
	pointer_t col_ptr;
	pointer_t row_ptr;

	/* Checking for error */
	if ((alignment == 0) || (rows == 0) || (columns == 0)
	    || (element_size == 0)) {
	/**
	 * If any of the input parameter is zero, this function will not work
	 * therefore returning a NULL pointer
	 */
		return NULL;
	}

	/* This won't work if alignment is not a power of two, returning NULL */
	if ((alignment) && ((alignment & (alignment - 1)) != 0)) {
		return NULL;
	}

	/***
	* Calculating total number of bytes needed for a single malloc call
	* Generally a good idea to allocate twice the alignment extra on both ends
	*/
	bytes_needed_row = columns * (rows * ((2 * alignment) + element_size));

	/* Calculating the number of bytes needed to store columns */
	bytes_needed_col = columns * sizeof(pointer_t);

	/* Calculating total bytes (rows + columns + 1 memory spot to store the
	 * original pointer to free)
	 */
	bytes_needed_tot =
	    bytes_needed_row + bytes_needed_col + sizeof(address_t);

	/* Allocating memory */
	head = (pointer_t) calloc(bytes_needed_tot, sizeof(byte_t));

	/* Returning NULL if malloc fails and returns NULL */
	if (head == NULL) {
		return NULL;
	}

	origin = head;		/* Storing the original pointer for free() */


	/* Incrementing pointer to store the address */
	head = get_aligned_pointer_up(head, sizeof(pointer_t));
	*((address_t *) head) = ((address_t) (origin));
	head += sizeof(pointer_t);

	/* Start tabulating columns */
	col_ptr = head;
	row_ptr = head + (columns * sizeof(pointer_t));

	for (c = 0; c < columns; ++c) {
		row_ptr = get_aligned_pointer_up(row_ptr, alignment);
		*((address_t *) col_ptr) = (address_t) row_ptr;
		row_ptr += (rows * element_size);
		col_ptr += sizeof(pointer_t);
	}

	return head;
}

static pointer_t get_aligned_pointer_up(pointer_t ptr, size_t alignment)
{
	address_t a = (address_t) (alignment - 1);
	address_t b = (address_t) (-alignment);
	address_t p = (address_t) (ptr);
	p = (p + a) & b;
	return (pointer_t) p;
}
