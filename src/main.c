#include "parser/fileregistry.h"
#include "error.h"
#include "unicode.h"
#include "malloc.h"

#include <stdio.h>

void _rlc_report_lexical_error(
	char const * file,
	size_t line,
	size_t column,
	rlc_char_t const * line_string,
	enum RlcTokResult cause)
{
	printf("%s:%zu:%zu: error: %s\n"
		"%" PRINTF_RLC_STR "\n",
		file,
		line,
		column,
		rlc_tok_result_message(cause),
		line_string);

	for(int i = 0; i < column-1; i++)
	{
		putc(line_string[i] == '\t' ? '\t': ' ', stdout);
	}
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
	for(size_t i = 1; i < argc; i++)
	{
		int error;
		if(rlc_parsed_file_registry_get(
			&parsed_registry,
			argv[i],
			&error))
		{
			printf("parsed file %s\n",
				argv[i]);
		} else
		{
			fprintf(
				stderr, "could not parse file %s (error: %s)\n",
				argv[i],
				error ? "true" : "false");
			status = 0;
		}
	}

	rlc_parsed_file_registry_destroy(&parsed_registry);

	size_t allocs;
	if(allocs = rlc_allocations())
	{
		fprintf(stderr, "Warning: leaked allocations: %zu.\n", allocs);
	}

	return status ? 0 : 1;
}