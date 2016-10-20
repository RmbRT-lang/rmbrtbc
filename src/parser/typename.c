#include "typename.h"
#include "../malloc.h"
#include "../assert.h"

void rlc_type_indirection_parse(
	struct RlcParserData * parser,
	enum RlcTypeIndirection * out)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	if(rlc_parser_data_consume(
		parser,
		kRlcTokAsterisk))
	{
		*out = kRlcTypeIndirectionPointer;
	} else if(rlc_parser_data_consume(
		parser,
		kRlcTokBackslash))
	{
		*out = kRlcTypeIndirectionNotNull;
	} else
	{
		* out = kRlcTypeIndirectionPlain;
	}
}

void rlc_type_qualifier_parse(
	struct RlcParserData * parser,
	enum RlcTypeQualifier * out)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	if(rlc_parser_data_consume(
		parser,
		kRlcTokConst))
	{
		if(rlc_parser_data_consume(
			parser,
			kRlcTokVolatile))
			*out = kRlcTypeQualifierConst | kRlcTypeQualifierVolatile;
		else
			*out = kRlcTypeQualifierConst;
	} else if(rlc_parser_data_consume(
		parser,
		kRlcTokVolatile))
	{
		if(rlc_parser_data_consume(
			parser,
			kRlcTokConst))
			*out = kRlcTypeQualifierConst | kRlcTypeQualifierVolatile;
		else
			*out = kRlcTypeQualifierVolatile;
	} else
		*out = kRlcTypeQualifierNone;
}

void rlc_parsed_type_name_destroy(
	struct RlcParsedTypeName * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fTypeModifiers)
	{
		rlc_free((void**)&this->fTypeModifiers);
		this->fTypeModifierCount = 0;
	}

	if(!this->fIsVoid)
		rlc_parsed_symbol_destroy(&this->fName);
}


void rlc_parsed_type_name_add_modifier(
	struct RlcParsedTypeName * this,
	struct RlcTypeModifier const * modifier)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(modifier != NULL);

	rlc_realloc(
		(void**)&this->fTypeModifiers,
		sizeof(struct RlcTypeModifier) * ++this->fTypeModifierCount);

	this->fTypeModifiers[this->fTypeModifierCount-1] = *modifier;
}

void rlc_parsed_type_name_create(
	struct RlcParsedTypeName * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_symbol_create(&this->fName);
	this->fIsVoid = 0;

	this->fTypeModifiers = NULL;
	this->fTypeModifierCount = 0;
}

int rlc_parsed_type_name_parse(
	struct RlcParserData * parser,
	struct RlcParsedTypeName * out)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	size_t const start = parser->fIndex;

	if(rlc_parser_data_consume(
		parser,
		kRlcTokVoid))
	{
		out->fIsVoid = 1;
	} else if(!rlc_parsed_symbol_parse(
		parser,
		&out->fName))
	{
		if(parser->fErrorCount)
			rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedSymbol);
		return 0;
	}

	struct RlcTypeModifier modifier;

	do
	{
		rlc_type_modifier_parse(
			parser,
			&modifier);
	} while(modifier.fTypeIndirection != kRlcTypeIndirectionPlain
		|| modifier.fTypeQualifier != kRlcTypeQualifierNone);
}