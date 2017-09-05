#include "typename.h"
#include "../malloc.h"
#include "../assert.h"

int rlc_type_indirection_parse(
	enum RlcTypeIndirection * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	if(rlc_parser_data_consume(
		parser,
		kRlcTokAsterisk))
	{
		*out = kRlcTypeIndirectionPointer;
		return 1;
	} else if(rlc_parser_data_consume(
		parser,
		kRlcTokBackslash))
	{
		*out = kRlcTypeIndirectionNotNull;
		return 1;
	} else
	{
		* out = kRlcTypeIndirectionPlain;
		return 0;
	}
}

int rlc_type_qualifier_parse(
	enum RlcTypeQualifier * out,
	struct RlcParserData * parser)
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
		return 1;
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
		return 1;
	} else
		*out = kRlcTypeQualifierNone;
	return 0;
}

void rlc_type_modifier_parse(
	struct RlcTypeModifier * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_type_indirection_parse(
		&out->fTypeIndirection,
		parser);

	rlc_type_qualifier_parse(
		&out->fTypeQualifier,
		parser);
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
	struct RlcParsedTypeName * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	size_t const start = parser->fIndex;

	rlc_parsed_type_name_create(out);

	if(rlc_parser_data_consume(
		parser,
		kRlcTokVoid))
	{
		out->fIsVoid = 1;
	} else if(!rlc_parsed_symbol_parse(
		&out->fName,
		parser))
	{
		if(parser->fErrorCount)
			rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedSymbol);
		rlc_parsed_type_name_destroy(out);
		return 0;
	}

	struct RlcTypeModifier modifier;

	int qualifier = 0;

	for(;;)
	{
		if(rlc_type_indirection_parse(
			&modifier.fTypeIndirection,
			parser))
		{
			qualifier = 0;
		}

		if(!qualifier)
		{
			qualifier = rlc_type_qualifier_parse(
				&modifier.fTypeQualifier,
				parser);
		} else break;

		if(modifier.fTypeIndirection != kRlcTypeIndirectionPlain || qualifier)
			rlc_parsed_type_name_add_modifier(
				out,
				&modifier);
		else
			break;
	}

	return 1;
}