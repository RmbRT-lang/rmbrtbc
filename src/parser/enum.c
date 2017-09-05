#include "enum.h"

#include "../tokeniser/tokens.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_enum_constant_create(
	struct RlcParsedEnumConstant * this)
{
	RLC_DASSERT(this != NULL);

	this->fNameTokens = NULL;
	this->fNameCount = 0;
}

void rlc_parsed_enum_constant_add_name(
	struct RlcParsedEnumConstant * this,
	size_t nameToken)
{
	RLC_DASSERT(this != NULL);

	rlc_realloc(
		(void**)&this->fNameTokens,
		sizeof(size_t) * ++this->fNameCount);

	this->fNameTokens[this->fNameCount-1] = nameToken;
}

void rlc_parsed_enum_constant_destroy(
	struct RlcParsedEnumConstant * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fNameTokens)
	{
		rlc_free((void**)&this->fNameTokens);
		this->fNameCount = 0;
	}
}

int rlc_parsed_enum_constant_parse(
	struct RlcParsedEnumConstant * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	rlc_parsed_enum_constant_create(out);

	do {
		if(rlc_parser_data_consume(
			parser,
			kRlcTokIdentifier))
		{
			rlc_parsed_enum_constant_add_name(
				out,
				rlc_parser_data_consumed_index(parser));
		} else
			return rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedIdentifier), 0;
	} while(rlc_parser_data_consume(
		parser,
		kRlcTokColonEqual));

	return 1;
}

void rlc_parsed_enum_create(
	struct RlcParsedEnum * this,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedEnum,
		start_index);

	this->fConstants = NULL;
	this->fConstantCount = 0;
}

void rlc_parsed_enum_destroy(
	struct RlcParsedEnum * this)
{
	RLC_DASSERT(this != NULL);

	for(int i = 0; i < this->fConstantCount; i++)
		rlc_parsed_enum_constant_destroy(
			&this->fConstants[i]);

	if(this->fConstants)
	{
		rlc_free((void**)&this->fConstants);
		this->fConstantCount = 0;
	}
}

void rlc_parsed_enum_add_constant(
	struct RlcParsedEnum * this,
	struct RlcParsedEnumConstant const * constant)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(constant != NULL);

	rlc_realloc(
		(void**)&this->fConstants,
		sizeof(struct RlcParsedEnumConstant) * ++this->fConstantCount);
	this->fConstants[this->fConstantCount-1] = *constant;
}

int rlc_parsed_enum_parse(
	struct RlcParsedEnum * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	size_t const start_index = parser->fIndex;
	enum RlcParseError error_code;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokEnum))
	{
		return 0;
	}

	rlc_parsed_enum_create(
		out,
		start_index);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier))
	{
		error_code = kRlcParseErrorExpectedIdentifier;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceOpen))
	{
		error_code = kRlcParseErrorExpectedBraceOpen;
		goto failure;
	}

	do {
		struct RlcParsedEnumConstant constant;

		if(!rlc_parsed_enum_constant_parse(
			&constant,
			parser))
		{
			error_code = kRlcParseErrorExpectedEnumConstant;
			goto failure;
		}

		rlc_parsed_enum_add_constant(
			out,
			&constant);

	} while(rlc_parser_data_consume(
		parser,
		kRlcTokComma));

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
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_enum_destroy(out);
	parser->fIndex = start_index;
	return 0;
}