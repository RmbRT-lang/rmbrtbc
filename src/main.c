#include "resolver/fileregistry.h"
#include "unicode.h"
#include "malloc.h"

#include <stdio.h>

int main(
	int argc,
	char ** argv)
{
	struct RlcResolvedFileRegistry resolved_registry;
	rlc_resolved_file_registry_create(&resolved_registry);

	int status = 1;
	for(int i = 1; i < argc; i++)
	{
		if(rlc_resolved_file_registry_get(
			&resolved_registry,
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

	rlc_resolved_file_registry_destroy(&resolved_registry);

	size_t allocs;
	if((allocs = rlc_allocations()))
	{
		fprintf(stderr, "Warning: leaked allocations: %zu.\n", allocs);
	}

	fflush(stdout);
	fflush(stderr);
	return status ? 0 : 1;
}