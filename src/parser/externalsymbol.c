#include "externalsymbol.h"
#include "parser.h"
#include "../assert.h"

void rlc_parsed_external_symbol_create(
	struct RlcParsedExternalSymbol * this,
	size_t start)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedExternalSymbol,
		start);

	this->fHasCustomLinkName = 0;
	rlc_parsed_type_name_create(&this->fType);
}

void rlc_parsed_external_symbol_destroy(
	struct RlcParsedExternalSymbol * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	rlc_parsed_type_name_destroy(&this->fType);
}

int rlc_parsed_external_symbol_parse(
	struct RlcParsedExternalSymbol * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start = parser->fIndex;
	enum RlcParseError error_code;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokExtern))
	{
		return 0;
	}


	rlc_parsed_external_symbol_create(
		out,
		start);

	if((out->fHasCustomLinkName = 0 != rlc_parser_data_consume(
		parser,
		kRlcTokBracketOpen)))
	{
		if(!rlc_parser_data_consume(
			parser,
			kRlcTokString))
		{
			error_code = kRlcParseErrorExpectedStringExpression;
			goto failure;
		}

		out->fCustomLinkName = rlc_parser_data_consumed_index(parser);

		if(!rlc_parser_data_consume(
			parser,
			kRlcTokBracketClose))
		{
			error_code = kRlcParseErrorExpectedBracketClose;
			goto failure;
		}
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier))
	{
		error_code = kRlcParseErrorExpectedIdentifier;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokColon))
	{
		error_code = kRlcParseErrorExpectedColon;
		goto failure;
	}

	if(!rlc_parsed_type_name_parse(
		&out->fType,
		parser))
	{
		error_code = kRlcParseErrorExpectedTypeName;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		error_code = kRlcParseErrorExpectedSemicolon;
		goto failure;
	}

	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	parser->fIndex = start;
	rlc_parsed_external_symbol_destroy(out);
	return 0;
}