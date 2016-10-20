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
	struct RlcParserData * parser,
	struct RlcParsedEnumConstant * out)
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
				rlc_parser_matched_index(parser));
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
	struct RlcParsedEnum * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedEnum);

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
	struct RlcParserData * parser,
	struct RlcParsedEnum * out)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);


	if(!rlc_parser_data_consume(
		parser,
		kRlcTokEnum))
	{
		return 0;
	}

	rlc_parsed_enum_create(out);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier))
	{
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedIdentifier), 0;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceOpen))
	{
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedBraceOpen), 0;
	}

	do {
		struct RlcParsedEnumConstant constant;

		if(!rlc_parsed_enum_constant_parse,
			parser,
			&constant)
		{
			return rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedEnumConstant), 0;
		}
		else
		{
			rlc_parsed_enum_add_constant(
				out,
				&constant);
		}
	} while(rlc_parser_data_consume(
		parser,
		kRlcTokComma));

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceClose))
	{
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedBraceClose), 0;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedSemicolon), 0;
	}

	return 1;
}