#include "file.h"
#include "tokeniser/tokeniser.h"
#include "error.h"
#include "parser/file.h"
#include <time.h>

#include <stdio.h>

#include "malloc.h"

int compile(
	char const * file)
{
	struct RlcFile tokfile;
	{
		enum RlcFileResult result = rlc_read_text_file(file, &tokfile.fContents);
		if(result != kRlcFileSuccess)
			printf(
				"%s: %s\n",
				file ? "/dev/stdin" : file,
				rlc_file_result_message(result));
	}
	tokfile.fContentLength = rlc_strlen(tokfile.fContents);

	tokfile.fName = file ? file : "/dev/stdin";
	size_t error_index;

	{
		enum RlcTokResult result = rlc_tokenise(
			tokfile.fContents,
			&tokfile,
			1,
			1,
			&error_index);

		if(result != kRlcTokResultOk)
			return 0;
	}

	struct RlcParsedFile * parsed_file = NULL;
	rlc_malloc((void**)&parsed_file, sizeof(struct RlcParsedFile));
	{
		struct RlcPreprocessedFile * pfile = NULL;
		rlc_malloc((void**)&pfile,
			sizeof(*pfile));
		rlc_preprocessed_file_create_from_rlc_file(
			pfile,
			&tokfile);

		if(!rlc_parsed_file_create(parsed_file, pfile))
		{
			rlc_free((void**)&parsed_file);
			rlc_file_destroy(&tokfile);
			return 0;
		}
	}

	rlc_parsed_file_destroy(parsed_file);
	rlc_free((void**)&parsed_file);
	rlc_file_destroy(&tokfile);

	fputs("success\n", stderr);
	return 1;
}
#include "unicode.h"
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
	int status = compile(argv[1]);

	size_t allocs;
	if(allocs = rlc_allocations())
	{
		fprintf(stderr, "Warning: leaked allocations: %zu.\n", allocs);
	}

	return status ? 0 : 1;
}