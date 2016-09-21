#include "typedef.h"
#include "parser.h"
#include "../assert.h"

int rlc_parsed_typedef_parse(
	struct RlcParserData * parser_data,
	struct RlcParsedTypedef * out)
{
	RLC_DASSERT(parser_data != NULL);
	RLC_DASSERT(out != NULL);

	size_t const start = parser_data->fIndex;

	RLC_BASE_CAST(out, RlcParsedScopeEntry)->fDeclarationIndex = parser_data->fIndex;

	if(!rlc_parser_data_match(
		parser_data,
		kRlcTokTypedef))
		return 0;

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(out, RlcParsedScopeEntry));

	rlc_parser_data_next(
		parser_data);

	if(!rlc_parsed_type_name_parse(
		parser_data,
		&out->fType))
	{
		return rlc_parser_data_add_error(
			parser_data,
			kRlcParseErrorExpectedTypeName), 0;
	}

	do {
		struct RlcToken const * cur;
		if(cur = rlc_parser_data_match(
			parser_data,
			kRlcTokIdentifier))
		{
			rlc_parsed_scope_entry_add_name(
				RLC_BASE_CAST(out, RlcParsedScopeEntry),
				cur->fBegin);
			RLC_BASE_CAST(out, RlcParsedScopeEntry)->fDeclarationIndex = start;
		} else
		{
			return rlc_parser_data_add_error(
				parser_data,
				kRlcParseErrorExpectedIdentifier), 0;
		}
	} while(rlc_parser_data_consume(
		parser_data,
		kRlcTokComma));

	if(rlc_parser_data_consume(
		parser_data,
		kRlcTokSemicolon))
	{
		return 1;
	}
	else
	{
		return rlc_parser_data_add_error(
			parser_data,
			kRlcParseErrorExpectedSemicolon), 0;
	}
}