#include "malloc.h"
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"

static size_t s_rlc_allocations = 0;

void rlc_malloc(
	void ** ptr,
	size_t size)
{
	RLC_DASSERT(ptr);
	RLC_DASSERT(!*ptr);

	*ptr = malloc(size);
	if(!*ptr)
	{
		fputs("Ran out of memory.", stderr);
		exit(EXIT_FAILURE);
	}

	s_rlc_allocations++;
}

void rlc_realloc(
	void ** ptr,
	size_t newsz)
{
	RLC_DASSERT(ptr);

	if(!*ptr)
		rlc_malloc(ptr, newsz);
	else
	{
		*ptr = realloc(*ptr, newsz);
		if(!*ptr)
		{
			fputs("Ran out of memory.", stderr);
			exit(EXIT_FAILURE);
		}
	}
}

void rlc_free(
	void ** ptr)
{
	RLC_DASSERT(ptr);
	RLC_DASSERT(*ptr);

	RLC_DASSERT(s_rlc_allocations && "Memory was redundantly freed.");

	free(*ptr);
	*ptr = NULL;
	
	s_rlc_allocations--;
}

size_t rlc_allocations()
{
	return s_rlc_allocations;
}