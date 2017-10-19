#include "file.h"
#include "tokeniser/tokeniser.h"
#include "error.h"
#include "parser/scopeentry.h"
#include <time.h>

#include <stdio.h>

#include "malloc.h"

void end_of_file_position(
	struct RlcPreprocessedFile const * file,
	size_t * line,
	size_t * column)
{
	*line = 0;

	size_t last_begin = 0;

	rlc_char_t const * const content = file->fBaseFile->fContents;

	for(size_t i = 0; i < file->fBaseFile->fContentLength; i++)
		if(content[i] == '\n')
		{
			++*line;
			last_begin = i+1;
		}

	*column = file->fBaseFile->fContentLength - last_begin;
}

int compile(
	char const * file)
{


	struct RlcFile tokfile;
	{
		enum RlcFileResult result = rlc_read_text_file(file, &tokfile.fContents);
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


	struct RlcParsedScopeEntryList parsed_list;
	rlc_parsed_scope_entry_list_create(&parsed_list);
	{
		struct RlcPreprocessedFile * pfile = NULL;
		rlc_malloc((void**)&pfile,
			sizeof(*pfile));
		rlc_preprocessed_file_create_from_rlc_file(
			pfile,
			&tokfile);
		struct RlcParserData parser;
		rlc_parser_data_create(
			&parser,
			pfile);

		struct RlcParsedScopeEntry * entry;
		while(rlc_parser_data_current(&parser))
		{
			entry = rlc_parsed_scope_entry_parse(&parser);
			if(!entry)
			{
				//printf("%zu parsing errors.\n", parser.fErrorCount);
				if(!parser.fErrorCount)
				{
					size_t line, column;
					rlc_token_position(rlc_parser_data_current(&parser), &line, &column);
					printf("%s:%zu:%zu: error: unexpected token.\n",
						parser.fFile->fBaseFile->fName,
						line,
						column);
				} else for(size_t i = 0; i<parser.fErrorCount; i++)
				{
					size_t line, column;

					struct RlcToken const * token;
					if(parser.fErrors[i].fLocation < parser.fFile->fTokenCount)
						rlc_token_position(
							parser.fFile->fTokens[parser.fErrors[i].fLocation],
							&line,
							&column);
					else
						end_of_file_position(
							parser.fFile,
							&line,
							&column);

					++line, ++column;

					printf("%s:%zu:%zu: error: %s\n",
						parser.fFile->fBaseFile->fName,
						line,
						column,
						rlc_parse_error_msg(parser.fErrors[i].fError));

#if !RLC_CASCADE_ERRORS
					break;
#endif
				}
				rlc_parsed_scope_entry_list_destroy(&parsed_list);
				rlc_parser_data_destroy(&parser);

				return 0;
			} else
				rlc_parsed_scope_entry_list_add(
					&parsed_list,
					entry);
		}

		rlc_parser_data_destroy(&parser);
	}
	rlc_parsed_scope_entry_list_destroy(&parsed_list);

	puts("success");

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
	compile(argv[1]) ? 0: 1;
	return 0;
}