#include "function.h"

#include "../assert.h"
#include "../malloc.h"
#include "../resolver/resolver.h"

void rlc_parsed_function_create(
	struct RlcParsedFunction * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedFunction,
		name);

	this->fTemplates = *templates;

	this->fHasReturnType = kRlcFunctionReturnTypeNone;
	this->fAutoReturnQualifier = kRlcTypeQualifierNone;
	this->fAutoReturnReference = kRlcReferenceTypeNone;

	this->fArguments = NULL;
	this->fArgumentCount = 0;

	this->fType = kRlcFunctionTypeFunction;
	this->fIsInline = 0;
	this->fIsAsync = 0;

	this->fHasBody = 0;
	this->fIsShortHandBody = 0;
}

void rlc_parsed_function_destroy(
	struct RlcParsedFunction * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fHasReturnType)
		rlc_parsed_type_name_destroy(&this->fReturnType);
	this->fHasReturnType = 0;

	if(this->fArguments)
	{
		for(size_t i = 0; i < this->fArgumentCount; i++)
			rlc_parsed_variable_destroy(&this->fArguments[i]);
		rlc_free((void**)&this->fArguments);
	}
	this->fArgumentCount = 0;

	this->fIsInline = 0;

	if(this->fHasBody)
	{
		if(!this->fIsShortHandBody)
		{
			rlc_parsed_block_statement_destroy(&this->fBodyStatement);
		} else
		{
			if(this->fReturnValue)
			{
				rlc_parsed_expression_destroy_virtual(
					this->fReturnValue);

				rlc_free((void**)&this->fReturnValue);
			}
		}
		this->fHasBody = 0;
	}

	rlc_parsed_template_decl_destroy(&this->fTemplates);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	this->fIsShortHandBody = 0;
}

void rlc_parsed_function_add_argument(
	struct RlcParsedFunction * this,
	struct RlcParsedVariable * variable)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(variable != NULL);

	rlc_realloc(
		(void**)&this->fArguments,
		sizeof(struct RlcParsedVariable) * ++this->fArgumentCount);

	this->fArguments[this->fArgumentCount - 1] = *variable;
}

int rlc_parsed_function_parse(
	struct RlcParsedFunction * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates,
	int allow_body)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcFunctionType fnType;
	int isThisFirst = 0;
	struct RlcToken name;
	if(rlc_parser_is_ahead(
		parser,
		kRlcTokParentheseOpen)
	&& rlc_parser_consume(
		parser,
		&name,
		kRlcTokIdentifier))
	{
		fnType = kRlcFunctionTypeFunction;
	} else if(rlc_parser_consume(
		parser,
		&name,
		kRlcTokLess))
	{
		fnType = kRlcFunctionTypeCast;
	} else if((isThisFirst = rlc_parser_consume(
		parser,
		&name,
		kRlcTokThis))
	|| rlc_parser_is_ahead(
		parser,
		kRlcTokThis))
	{
		fnType = kRlcFunctionTypeOperator;
	} else if(rlc_parser_consume(
		parser,
		&name,
		kRlcTokTripleLess))
	{
		fnType = kRlcFunctionTypeFactory;
	} else
		return 0;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "function", &tracer);
	rlc_parsed_function_create(
		out,
		&name.content,
		templates);
	out->fType = fnType;

	switch(out->fType)
	{
	case kRlcFunctionTypeCast:
		{
			out->fHasReturnType = kRlcFunctionReturnTypeType;

			if(!rlc_parsed_type_name_parse(&out->fReturnType, parser, 1))
				rlc_parser_fail(parser, "expected type name");

			rlc_parser_expect(parser, NULL, 1, kRlcTokGreater);
		} break;
	case kRlcFunctionTypeFactory:
		{
			struct RlcParsedVariable arg;
			do
			{
				if(!rlc_parsed_variable_parse(&arg, parser, NULL, 0, 0, 0, 0, 1))
					rlc_parser_fail(parser, "expected argument");
				rlc_parsed_function_add_argument(out, &arg);
			} while(rlc_parser_consume(parser, NULL, kRlcTokComma));
			rlc_parser_expect(parser, NULL, 1, kRlcTokTripleGreater);
		} break;
	// Expecting operator now (except '(' and '[')?
	case kRlcFunctionTypeOperator:
	if(!isThisFirst
	|| (!rlc_parser_is_current(
			parser,
			kRlcTokParentheseOpen)
		&& !rlc_parser_is_current(
			parser,
			kRlcTokBracketOpen)))
	{
		if(isThisFirst)
		{
			int arity = 0;
			if(!rlc_operator_parse_unary_postfix(&out->fOperatorName, parser)
			&& !(arity = rlc_operator_parse_binary(&out->fOperatorName, parser))
			&& !(arity = 2 * rlc_parser_consume(parser, NULL, kRlcTokQuestionMark)))
				rlc_parser_fail(parser, "expected postfix or binary operator");

			if(arity == 2)
				out->fOperatorName = kConditional;

			// this++
			// this + (arg)
			// this ? (arg) : (arg)
			for(int i = 0; i < arity; i++)
			{
				rlc_parser_expect(parser, NULL, 1, kRlcTokParentheseOpen);
				struct RlcParsedVariable rhs;
				if(!rlc_parsed_variable_parse(&rhs, parser, NULL, 0,0,0,0,1))
					rlc_parser_fail(parser, "expected argument");
				rlc_parsed_function_add_argument(out, &rhs);
				rlc_parser_expect(parser, NULL, 1, kRlcTokParentheseClose);

				if(!i && arity == 2)
					rlc_parser_expect(parser, NULL, 1, kRlcTokColon);
			}
		} else
		{
			// ++this
			if(!rlc_operator_parse_unary_prefix(&out->fOperatorName, parser))
				rlc_parser_fail(parser, "expected prefix operator before THIS");
			rlc_parser_expect(parser, &name, 1, kRlcTokThis);
			RLC_BASE_CAST(out, RlcParsedScopeEntry)->fName = name.content;
		}
		break;
	} // fallthrough
	case kRlcFunctionTypeFunction:
		{
			switch(rlc_parser_expect(
				parser,
				NULL,
				1 + isThisFirst,
				kRlcTokParentheseOpen,
				kRlcTokBracketOpen))
			{
			default: RLC_DASSERT(!"this should never happen");
			case kRlcTokParentheseOpen:	out->fOperatorName = kCall; break;
			case kRlcTokBracketOpen: out->fOperatorName = kSubscript; break;
			}

			// parse arguments.
			struct RlcParsedVariable argument;
			while(rlc_parsed_variable_parse(
					&argument,
					parser,
					NULL,
					0,
					0,
					0,
					0,
					1))
			{
				rlc_parsed_function_add_argument(
					out,
					&argument);

				if(!rlc_parser_consume(
					parser,
					NULL,
					kRlcTokComma))
					break;
			}

			rlc_parser_expect(
				parser,
				NULL,
				1,
				out->fOperatorName == kCall
					? kRlcTokParentheseClose
					: kRlcTokBracketClose);
		}
	}

	out->fIsAsync = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokAt);

	out->fIsInline = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokInline);

	if(fnType != kRlcFunctionTypeCast
	&& rlc_parsed_type_name_parse(
		&out->fReturnType,
		parser,
		1))
	{
		out->fHasReturnType = kRlcFunctionReturnTypeType;
		if(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokSemicolon))
		{
			out->fHasBody = 0;
			rlc_parser_untrace(parser, &tracer);
			return 1;
		} else if(!allow_body)
			rlc_parser_fail(parser, "expected ';'");
	} else if(!allow_body)
		rlc_parser_fail(parser, "expected return type");

	out->fHasBody = 1;
	if(!out->fHasReturnType)
	{
		rlc_type_qualifier_parse(&out->fAutoReturnQualifier, parser);
		if(rlc_parser_consume(parser, NULL, kRlcTokAnd))
			out->fAutoReturnReference = kRlcReferenceTypeReference;
		else if(rlc_parser_consume(parser, NULL, kRlcTokDoubleAnd))
			out->fAutoReturnReference = kRlcReferenceTypeTempReference;

		if(rlc_parser_consume(parser, NULL, kRlcTokQuestionMark))
		{
			out->fHasReturnType = kRlcFunctionReturnTypeAuto;
			if(!rlc_parsed_block_statement_parse(
				&out->fBodyStatement,
				parser))
				rlc_parser_fail(parser, "expected block statement");

			out->fIsShortHandBody = 0;
		} else
			out->fIsShortHandBody = 1;
	}
	else
		out->fIsShortHandBody = !rlc_parsed_block_statement_parse(
			&out->fBodyStatement,
			parser);

	if(out->fIsShortHandBody)
	{
		enum RlcTokenType tok;
		if(out->fHasReturnType == kRlcFunctionReturnTypeType)
			tok = rlc_parser_expect(
				parser,
				NULL,
				1,
				kRlcTokColonEqual);
		else
			tok = rlc_parser_expect(
				parser,
				NULL,
				2,
				kRlcTokColonEqual,
				kRlcTokDoubleColonEqual);

		if(tok == kRlcTokDoubleColonEqual)
			out->fHasReturnType = kRlcFunctionReturnTypeAuto;

		if(!(out->fReturnValue = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType))))
		{
			rlc_parser_fail(parser, "expected expression");
		}

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokSemicolon);
	}

	rlc_parser_untrace(parser, &tracer);
	return 1;
}

static void rlc_parsed_function_print_head_1(
	struct RlcParsedFunction const * this,
	struct RlcSrcFile const * file,
	FILE * out,
	int templates)
{
	if(templates)
		rlc_parsed_template_decl_print(&this->fTemplates, file, out);

	if(this->fType != kRlcFunctionTypeCast
	&& this->fHasReturnType != kRlcFunctionReturnTypeNone)
		fputs("auto ", out);
}

static void rlc_parsed_function_print_head_2(
	struct RlcParsedFunction const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	switch(this->fType)
	{
	case kRlcFunctionTypeFunction:
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		break;
	case kRlcFunctionTypeFactory:
		fputs("__rl_factory", out);
		break;
	case kRlcFunctionTypeOperator:
		{
			if(this->fOperatorName == kStreamFeed)
			{
				fputs("__rl_stream_feed", out);
				break;
			} else if(this->fOperatorName == kCount)
			{
				fputs("__rl_count", out);
				break;
			} else if(this->fOperatorName == kValueOf)
			{
				fputs("__rl_value_of", out);
				break;
			}

			fputs("operator ", out);
			char const * op;
			switch(this->fOperatorName)
			{
			case kAdd: case kPos: op = "+"; break;
			case kSub: case kNeg: op = "-"; break;
			case kMul: case kDereference: op = "*"; break;
			case kDiv: op = "/"; break;
			case kMod: op = "%"; break;
			case kEquals: op = "=="; break;
			case kNotEquals: op = "!="; break;
			case kLess: op = "<"; break;
			case kLessEquals: op = "<="; break;
			case kGreater: op = ">"; break;
			case kGreaterEquals: op = ">="; break;

			case kBitAnd: op = "&"; break;
			case kBitOr: op = "|"; break;
			case kBitXor: op = "^"; break;
			case kBitNot: op = "~"; break;

			case kLogAnd: op = "&&"; break;
			case kLogOr: op = "||"; break;
			case kLogNot: op = "!"; break;

			case kShiftLeft: op = "<<"; break;
			case kShiftRight: op = ">>"; break;

			case kSubscript: op = "[]"; break;
			case kCall: op = "()"; break;

			case kPreIncrement: case kPostIncrement: op = "++"; break;
			case kPreDecrement: case kPostDecrement: op = "--"; break;

			case kAwait: op = "co_await"; break;

			case kAssign: op = "="; break;

			case kAddAssign: op = "+="; break;
			case kSubAssign: op = "-="; break;
			case kMulAssign: op = "*="; break;
			case kDivAssign: op = "/="; break;
			case kModAssign: op = "%="; break;

			case kBitAndAssign: op = "&="; break;
			case kBitOrAssign: op = "|="; break;
			case kBitXorAssign: op = "^="; break;

			case kShiftLeftAssign: op = "<<="; break;
			case kShiftRightAssign: op = ">>="; break;
			default:
				rlc_resolver_fail(
					&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
					file,
					"this operator cannot be user-defined");
			}

			fputs(op, out);
		} break;
	case kRlcFunctionTypeCast:
		{
			fputs("operator ", out);
			if(this->fIsAsync)
				fputs("::std::future<", out);
			rlc_parsed_type_name_print(&this->fReturnType, file, out);
			if(this->fIsAsync)
					fputc('>', out);
		} break;
	}
	fputc('(', out);
	for(RlcSrcIndex i = 0; i < this->fArgumentCount; i++)
	{
		if(i)
			fputc(',', out);
		fprintf(out, "\n\t");
		rlc_parsed_variable_print_argument(
			&this->fArguments[i],
			file,
			out,
			1);
	}

	// THIS++/THIS-- needs (int) as dummy argument.
	if(this->fOperatorName == kPostIncrement
	|| this->fOperatorName == kPostDecrement)
		fputs("int", out);

	fprintf(out, ")");
}

static void rlc_parsed_function_print_head_3(
	struct RlcParsedFunction const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(this->fType == kRlcFunctionTypeCast)
		return;

	switch(this->fHasReturnType)
	{
	case kRlcFunctionReturnTypeAuto:
		{
			if(this->fIsShortHandBody)
			{
				fputs(" -> ", out);
				if(this->fIsAsync)
					fputs("::std::future<", out);
				fputs("::__rl::auto_t<decltype(", out);
				rlc_parsed_expression_print(this->fReturnValue, file, out);
				fputs(")>\n", out);
				if(this->fAutoReturnQualifier & kRlcTypeQualifierConst)
					fputs(" const ", out);
				if(this->fAutoReturnQualifier & kRlcTypeQualifierVolatile)
					fputs(" volatile ", out);
				if(this->fAutoReturnReference == kRlcReferenceTypeReference)
					fputs("&", out);
				else if(this->fAutoReturnReference == kRlcReferenceTypeTempReference)
					fputs("&&", out);
				if(this->fIsAsync)
					fputc('>', out);
			} else
			{
				;
			}
		} break;
	case kRlcFunctionReturnTypeType:
		{
			fputs(" -> ", out);
			if(this->fIsAsync)
				fputs("::std::future<", out);
			rlc_parsed_type_name_print(&this->fReturnType, file, out);
			if(this->fIsAsync)
				fputc('>', out);
		} break;
	case kRlcFunctionReturnTypeNone: break;
	}
}

void rlc_parsed_function_print_head(
	struct RlcParsedFunction const * this,
	struct RlcSrcFile const * file,
	FILE * out,
	int templates)
{
	rlc_parsed_function_print_head_1(this, file, out, templates);
	rlc_parsed_function_print_head_2(this, file, out);
	rlc_parsed_function_print_head_3(this, file, out);
}

static void rlc_parsed_function_print_body(
	struct RlcParsedFunction const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(!this->fHasBody)
	{
		fprintf(out, ";\n");
		return;
	}

	if(this->fIsAsync)
		fputs(
			"\n"
			"#define _return co_return\n"
			"#define __rl_sleep co_await ::__rl::sleep_coroutine\n", out);
	else
		fputs(
			"\n"
			"#define _return return\n"
			"#define __rl_sleep ::__rl::sleep_thread\n", out);

	if(this->fIsShortHandBody)
	{
		fprintf(out, "\n{ _return ");
		rlc_parsed_expression_print(this->fReturnValue, file, out);
		fprintf(out, "; }\n");
	} else
	{
		rlc_parsed_block_statement_print(&this->fBodyStatement, file, out);
	}

	fputs("\n"
		"#undef _return\n"
		"#undef __rl_sleep\n", out);
}

void rlc_parsed_function_print(
	struct RlcParsedFunction const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * printer)
{

	rlc_parsed_function_print_head(this, file, printer->fFuncs, 1);
	fputs(";\n", printer->fFuncs);
	rlc_parsed_function_print_head(this, file, printer->fFuncsImpl, 1);
	rlc_parsed_function_print_body(this, file, printer->fFuncsImpl);
}

void rlc_parsed_member_function_create(
	struct RlcParsedMemberFunction * this,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberFunction,
		member);
}

int rlc_parsed_member_function_parse(
	struct RlcParsedMemberFunction * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(member != NULL);

	int any = 0;
	if(rlc_parser_is_current(parser, kRlcTokTripleLess))
	{
		out->fAbstractness = kRlcMemberFunctionAbstractnessNone;
	} else
	{
		static struct {
			enum RlcTokenType token;
			enum RlcMemberFunctionAbstractness value;
		} k_lookup[] = {
			{ kRlcTokVirtual, kRlcMemberFunctionAbstractnessVirtual },
			{ kRlcTokAbstract, kRlcMemberFunctionAbstractnessAbstract },
			{ kRlcTokOverride, kRlcMemberFunctionAbstractnessOverride },
			{ kRlcTokFinal, kRlcMemberFunctionAbstractnessFinal }
		};

		for(size_t i = 0; i < _countof(k_lookup); i++)
			if(rlc_parser_consume(
				parser,
				NULL,
				k_lookup[i].token))
			{
				out->fAbstractness = k_lookup[i].value;
				any = 1;
				break;
			}

		if(!any)
			out->fAbstractness = kRlcMemberFunctionAbstractnessNone;
	}

	if(!rlc_parsed_function_parse(
		RLC_BASE_CAST(out, RlcParsedFunction),
		parser,
		&member->templates,
		out->fAbstractness != kRlcMemberFunctionAbstractnessAbstract))
	{
		if(any)
			rlc_parser_fail(parser, "expected function");
		else
			return 0;
	}

	rlc_parsed_member_function_create(
		out,
		member);

	return 1;
}

void rlc_parsed_member_function_destroy(
	struct RlcParsedMemberFunction * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_function_destroy(
		RLC_BASE_CAST(this, RlcParsedFunction));

	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));
}

void rlc_parsed_member_function_print(
	struct RlcParsedMemberFunction const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * printer)
{
	struct RlcParsedMember const * member = RLC_BASE_CAST(
		this,
		RlcParsedMember);

	FILE * out = printer->fTypesImpl;

	rlc_visibility_print(
		member->fVisibility,
		1,
		out);

	rlc_parsed_template_decl_print(
		&RLC_BASE_CAST(this, RlcParsedFunction)->fTemplates,
		file,
		out);

	if(member->fAttribute == kRlcMemberAttributeStatic)
		fputs(" static ", out);

	switch(this->fAbstractness)
	{
	case kRlcMemberFunctionAbstractnessVirtual:
	case kRlcMemberFunctionAbstractnessAbstract:
	case kRlcMemberFunctionAbstractnessOverride:
		fputs(" virtual ", out);
	default:;
	}

	rlc_parsed_function_print_head_1(
		RLC_BASE_CAST(this, RlcParsedFunction),
		file,
		out,
		0);

	rlc_parsed_function_print_head_2(
		RLC_BASE_CAST(this, RlcParsedFunction),
		file,
		out);

	if(member->fAttribute == kRlcMemberAttributeIsolated)
		fputs(" const", out);

	rlc_parsed_function_print_head_3(
		RLC_BASE_CAST(this, RlcParsedFunction),
		file,
		out);

	switch(this->fAbstractness)
	{
	case kRlcMemberFunctionAbstractnessAbstract:
		fputs(" = 0\n", out);
		break;
	case kRlcMemberFunctionAbstractnessFinal:
		fputs(" final\n", out);
		break;
	case kRlcMemberFunctionAbstractnessOverride:
		fputs(" override\n", out);
		break;
	default:;
	}

	fputs(";\n", out);


	if(RLC_BASE_CAST(this, RlcParsedFunction)->fHasBody)
	{
		out = printer->fFuncsImpl;

		rlc_printer_print_ctx_tpl(printer, file, out);
		rlc_parsed_template_decl_print(
			&RLC_BASE_CAST(this, RlcParsedFunction)->fTemplates,
			file,
			out);

		rlc_parsed_function_print_head_1(
			RLC_BASE_CAST(this, RlcParsedFunction),
			file,
			out,
			0);

		rlc_printer_print_ctx_symbol(printer, file, out);
		fputs("::", out);

		rlc_parsed_function_print_head_2(
			RLC_BASE_CAST(this, RlcParsedFunction),
			file,
			out);

		if(member->fAttribute == kRlcMemberAttributeIsolated)
			fputs(" const", out);

		rlc_parsed_function_print_head_3(
			RLC_BASE_CAST(this, RlcParsedFunction),
			file,
			out);

		rlc_parsed_function_print_body(
			RLC_BASE_CAST(this, RlcParsedFunction),
			file,
			out);
	}

	if(RLC_BASE_CAST(this, RlcParsedFunction)->fType == kRlcFunctionTypeOperator)
	{
		switch(RLC_BASE_CAST(this, RlcParsedFunction)->fOperatorName)
		{
		case kDereference:
			{
				fprintf(
					printer->fTypesImpl,
					"\ninline auto operator->() %s { return & * *this; }\n",
					member->fAttribute == kRlcMemberAttributeIsolated ? "const" : "");
			} break;
		default:;
		}
	}
}