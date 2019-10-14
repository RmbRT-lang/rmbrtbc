#include "file.h"
#include "../assert.h"
#include "../malloc.h"
#include <stdio.h>

static void rlc_parsed_file_report_errors(
	struct RlcParsedFile * this);

int rlc_parsed_file_create(
	struct RlcParsedFile * this,
	char const * filename)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(filename != NULL);

	this->fIncludes = NULL;
	this->fIncludeCount = 0;
	rlc_parsed_scope_entry_list_create(&this->fScopeEntries);
	struct RlcParser parser;
	struct RlcSrcFile srcfile;
	rlc_src_file_read(&srcfile, filename);
	rlc_parser_create(&parser, &srcfile);

	struct RlcParsedIncludeStatement include;
	while(rlc_parsed_include_statement_parse(
		&include,
		&parser))
	{
		rlc_realloc(
			(void**)&this->fIncludes,
			sizeof(struct RlcParsedIncludeStatement) * ++this->fIncludeCount);
		this->fIncludes[this->fIncludeCount-1] = include;
	}

	struct RlcParsedScopeEntry * entry;
	while(!rlc_parser_eof(&this->fParser))
	{
		if(entry = rlc_parsed_scope_entry_parse(&this->fParser))
			rlc_parsed_scope_entry_list_add(
				&this->fScopeEntries,
				entry);
		else
			goto failbure;
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