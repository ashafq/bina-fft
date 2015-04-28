/********************************************************************************************

	oooooooooo.   o8o                        oooooooooooo oooooooooooo  ooooooooooooo 
	`888'   `Y8b  `"'                        `888'     `8 `888'     `8  8'   888   `8 
	 888     888 oooo  ooo. .oo.    .oooo.    888          888               888      
	 888oooo888' `888  `888P"Y88b  `P  )88b   888oooo8     888oooo8          888      
	 888    `88b  888   888   888   .oP"888   888    "     888    "          888      
	 888    .88P  888   888   888  d8(  888   888          888               888      
	o888bood8P'  o888o o888o o888o `Y888""8o o888o        o888o             o888o
                                                               
	Bina FFT: An AVX Intrinsics Based Radix-2 FFT Routine
	Copyright (C) 2012 - Ayan Shafqat, All rights reserved.


	The MIT License:
	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
	OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


	 /\_/\    <~~~~~~~~~~~~~~~>
	( o.o ) <      ENJOY!     |
	 > ^ <    ^~~~~~~~~~~~~~~~^

********************************************************************************************/
#include <stdlib.h>
#include "bina_types.h"
#ifndef _BINA_MALLOC_H_
#define _BINA_MALLOC_H_

// Aligned malloc function
#ifndef bina_aligned_malloc
static void* bina_aligned_malloc (size_t bytes, size_t alignment)
{
	void *p1 ,*p2; // basic pointer needed for computation.
	if((p1 =(void *) malloc(bytes + alignment + sizeof(size_t)))==NULL)
	    return NULL;
	size_t addr=(size_t)p1+alignment+sizeof(size_t);
	p2=(void *)(addr - (addr%alignment));
	*((size_t *)p2-1)=(size_t)p1;
	return p2;
}
#endif // bina_aligned_malloc

#ifndef bina_aligned_free
static void bina_aligned_free (void *p)
{
    /* Find the address stored by aligned_malloc ,"size_t" bytes above the
     current pointer then free it using normal free routine provided by C. */
    free((void *)(*((size_t *) p-1)));
}
#endif // bina_aligned_free

// Useless code :3
// static bina_complex* bina_complex_malloc(size_t num_elements)
// {
// 	#if defined (__AVX__)
// 	#define _ALIGNMENT_FACTOR 32
// 	#else 
// 	#define _ALIGNMENT_FACTOR 16
// 	#endif
// 	static bina_complex* array_of_complex 
// 		= (bina_complex*)_aligned_malloc(num_elements*sizeof(bina_complex), _ALIGNMENT_FACTOR);
// 	#undef _ALIGNMENT_FACTOR
// 	return array_of_complex;
// }
// static void bina_complex_free(bina_complex* ptr)
// {
// 	_aligned_free(ptr);
// }

#endif /* _BINA_MALLOC_H_ */