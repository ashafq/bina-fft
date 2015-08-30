/*******************************************************************************
* bina-fft: A simple FFT implementation for education purpose
* Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
*
* check.h - Contains a simple unit-testing framework.
*******************************************************************************/

#include <stdio.h>

#ifndef BINA_FFT_CHECK_H
#define BINA_FFT_CHECK_H

#if defined(__linux__) || defined(__unix)
#include <unistd.h>
#define HAS_FD (1)
#else
/* no color terminal support */
#endif

#define CHECK(expression) unit_test_checker(#expression, (expression))

static void unit_test_checker(const char *expr, int eval)
{
	char *pass_str;
	char *fail_str;
	char *long_dash = "---------------";

#	ifdef HAS_FD /* I really should have something better here.
			Unfortunately I don't. :'( */

	/* Check if stdout (file descriptor 1) is a going into a terminal.
	 * If so, then display pretty colors, else just use plain text.
	 */
	if (isatty(1)) {
		pass_str = "[\e[1;32mPASS\e[0m]";
		fail_str = "[\e[1;31mFAIL\e[0m]";
	} else {
#	endif
		pass_str = "[PASS]";
		fail_str = "[FAIL]";
#	ifdef HAS_FD
	}
#	endif

	if(!eval) fprintf(stdout, "%s", fail_str);
	else fprintf(stdout, "%s", pass_str);

	fprintf(stdout, " %s %s\n", long_dash, expr);

	return;
}

#endif /* BINA_FFT_CHECK_H */

