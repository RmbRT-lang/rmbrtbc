#include "parser/fileregistry.h"
#include "error.h"
#include "unicode.h"
#include "malloc.h"

#include <stdio.h>

void _rlc_report_lexical_error(
	struct RlcSrcFile const * file,
	struct RlcSrcPosition position,
	struct RlcSrcString line_string,
	char const * cause)
{
	char const * line = rlc_src_string_cstr(&line_string, file);
	printf("%s:%u:%u: error: %s\n"
		"%s\n",
		file->fName,
		position.line, position.column,
		cause,
		line);

	for(unsigned i = 0; i < position.column-1; i++)
		putc(line[i] == '\t' ? '\t': ' ', stdout);
	puts("^");
}

int main(
	int argc,
	char ** argv)
{
	rlc_register_lexical_error_function(_rlc_report_lexical_error);
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

	return status ? 0 : 1;
}