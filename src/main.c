#include "scoper/fileregistry.h"
#include "unicode.h"
#include "malloc.h"
#include "fs.h"

#include <stdio.h>

int main(
	int argc,
	char ** argv)
{
	struct RlcScopedFileRegistry scoped_registry;
	rlc_scoped_file_registry_create(&scoped_registry);

	int status = 1;
	for(int i = 1; i < argc; i++)
	{
		char const * abs = to_absolute_path(argv[i]);
		if(rlc_scoped_file_registry_get(
			&scoped_registry,
			abs))
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

	rlc_scoped_file_registry_destroy(&scoped_registry);

	size_t allocs;
	if((allocs = rlc_allocations()))
	{
		fprintf(stderr, "Warning: leaked allocations: %zu.\n", allocs);
	}

	fflush(stdout);
	fflush(stderr);
	return status ? 0 : 1;
}