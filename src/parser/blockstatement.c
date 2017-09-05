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
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);


	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceOpen))
		return 0;

	enum RlcParseError error_code;

	rlc_parsed_block_statement_create(out);

	while(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceClose))
	{
		struct RlcParsedStatement * statement =
			rlc_parsed_statement_parse(
				parser,
				RLC_ALL_FLAGS(RlcParsedStatementType));

		if(!statement)
		{
			error_code = kRlcParseErrorExpectedStatement;
			goto failure;
		}

		rlc_parsed_statement_list_add(
			&out->fList,
			statement);
	}

success:
	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_block_statement_destroy(out);
	return 0;
}