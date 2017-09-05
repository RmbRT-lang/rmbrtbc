#include "namespace.h"

#include "../assert.h"

void rlc_parsed_namespace_create(
	struct RlcParsedNamespace * this,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedNamespace,
		start_index);
	rlc_parsed_scope_entry_list_create(&this->fEntryList);
}

void rlc_parsed_namespace_destroy(
	struct RlcParsedNamespace * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_list_destroy(&this->fEntryList);
	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));
}

int rlc_parsed_namespace_parse(
	struct RlcParsedNamespace * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start_index = parser->fIndex;
	enum RlcParseError error_code;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokNamespace))
		return 0;

	rlc_parsed_namespace_create(
		out,
		start_index);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier))
	{
		error_code = kRlcParseErrorExpectedIdentifier;
		goto failure;
	}

	rlc_parsed_scope_entry_add_name(
		RLC_BASE_CAST(out, RlcParsedScopeEntry),
		rlc_parser_data_consumed_index(parser));

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokColonEqual))
	{
		error_code = kRlcParseErrorExpectedColonEqual;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceOpen))
	{
		error_code = kRlcParseErrorExpectedBraceOpen;
		goto failure;
	}

	for(;;)
	{
		struct RlcParsedScopeEntry * scopeEntry;
		if(scopeEntry = rlc_parsed_scope_entry_parse(
			parser))
		{
			rlc_parsed_scope_entry_list_add(
				&out->fEntryList,
				scopeEntry);
		}
		else if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedScopeEntry;
			goto failure;
		}
		else break;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceClose))
	{
		error_code = kRlcParseErrorExpectedBraceClose;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		error_code = kRlcParseErrorExpectedSemicolon;
		goto failure;
	}

success:
	RLC_BASE_CAST(out, RlcParsedScopeEntry)->fLocation.fEnd = parser->fIndex;
	return 1;
failure:
	rlc_parsed_namespace_destroy(out);
	rlc_parser_data_add_error(
		parser,
		error_code);
	parser->fIndex = start_index;
	return 0;
}