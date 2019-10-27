#include "file.h"
#include "../assert.h"
#include "../malloc.h"
#include <stdio.h>

int rlc_parsed_file_create(
	struct RlcParsedFile * this,
	char const * filename)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(filename != NULL);

	if(!rlc_src_file_read(&this->fSource, filename))
		return 0;

	struct RlcParser parser;

	this->fIncludes = NULL;
	this->fIncludeCount = 0;
	rlc_parsed_scope_entry_list_create(&this->fScopeEntries);
	rlc_parser_create(&parser, &this->fSource);

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
	while(!rlc_parser_eof(&parser))
	{
		if((entry = rlc_parsed_scope_entry_parse(&parser)))
			rlc_parsed_scope_entry_list_add(
				&this->fScopeEntries,
				entry);
		else
			rlc_parser_fail(&parser, "expected scope entry");
	}

	rlc_parser_destroy(&parser);

	return 1;
}

void rlc_parsed_file_destroy(
	struct RlcParsedFile * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fIncludes)
		rlc_free((void**)&this->fIncludes);
	this->fIncludeCount = 0;

	rlc_parsed_scope_entry_list_destroy(&this->fScopeEntries);
	rlc_src_file_destroy(&this->fSource);
}

char const * rlc_parsed_file_name(
	struct RlcParsedFile const * this)
{
	RLC_DASSERT(this != NULL);

	return this->fSource.fName;
}