#include "file.h"
#include "tokeniser/tokeniser.h"
#include "error.h"
#include "parser/scopeentry.h"
#include <time.h>

#include <stdio.h>

#include "malloc.h"

#define RLC_CASCADE_ERRORS 1

void end_of_file_position(
	struct RlcPreprocessedFile const * file,
	size_t * line,
	size_t * column)
{
	if(file->fTokenCount == 0)
	{
		*line = 0;
		*column = 0;
	} else
	{
		struct RlcToken const * last = file->fTokens[file->fTokenCount-1];
		rlc_token_end(last, line, column);
	}
}

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
				printf("%zu parsing errors.\n", parser.fErrorCount);
				if(!parser.fErrorCount)
				{
					size_t line, column;
					struct RlcToken const * token = rlc_parser_data_latest(&parser);
					if(token)
					{
						rlc_token_position(token, &line, &column);
						rlc_char_t * token_content = rlc_token_content(token);
						printf("%s:%zu:%zu: error: unexpected '%"PRINTF_RLC_STR"' (%s).\n",
							parser.fFile->fBaseFile->fName,
							line+1,
							column+1,
							token_content,
							rlc_token_type_name(token->fType));
						rlc_free((void**)&token_content);
					} else
					{
						end_of_file_position(
							parser.fFile,
							&line,
							&column);
						printf("%s:%zu:%zu: error: unexpected end of file.\n",
							parser.fFile->fBaseFile->fName,
							line+1,
							column+1);
					}
				} else for(size_t i = 0; i<parser.fErrorCount; i++)
				{
					size_t line, column;

					struct RlcToken const * token;
					struct RlcToken const * after_latest_token;
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

					if(i == 0)
					{
						if(parser.fLatestIndex+1 >= parser.fFile->fTokenCount)
						{
							printf("%s:%zu:%zu: error: unexpected end of file: %s.\n",
								parser.fFile->fBaseFile->fName,
								line,
								column,
								rlc_parse_error_msg(parser.fErrors[i].fError));
						} else
						{
							struct RlcToken const * latest = rlc_parser_data_latest(&parser);
							rlc_char_t * next_token_contents = rlc_token_content(latest);
							if(parser.fErrors[i].fLocation != parser.fLatestIndex)
							{
								size_t latest_line, latest_column;
								rlc_token_position(
									latest,
									&latest_line,
									&latest_column);
								++latest_line;
								++latest_column;

								printf("%s:%zu:%zu: error: unexpected '%"PRINTF_RLC_STR"'.\n",
									parser.fFile->fBaseFile->fName,
									latest_line,
									latest_column,
									next_token_contents);
								printf("%s:%zu:%zu: error: %s.\n",
									parser.fFile->fBaseFile->fName,
									line,
									column,
									rlc_parse_error_msg(parser.fErrors[i].fError));
							} else
							{
								printf("%s:%zu:%zu: error: unexpected '%"PRINTF_RLC_STR"': %s.\n",
									parser.fFile->fBaseFile->fName,
									line,
									column,
									next_token_contents,
									rlc_parse_error_msg(parser.fErrors[i].fError));
							}
							rlc_free((void**)&next_token_contents);
						}
					} else {
						printf("%s:%zu:%zu: error: %s.\n",
								parser.fFile->fBaseFile->fName,
								line,
								column,
								rlc_parse_error_msg(parser.fErrors[i].fError));
					}

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