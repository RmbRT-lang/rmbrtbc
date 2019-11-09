#include "blockstatement.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_block_statement_create(
	struct RlcParsedBlockStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedBlockStatement);

	rlc_parsed_statement_list_create(&this->fList);
}

void rlc_parsed_block_statement_destroy(
	struct RlcParsedBlockStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));

	rlc_parsed_statement_list_destroy(&this->fList);
}

int rlc_parsed_block_statement_parse(
	struct RlcParsedBlockStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBraceOpen))
		return 0;

	rlc_parsed_block_statement_create(out);

	// A block statement is either a single empty statement, or other statements.
	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokSemicolon))
	{
		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokBraceClose);

		return 1;
	}

	struct RlcParsedStatement * stmt;
	while(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBraceClose))
	{
		if(!(stmt = rlc_parsed_statement_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedStatementType))))
			rlc_parser_fail(parser, "expected statement");
		rlc_parsed_statement_list_add(
			&out->fList,
			stmt);
	}

	return 1;
}