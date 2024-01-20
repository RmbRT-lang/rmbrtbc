#include "assert.h"

#include <stdio.h>
#include <stdlib.h>

void rlc_assertion_failure(
	char const * expression,
	char const * file,
	int line,
	char const * function)
{
	fprintf(stderr, "%s:%i: assertion failure: \"%s\" in (%s).\n", file, line, expression, function);
	fflush(stdout);
	fflush(stderr);
	abort();
}