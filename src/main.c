#include "parser/fileregistry.h"
#include "unicode.h"
#include "malloc.h"

#include <stdio.h>

int main(
	int argc,
	char ** argv)
{
	struct RlcParsedFileRegistry parsed_registry;
	rlc_parsed_file_registry_create(&parsed_registry);

	int status = 1;
	for(int i = 1; i < argc; i++)
	{
		if(rlc_parsed_file_registry_get(
			&parsed_registry,
			argv[i]))
		{
			printf("parsed file %s\n",
				argv[i]);
		} else
		{
			fprintf(
				stderr, "%s not found\n",
				argv[i]);
			status = 0;
		}
	}

	rlc_parsed_file_registry_destroy(&parsed_registry);

	size_t allocs;
	if((allocs = rlc_allocations()))
	{
		fprintf(stderr, "Warning: leaked allocations: %zu.\n", allocs);
	}

	fflush(stdout);
	fflush(stderr);
	return status ? 0 : 1;
}