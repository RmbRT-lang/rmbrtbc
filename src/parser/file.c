#include "file.h"
#include "../assert.h"
#include "../malloc.h"
#include <stdio.h>

static void rlc_parsed_file_report_errors(
	struct RlcParsedFile * this);

int rlc_parsed_file_create(
	struct RlcParsedFile * this,
	struct RlcPreprocessedFile * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	this->fIncludes = NULL;
	this->fIncludeCount = 0;
	rlc_parsed_scope_entry_list_create(&this->fScopeEntries);
	rlc_parser_data_create(&this->fParser, file);

	struct RlcParsedIncludeStatement include;
	while(rlc_parsed_include_statement_parse(
		&include,
		&this->fParser))
	{
		rlc_realloc(
			(void**)&this->fIncludes,
			sizeof(struct RlcParsedIncludeStatement) * ++this->fIncludeCount);
		this->fIncludes[this->fIncludeCount-1] = include;
	}

	if(this->fParser.fErrorCount)
	{
		rlc_parser_data_add_error(
			&this->fParser,
			kRlcParseErrorExpectedIncludeStatement);
		goto failure;
	}

	struct RlcParsedScopeEntry * entry;
	while(rlc_parser_data_current(&this->fParser))
	{
		if(entry = rlc_parsed_scope_entry_parse(&this->fParser))
			rlc_parsed_scope_entry_list_add(
				&this->fScopeEntries,
				entry);
		else
			goto failure;
	}

	return 1;
failure:
	rlc_parsed_file_report_errors(this);
	rlc_parsed_file_destroy(this);
	return 0;
}

void rlc_parsed_file_destroy(
	struct RlcParsedFile * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fIncludes)
		rlc_free((void**)&this->fIncludes);
	this->fIncludeCount = 0;

	rlc_parsed_scope_entry_list_destroy(&this->fScopeEntries);
	rlc_parser_data_destroy(&this->fParser);
}

char const * rlc_parsed_file_name(
	struct RlcParsedFile const * this)
{
	RLC_DASSERT(this != NULL);

	return this->fParser.fFile->fBaseFile->fName;
}

static void end_of_file_position(
	struct RlcPreprocessedFile const * file,
	size_t * line,
	size_t * column);

static void rlc_parsed_file_report_unexpected_token(
	struct RlcParsedFile * this);

static void rlc_parsed_file_report_errors(
	struct RlcParsedFile * this)
{
	printf("%s: %zu parsing errors.\n",
		rlc_parsed_file_name(this),
		this->fParser.fErrorCount);

	rlc_parsed_file_report_unexpected_token(this);

	for(size_t i = 0; i < this->fParser.fErrorCount; i++)
	{
		if(i > 2)
			break;

		size_t line, column;

		struct RlcToken const * token;
		struct RlcToken const * after_latest_token;
		if(this->fParser.fErrors[i].fLocation < this->fParser.fFile->fTokenCount)
			rlc_token_position(
				this->fParser.fFile->fTokens[this->fParser.fErrors[i].fLocation],
				&line,
				&column);
		else
			end_of_file_position(
				this->fParser.fFile,
				&line,
				&column);

		++line, ++column;

		printf("%s:%zu:%zu: error: %s.\n",
			this->fParser.fFile->fBaseFile->fName,
			line,
			column,
			rlc_parse_error_msg(this->fParser.fErrors[i].fError));
	}
}

static void rlc_parsed_file_report_unexpected_token(
	struct RlcParsedFile * this)
{
	size_t line, column;
	struct RlcToken const * token = rlc_parser_data_latest(&this->fParser);
	if(token)
	{
		rlc_token_position(token, &line, &column);
		rlc_char_t * token_content = rlc_token_content(token);
		printf("%s:%zu:%zu: error: unexpected '%"PRINTF_RLC_STR"' (%s).\n",
			this->fParser.fFile->fBaseFile->fName,
			line+1,
			column+1,
			token_content,
			rlc_token_type_name(token->fType));
		rlc_free((void**)&token_content);
	} else
	{
		end_of_file_position(
			this->fParser.fFile,
			&line,
			&column);
		printf("%s:%zu:%zu: error: unexpected end of file.\n",
			this->fParser.fFile->fBaseFile->fName,
			line+1,
			column+1);
	}
}

static void end_of_file_position(
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