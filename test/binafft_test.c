#include "stdlib.h"
#include "complex.h"
#include "binafft.h"

int main()
{
	bina_complex *in = NULL;
	bina_complex *out = NULL;
	bina_transform transform = bina_transform_create_radix2_c2c_fft(in, out, 0, 0);
	bina_transform_execute(transform);
	bina_transform_free(transform);
	return 0;
}

