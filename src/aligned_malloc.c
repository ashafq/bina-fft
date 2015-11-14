/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
* See LICENSE for more information.
*
* aligned_malloc.c - Aligned memory allocator
*******************************************************************************/

#include <stdint.h>
#include <stdlib.h>

#include "internal/aligned_malloc.h"

/* Some typedefs */
typedef unsigned char *pointer_t;
typedef unsigned char byte_t;
typedef uintptr_t address_t;
typedef size_t offset_t;

/* Allocates an aligned segment of memory of `elements' numbers of `size'
 * chunks.
 *
 * Note: The pointer that is returned can only be freed using `aligned_free()'
 * since this stored the original pointer before the returned aligned pointer.
 */
void *aligned_calloc(size_t alignment, size_t elements, size_t size)
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

	/* Size is the number of elements and each element size */
	size = size * elements;

	/**
	* Need to allocate enough storage for the requested bytes, some
	* book-keeping (to store the location returned by malloc) and some
	* extra padding to allow us to find an aligned byte. I am not entirely
	* sure if 2 time alignment is enough here, its just an asumption
	* that has worked for me, I guess.
	*/
	total_size = size + (2 * alignment) + sizeof(address_t);

	/* allocate the data */
	head = calloc(total_size, sizeof(byte_t));

	/* If calloc fails, then return NULL */
	if (head == NULL) {
		return NULL;
	}

	/* Store the original pointer */
	origin = head;

	/* Dedicate enough space from head to store a memory address */
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

	return head;
}

/*
 * Helper function needed needed by aligned_2d_calloc()
 */
static pointer_t get_aligned_pointer_up(pointer_t ptr, size_t alignment)
{
	address_t a = (address_t) (alignment - 1);
	address_t b = (address_t) (-alignment);
	address_t p = (address_t) (ptr);
	p = (p + a) & b;
	return (pointer_t) p;
}

/*
 * Allocates a two dimentional array of specific alignment.
 *
 * Description of memory-map: ASCII art warning
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
	size_t bytes_needed_row = 0;
	size_t bytes_needed_col = 0;
	size_t bytes_needed_tot = 0;
	pointer_t head = NULL;
	pointer_t origin = NULL;
	pointer_t col_ptr = NULL;
	pointer_t row_ptr = NULL;

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

	for (size_t c = 0; c < columns; ++c) {
		row_ptr = get_aligned_pointer_up(row_ptr, alignment);
		*((address_t *) col_ptr) = (address_t) row_ptr;
		row_ptr += (rows * element_size);
		col_ptr += sizeof(pointer_t);
	}

	return head;
}

/*
 * Frees up the aligned memory allocated by aligned_calloc(), and
 * aligned_2d_calloc().
 */
void aligned_free(void *ptr)
{
	if (ptr == NULL) {
		return; /* Don't do anything if ptr points to nothing */
	}

	pointer_t p = (pointer_t) ptr;
	pointer_t ptr_to_free = NULL;

	/* Grab the original pointer */
	p -= sizeof(address_t);
	ptr_to_free = (pointer_t) (*((address_t *) p));

	free(ptr_to_free);
}

