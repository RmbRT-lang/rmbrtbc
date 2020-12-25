#include "typename.h"
#include "expression.h"
#include "symbolconstantexpression.h"
#include "../malloc.h"
#include "../assert.h"
#include "../printer.h"

int rlc_type_indirection_parse(
	enum RlcTypeIndirection * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokAsterisk))
	{
		*out = kRlcTypeIndirectionPointer;
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBackslash))
	{
		*out = kRlcTypeIndirectionNotNull;
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokAt))
	{
		*out = kRlcTypeIndirectionFuture;
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokDoubleDotExclamationMark))
	{
		*out = kRlcTypeIndirectionExpectDynamic;
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokDoubleDotQuestionMark))
	{
		*out = kRlcTypeIndirectionMaybeDynamic;
	} else
	{
		*out = kRlcTypeIndirectionPlain;
		return 0;
	}
	return 1;
}

int rlc_type_qualifier_parse(
	enum RlcTypeQualifier * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	int ret = 0;

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokHash))
	{
		if(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokDollar))
			*out = kRlcTypeQualifierConst | kRlcTypeQualifierVolatile;
		else
			*out = kRlcTypeQualifierConst;
		ret = 1;
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokDollar))
	{
		if(rlc_parser_consume(
			parser,
			NULL,
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
	struct RlcParser * parser,
	int expect_indirection)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	int indirection = rlc_type_indirection_parse(
			&out->fTypeIndirection,
			parser);

	if(!indirection && expect_indirection)
		return 0;

	int qualifier = rlc_type_qualifier_parse(
			&out->fTypeQualifier,
			parser);

	if((out->fIsArray = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBracketOpen)))
	{
		out->fArraySize = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType));
		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokBracketClose);
	} else
		out->fArraySize = NULL;

	return indirection || qualifier || out->fArraySize;
}

void rlc_type_modifier_destroy(
	struct RlcTypeModifier * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fArraySize)
	{
		rlc_parsed_expression_destroy_virtual(this->fArraySize);
		rlc_free((void**)&this->fArraySize);
	}
}

void rlc_parsed_type_name_destroy(
	struct RlcParsedTypeName * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fTypeModifiers)
	{
		for(RlcSrcIndex i = 0; i < this->fTypeModifierCount; i++)
			rlc_type_modifier_destroy(&this->fTypeModifiers[i]);
		rlc_free((void**)&this->fTypeModifiers);
		this->fTypeModifierCount = 0;
	}

	switch(this->fValue)
	{
	case kRlcParsedTypeNameValueName:
		{
			if(this->fName)
			{
				rlc_parsed_symbol_destroy(this->fName);
				rlc_free((void**)&this->fName);
			}
		} break;
	case kRlcParsedTypeNameValueFunction:
		{
			if(this->fFunction)
			{
				rlc_parsed_function_signature_destroy(this->fFunction);
				rlc_free((void**)&this->fFunction);
			}
		} break;
	case kRlcParsedTypeNameValueExpression:
		{
			if(this->fExpression)
			{
				rlc_parsed_expression_destroy_virtual(this->fExpression);
				rlc_free((void**)&this->fExpression);
			}
		} break;
	case kRlcParsedTypeNameValueTuple:
		{
			if(this->fTuple.fTypes)
			{
				for(RlcSrcIndex i = 0; i < this->fTuple.fTypeCount; i++)
					rlc_parsed_type_name_destroy(&this->fTuple.fTypes[i]);
				rlc_free((void**)&this->fTuple.fTypes);
				this->fTuple.fTypeCount = 0;
			}
		} break;
	case kRlcParsedTypeNameValueVoid:
	case kRlcParsedTypeNameValueSymbolConstant: { ; }
	}
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
	this->fReferenceType = kRlcReferenceTypeNone;

	this->fTuple.fTypeCount = 0;
	this->fTypeModifiers = NULL;
	this->fTypeModifierCount = 0;
}

static int rlc_parsed_type_name_parse_impl(
	struct RlcParsedTypeName * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	rlc_parsed_type_name_create(out);

	out->fValue = kRlcParsedTypeNameValueVoid;
	out->fNoDecay = 0;

	union {
		struct RlcParsedSymbol fSymbol;
		struct RlcParsedFunctionSignature fFunction;
	} parse;

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokVoid))
	{
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokType))
	{
		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokParentheseOpen);

		if(!(out->fExpression = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType))))
		{
			rlc_parser_fail(parser, "expected expression");
		}

		out->fValue = kRlcParsedTypeNameValueExpression;

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokParentheseClose);
	} else if(rlc_parsed_symbol_parse(
		&parse.fSymbol,
		parser,
		0))
	{
		out->fValue = kRlcParsedTypeNameValueName;
		rlc_malloc((void**)&out->fName, sizeof(struct RlcParsedSymbol));
		*out->fName = parse.fSymbol;
		out->fNoDecay = rlc_parser_consume(
			parser,
			NULL,
			kRlcTokExclamationMark);
	} else if(rlc_parsed_function_signature_parse(
		&parse.fFunction,
		parser))
	{
		out->fValue = kRlcParsedTypeNameValueFunction;
		rlc_malloc((void**)&out->fFunction, sizeof(struct RlcParsedFunctionSignature));
		*out->fFunction = parse.fFunction;
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokColon))
	{
		out->fValue = kRlcParsedTypeNameValueSymbolConstant;
		struct RlcToken name;
		rlc_parser_expect(parser, &name, 1, kRlcTokIdentifier);
		out->fSymbolConstant = name.content;
		rlc_parsed_symbol_constant_register(rlc_parser_file(parser), &out->fSymbolConstant);
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBraceOpen))
	{
		out->fValue = kRlcParsedTypeNameValueTuple;
		struct RlcParsedTypeName type;
		do {
			if(!rlc_parsed_type_name_parse(&type, parser, 1))
				rlc_parser_fail(parser, "expected type name");
			rlc_realloc(
				(void**)&out->fTuple.fTypes,
				++out->fTuple.fTypeCount * sizeof(struct RlcParsedTypeName));
			out->fTuple.fTypes[out->fTuple.fTypeCount-1] = type;
		} while(rlc_parser_consume(parser, NULL, kRlcTokComma));
		rlc_parser_expect(parser, NULL, 1, kRlcTokBraceClose);
	} else
		return 0;

	struct RlcTypeModifier modifier;

	int expect_indirection = 0;
	while(rlc_type_modifier_parse(
		&modifier,
		parser,
		expect_indirection))
	{
		expect_indirection = 1;

		rlc_parsed_type_name_add_modifier(
			out,
			&modifier);
	}

	return 1;
}

int rlc_parsed_type_name_parse(
	struct RlcParsedTypeName * out,
	struct RlcParser * parser,
	int allow_reference)
{
	if(!rlc_parsed_type_name_parse_impl(out, parser))
		return 0;

after_type_name:
	if(rlc_parser_is_ahead(parser, kRlcTokMinus)
	|| allow_reference)
	{
		if(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokAnd))
		{
			out->fReferenceType = kRlcReferenceTypeReference;
		} else if(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokDoubleAnd))
		{
			out->fReferenceType = kRlcReferenceTypeTempReference;
		} else
			out->fReferenceType = kRlcReferenceTypeNone;
	}

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokMinus))
	{
		// Expect starting token of symbol.
		static enum RlcTokenType const expect[] = {
			kRlcTokDoubleColon,
			kRlcTokBracketOpen,
			kRlcTokIdentifier
		};
		int found = 0;
		for(size_t i = 0; i < _countof(expect); i++)
			found |= rlc_parser_is_current(parser, expect[i]);
		if(!found)
			rlc_parser_fail(parser, "expected symbol");

		// Parse next type (symbol type).
		struct RlcParsedTypeName * temp = NULL;
		rlc_malloc((void**)&temp, sizeof(struct RlcParsedTypeName));
		*temp = *out;
		if(!rlc_parsed_type_name_parse_impl(out, parser))
			rlc_parser_fail(parser, "expected type name");
		RLC_ASSERT(out->fValue == kRlcParsedTypeNameValueName);

		// Insert previous type as last template argument to next type.
		struct RlcParsedSymbol * symbol = out->fName;
		struct RlcParsedSymbolChild * child = &symbol->fChildren[symbol->fChildCount-1];
		rlc_realloc(
			(void**)&child->fTemplates,
			++child->fTemplateCount * sizeof(struct RlcParsedSymbolChildTemplate));
		child->fTemplates[child->fTemplateCount-1].fIsExpression = 0;
		child->fTemplates[child->fTemplateCount-1].fTypeName = temp;

		goto after_type_name;
	}

	return 1;
}

struct RlcTypeModifier const * rlc_parsed_type_name_top_modifier(
	struct RlcParsedTypeName const * this)
{
	RLC_DASSERT(this != NULL);
	static struct RlcTypeModifier const kDefault = {
		kRlcTypeIndirectionPlain, kRlcTypeQualifierNone, 0, NULL
	};

	if(!this->fTypeModifierCount)
	{
		return &kDefault;
	} else {
		return &this->fTypeModifiers[this->fTypeModifierCount-1];
	}
}

void rlc_parsed_type_name_print(
	struct RlcParsedTypeName const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	// Arrays are printed as "typename ::__rl::template array<..., size>".
	for(RlcSrcIndex i = this->fTypeModifierCount; i--;)
	{
		if(this->fTypeModifiers[i].fIsArray)
		{
			if(this->fTypeModifiers[i].fArraySize != NULL)
				fputs("typename ::__rl::template array<", out);
			else
				fputs("typename ::__rl::template unsized_array<", out);
		}
		if(this->fTypeModifiers[i].fTypeIndirection == kRlcTypeIndirectionFuture)
			fputs("::std::future<", out);
	}

	switch(this->fValue)
	{
	case kRlcParsedTypeNameValueVoid:
		{
			fprintf(out, "void");
		} break;
	case kRlcParsedTypeNameValueName:
		{
			int decay = !this->fNoDecay
				&& this->fName->fChildCount == 1
				&& !this->fName->fChildren[0].fTemplateCount;
			if(decay)
				fputs("::std::decay_t<", out);
			for(RlcSrcIndex i = 0; (RlcSrcIndex)(i+1) < this->fName->fChildCount; i++)
				if(this->fName->fChildren[i].fTemplateCount)
				{
					fputs(" typename ", out);
					break;
				}
			rlc_parsed_symbol_print(
				this->fName,
				file,
				out);
			if(decay)
				fputs(">", out);
		} break;
	case kRlcParsedTypeNameValueSymbolConstant:
		{
			fputs("::__rl::constant::_t_", out);
			rlc_src_string_print_noreplace(&this->fSymbolConstant, file, out);
		} break;
	case kRlcParsedTypeNameValueFunction:
		{
			rlc_parsed_function_signature_print(
				this->fFunction,
				file,
				out);
		} break;
	case kRlcParsedTypeNameValueExpression:
		{
			fputs("__rl::auto_t<decltype(", out);
			rlc_parsed_expression_print(this->fExpression, file, out);
			fputs(")>", out);
		} break;
	case kRlcParsedTypeNameValueTuple:
		{
			fputs("__rl::Tuple<", out);
			for(RlcSrcIndex i = 0; i < this->fTuple.fTypeCount; i++)
			{
				if(i) fputs(", ", out);
				rlc_parsed_type_name_print(&this->fTuple.fTypes[i], file, out);
			}
			fputs(">", out);
		} break;
	}

	for(RlcSrcIndex i = 0; i < this->fTypeModifierCount; i++)
	{
		switch(this->fTypeModifiers[i].fTypeIndirection)
		{
		case kRlcTypeIndirectionPlain: break;
		case kRlcTypeIndirectionNotNull:
		case kRlcTypeIndirectionPointer:
			{
				fprintf(out, " *");
			} break;
		case kRlcTypeIndirectionFuture: break;
		default:
			RLC_ASSERT(!"not implemented");
		}

		if(this->fTypeModifiers[i].fTypeQualifier & kRlcTypeQualifierConst)
			fprintf(out, " const");
		if(this->fTypeModifiers[i].fTypeQualifier & kRlcTypeQualifierVolatile)
			fprintf(out, " volatile");

		if(this->fTypeModifiers[i].fIsArray)
		{
			if(this->fTypeModifiers[i].fArraySize != NULL)
			{
				fputs(", ", out);
				rlc_parsed_expression_print(
					this->fTypeModifiers[i].fArraySize,
					file,
					out);
			}
			fputs(">", out);
		}
		if(this->fTypeModifiers[i].fTypeIndirection == kRlcTypeIndirectionFuture)
			fputs(">", out);
	}

	if(this->fReferenceType == kRlcReferenceTypeReference)
		fprintf(out, "&");
	if(this->fReferenceType == kRlcReferenceTypeTempReference)
		fprintf(out, "&&");
}

void rlc_parsed_function_signature_create(
	struct RlcParsedFunctionSignature * this)
{
	RLC_DASSERT(this != NULL);

	this->fArguments = NULL;
	this->fArgumentCount = 0;
	rlc_parsed_type_name_create(&this->fResult);
	this->fIsAsync = 0;
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
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokParentheseOpen))
	{
		return 0;
	}

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "signature", &tracer);

	rlc_parsed_function_signature_create(out);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokParentheseClose))
	{
		;
	// explicit void arguments?
	} else if(rlc_parser_is_current(
		parser,
		kRlcTokVoid)
	&& rlc_parser_is_ahead(
		parser,
		kRlcTokParentheseClose))
	{
		rlc_parser_skip(parser);
		rlc_parser_skip(parser);
	} else
	{
		// parse argument list.
		struct RlcParsedTypeName name;
		do {
			if(!rlc_parsed_type_name_parse(
				&name,
				parser,
				1))
			{
				rlc_parser_fail(parser, "expected type name");
			}
			rlc_parsed_function_signature_add_argument(
				out,
				&name);
		} while(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokComma));

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokParentheseClose);
	}

	if((out->fIsAsync = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokAt)))
	{
		out->fIsClosure = 0;
	} else
	{
		out->fIsClosure = rlc_parser_consume(
			parser,
			NULL,
			kRlcTokAnd);
	}

	if(!rlc_parsed_type_name_parse(
		&out->fResult,
		parser,
		1))
	{
		rlc_parser_fail(parser, "expected type name");
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseClose);

	rlc_parser_untrace(parser, &tracer);
	return 1;
}

void rlc_parsed_function_signature_print(
	struct RlcParsedFunctionSignature const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	RLC_ASSERT(!this->fIsClosure);

	fprintf(out, "::__rl::function_t<");
	if(this->fIsAsync)
		fputs("::std::future<", out);
	rlc_parsed_type_name_print(&this->fResult, file, out);
	if(this->fIsAsync)
		fputs(">", out);
	for(RlcSrcIndex i=0; i < this->fArgumentCount; i++)
	{
		fprintf(out, ", ");
		rlc_parsed_type_name_print(&this->fArguments[i], file, out);
	}
	fprintf(out, ">");
}