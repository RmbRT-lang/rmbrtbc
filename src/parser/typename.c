#include "typename.h"
#include "../malloc.h"
#include "../assert.h"

int rlc_type_indirection_parse(
	enum RlcTypeIndirection * out,
	struct RlcParserData * parser,
	int allow_dynamic)
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
	} else if(rlc_parser_data_consume(
		parser,
		kRlcTokAt))
	{
		*out = kRlcTypeIndirectionFuture;
	} else if(rlc_parser_data_match(
		parser,
		kRlcTokTripleDotExclamationMark))
	{
		if(!allow_dynamic)
			goto forbidden_dynamic;

		rlc_parser_data_next(parser);
		*out = kRlcTypeIndirectionExpectDynamic;
	} else if(rlc_parser_data_match(
		parser,
		kRlcTokTripleDot))
	{
		if(!allow_dynamic)
			goto forbidden_dynamic;

		rlc_parser_data_next(parser);
		*out = kRlcTypeIndirectionMaybeDynamic;
	} else
	{
		*out = kRlcTypeIndirectionPlain;
		return 0;
	}
	return 1;

forbidden_dynamic:
	rlc_parser_data_add_error(parser, kRlcParseErrorForbiddenDynamic);
	return 0;
}

int rlc_type_qualifier_parse(
	enum RlcTypeQualifier * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	int ret = 0;

	if(rlc_parser_data_consume(
		parser,
		kRlcTokConst)
	|| rlc_parser_data_consume(
		parser,
		kRlcTokHash))
	{
		if(rlc_parser_data_consume(
			parser,
			kRlcTokVolatile)
		|| rlc_parser_data_consume(
			parser,
			kRlcTokDollar))
			*out = kRlcTypeQualifierConst | kRlcTypeQualifierVolatile;
		else
			*out = kRlcTypeQualifierConst;
		ret = 1;
	} else if(rlc_parser_data_consume(
		parser,
		kRlcTokVolatile)
	|| rlc_parser_data_consume(
		parser,
		kRlcTokDollar))
	{
		if(rlc_parser_data_consume(
			parser,
			kRlcTokConst)
		|| rlc_parser_data_consume(
			parser,
			kRlcTokHash))
			*out = kRlcTypeQualifierConst | kRlcTypeQualifierVolatile;
		else
			*out = kRlcTypeQualifierVolatile;
		ret = 1;
	} else
		*out = kRlcTypeQualifierNone;

	return ret;
}

int rlc_type_modifier_parse(
	struct RlcTypeModifier * out,
	struct RlcParserData * parser,
	int allow_dynamic)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start = parser->fIndex;
	rlc_type_indirection_parse(
		&out->fTypeIndirection,
		parser,
		allow_dynamic);

	if(parser->fErrorCount)
	{
		return 0;
	}

	rlc_type_qualifier_parse(
		&out->fTypeQualifier,
		parser);

	return !parser->fErrorCount;
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

	if(this->fValue == kRlcParsedTypeNameValueName)
	{
		if(this->fName)
		{
			rlc_parsed_symbol_destroy(this->fName);
			rlc_free((void**)&this->fName);
		}
	}
	else if(this->fValue == kRlcParsedTypeNameValueFunction)
	{
		if(this->fFunction)
		{
			rlc_parsed_function_signature_destroy(this->fFunction);
			rlc_free((void**)&this->fFunction);
		}
	} else RLC_DASSERT(this->fValue == kRlcParsedTypeNameValueVoid);
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

	this->fValue = kRlcParsedTypeNameValueVoid;
	this->fName = NULL;

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
	enum RlcParseError error_code;

	rlc_parsed_type_name_create(out);

	out->fValue = kRlcParsedTypeNameValueVoid;

	union {
		struct RlcParsedSymbol fSymbol;
		struct RlcParsedFunctionSignature fFunction;
	} parse;

	if(rlc_parser_data_consume(
		parser,
		kRlcTokVoid))
	{
	} else if(rlc_parsed_symbol_parse(
		&parse.fSymbol,
		parser))
	{
		out->fValue = kRlcParsedTypeNameValueName;
		rlc_malloc((void**)&out->fName, sizeof(struct RlcParsedSymbol));
		*out->fName = parse.fSymbol;
	} else if(parser->fErrorCount)
	{
		error_code = kRlcParseErrorExpectedSymbol;
		goto failure;
	} else if(rlc_parsed_function_signature_parse(
		&parse.fFunction,
		parser))
	{
		out->fValue = kRlcParsedTypeNameValueFunction;
		rlc_malloc((void**)&out->fFunction, sizeof(struct RlcParsedFunctionSignature));
		*out->fFunction = parse.fFunction;
	} else if(parser->fErrorCount)
	{
		error_code = kRlcParseErrorExpectedFunctionSignature;
		goto failure;
	} else
		goto nonfatal_failure;

	struct RlcTypeModifier modifier;

	int allow_dynamic = 1;

	for(;;)
	{
		if(!rlc_type_modifier_parse(
			&modifier,
			parser,
			allow_dynamic))
		{
			error_code = kRlcParseErrorExpectedTypeModifier;
			goto failure;
		}

		switch(modifier.fTypeIndirection)
		{
		case kRlcTypeIndirectionExpectDynamic:
		case kRlcTypeIndirectionMaybeDynamic:
			allow_dynamic = 0;
			break;
		case kRlcTypeIndirectionPlain:
			{ ; } break;
		default:
			allow_dynamic = 1;
			break;
		}

		if(modifier.fTypeIndirection == kRlcTypeIndirectionPlain
		&& modifier.fTypeQualifier == kRlcTypeQualifierNone)
			break;

		rlc_parsed_type_name_add_modifier(
			out,
			&modifier);
	}

	return 1;

failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
nonfatal_failure:
	rlc_parsed_type_name_destroy(out);
	return 0;
}

void rlc_parsed_function_signature_create(
	struct RlcParsedFunctionSignature * this)
{
	RLC_DASSERT(this != NULL);

	this->fArguments = NULL;
	this->fArgumentCount = 0;
	rlc_parsed_type_name_create(&this->fResult);
	this->fIsAsync = this->fIsClosure = 0;
}

void rlc_parsed_function_signature_destroy(
	struct RlcParsedFunctionSignature * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fArguments)
	{
		for(size_t i = this->fArgumentCount; i--;)
			rlc_parsed_type_name_destroy(&this->fArguments[i]);
		rlc_free((void**)&this->fArguments);
		this->fArgumentCount = 0;
	}

	rlc_parsed_type_name_destroy(&this->fResult);
}

void rlc_parsed_function_signature_add_argument(
	struct RlcParsedFunctionSignature * this,
	struct RlcParsedTypeName * argument)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(argument != NULL);

	rlc_realloc(
		(void**)&this->fArguments,
		sizeof(struct RlcParsedTypeName) * ++this->fArgumentCount);
	this->fArguments[this->fArgumentCount-1] = *argument;
}


int rlc_parsed_function_signature_parse(
	struct RlcParsedFunctionSignature * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start = parser->fIndex;

	int is_async, is_closure;

	if(!(is_closure = rlc_parser_data_consume(
		parser,
		kRlcTokAnd)))
		is_closure = is_async = rlc_parser_data_consume(
			parser,
			kRlcTokAt);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseOpen))
	{
		parser->fIndex = start;
		return 0;
	}

	enum RlcParseError error_code = kRlcParseErrorExpectedSymbol;

	rlc_parsed_function_signature_create(out);
	out->fIsAsync = is_async;
	out->fIsClosure = is_closure;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseOpen))
	{
		error_code = kRlcParseErrorExpectedParentheseOpen;
		goto failure;
	}

	if(rlc_parser_data_consume(
		parser,
		kRlcTokParentheseClose))
	{
		;
	// explicit void arguments?
	} else if(rlc_parser_data_match(
		parser,
		kRlcTokVoid)
	&& rlc_parser_data_match_ahead(
		parser,
		kRlcTokParentheseClose))
	{
		rlc_parser_data_next(parser);
		rlc_parser_data_next(parser);
	} else
	{
		// parse argument list.
		struct RlcParsedTypeName name;
		do {
			if(!rlc_parsed_type_name_parse(
				&name,
				parser))
			{
				error_code = kRlcParseErrorExpectedTypeName;
				goto failure;
			}
			rlc_parsed_function_signature_add_argument(
				out,
				&name);
		} while(rlc_parser_data_consume(
			parser,
			kRlcTokComma));

		if(!rlc_parser_data_consume(
			parser,
			kRlcTokParentheseClose))
		{
			error_code = kRlcParseErrorExpectedParentheseClose;
			goto failure;
		}
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokColon))
	{
		error_code = kRlcParseErrorExpectedColon;
		goto failure;
	}

	if(!rlc_parsed_type_name_parse(
		&out->fResult,
		parser))
	{
		error_code = kRlcParseErrorExpectedTypeName;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseClose))
	{
		error_code = kRlcParseErrorExpectedParentheseClose;
		goto failure;
	}

	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_function_signature_destroy(out);
	return 0;
}