#include <stddef.h>

#ifndef BINA_FFT_ALIGNED_MALLOC_H
#define BINA_FFT_ALIGNED_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

	void *aligned_calloc(size_t alignment, size_t size);
	void *aligned_2d_calloc(size_t alignment, size_t rows, size_t columns,
				size_t element_size);
	void aligned_free(void *ptr);

#ifdef __cplusplus
}				/* extern "C" */
#endif
#endif				/* BINA_FFT_ALIGNED_MALLOC_H */
