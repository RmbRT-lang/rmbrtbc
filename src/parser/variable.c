#include "variable.h"
#include "operatorexpression.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_variable_create(
	struct RlcParsedVariable * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedVariable,
		name);

	if(!templates)
		rlc_parsed_template_decl_create(&this->fTemplates);
	else
		this->fTemplates = *templates;

	this->fIsNoInit = 0;
	this->fHasType = 0;
	this->fInitArgs = NULL;
	this->fInitArgCount = 0;
}

void rlc_parsed_variable_destroy(
	struct RlcParsedVariable * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	rlc_parsed_template_decl_destroy(&this->fTemplates);
	if(this->fHasType)
		rlc_parsed_type_name_destroy(&this->fType);

	if(this->fInitArgs)
	{
		for(size_t i = this->fInitArgCount; i--;)
		{
			rlc_parsed_expression_destroy_virtual(this->fInitArgs[i]);
			rlc_free((void**)&this->fInitArgs[i]);
		}
		rlc_free((void**)&this->fInitArgs);
	}
}

static void rlc_parsed_variable_add_arg(
	struct RlcParsedVariable * out,
	struct RlcParsedExpression * arg)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(arg != NULL);

	rlc_realloc(
		(void**)&out->fInitArgs,
		sizeof(struct RlcParsedExpression *) * ++out->fInitArgCount);

	out->fInitArgs[out->fInitArgCount - 1] = arg;
}


int rlc_parsed_variable_parse(
	struct RlcParsedVariable * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates,
	int needs_name,
	int allow_initialiser,
	int force_initialiser,
	int allow_templates,
	int allow_reference)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(rlc_parser_is_current(
		parser,
		kRlcTokIdentifier))
	{
		static struct {
			enum RlcTokenType token;
			int allowIfNeedsName;
		} const k_needed_ahead[] = {
			{ kRlcTokColon, 1 },
			{ kRlcTokDoubleColonEqual, 1 },
			{ kRlcTokHash, 1 },
			{ kRlcTokDollar, 1 },
			{ kRlcTokAnd, 0 },
			{ kRlcTokDoubleAnd, 0 },
			{ kRlcTokAsterisk, 0 },
			{ kRlcTokBackslash, 0 },
			{ kRlcTokAt, 0 },
			{ kRlcTokDoubleAt, 0 },
			{ kRlcTokDoubleDotExclamationMark, 0 },
			{ kRlcTokDoubleDotQuestionMark, 0 },
			{ kRlcTokDoubleColon, 0 },
			{ kRlcTokMinus, 0 },
			{ kRlcTokSemicolon, 0 },
			{ kRlcTokComma, 0 },
			{ kRlcTokParentheseClose, 0 },
			{ kRlcTokBraceClose, 0 },
			{ kRlcTokExclamationMark, 0 }
		};
		int found = 0;
		for(size_t i = 0; i < _countof(k_needed_ahead); i++)
			if(needs_name <= k_needed_ahead[i].allowIfNeedsName
			&& rlc_parser_is_ahead(
				parser,
				k_needed_ahead[i].token))
			{
				found = 1;
				break;
			}
		if(!found)
			return 0;
	}

	int needs_type = 1;
	int has_name = 0;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "variable", &tracer);

	struct RlcToken name;
	if(rlc_parser_is_current(
		parser,
		kRlcTokIdentifier))
	{
		// "name: type" style variable?
		if(rlc_parser_is_ahead(
			parser,
			kRlcTokColon))
		{
			has_name = 1;
			rlc_parser_expect(
				parser,
				&name,
				1,
				kRlcTokIdentifier);
			rlc_parser_skip(parser);
			if(rlc_parser_consume(parser, NULL, kRlcTokQuestionMark))
			{
				needs_type = 0;
				out->fHasType = 0;
				rlc_type_qualifier_parse(
					&out->fTypeQualifier,
					parser);
				if(rlc_parser_consume(parser, NULL, kRlcTokAnd))
					out->fReference = kRlcReferenceTypeReference;
				else if(rlc_parser_consume(parser, NULL, kRlcTokDoubleAnd))
					out->fReference = kRlcReferenceTypeTempReference;
				else
					out->fReference = kRlcReferenceTypeNone;

				rlc_parser_expect(parser, NULL, 1, kRlcTokColonEqual);
			}
		} else if(allow_initialiser)
		{
			static enum RlcTokenType const k_need_ahead[] = {
				kRlcTokHash,
				kRlcTokDollar,
				kRlcTokDoubleColonEqual
			};

			for(size_t i = 0; i < _countof(k_need_ahead); i++)
			{
				if(rlc_parser_is_ahead(
					parser,
					k_need_ahead[i]))
				{
					rlc_parser_expect(
						parser,
						&name,
						1,
						kRlcTokIdentifier);

					enum RlcTypeQualifier qualifier;
					rlc_type_qualifier_parse(
						&qualifier,
						parser);



					// "name ::=" style variable?
					rlc_parser_expect(
						parser,
						NULL,
						1,
						kRlcTokDoubleColonEqual);

					has_name = 1;
					needs_type = 0;
					out->fHasType = 0;
					out->fTypeQualifier = qualifier;
					out->fReference = kRlcReferenceTypeNone;
					break;
				}
			}
		}
	} // If !isArgument, "name: type" is expected.
	if(!has_name
	&& needs_name)
	{
		rlc_parser_untrace(parser, &tracer);
		return 0;
	}

	if(!has_name)
		name.content = kRlcSrcStringEmpty;

	rlc_parsed_variable_create(
		out,
		&name.content,
		templates);

	if(!needs_type)
	{
		struct RlcParsedExpression * init = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType));
		if(!init)
			rlc_parser_fail(parser, "expected expression");
		rlc_parsed_variable_add_arg(out, init);
	} else
	{
		if(!rlc_parsed_type_name_parse(
			&out->fType,
			parser,
			allow_reference))
		{
			if(needs_name)
				rlc_parser_fail(parser, "expected type name");
			else
			{
				rlc_parser_untrace(parser, &tracer);
				return 0;
			}
		}

		out->fHasType = 1;

		if(allow_initialiser)
		{
			int isParenthese = 0;
			if(rlc_parser_consume(
				parser,
				NULL,
				kRlcTokColonEqual)
			|| (isParenthese = rlc_parser_consume(
				parser,
				NULL,
				kRlcTokParentheseOpen)))
			{
				// check for empty initialiser.
				if(!isParenthese
				|| !rlc_parser_consume(
					parser,
					NULL,
					kRlcTokParentheseClose))
				{
					if(!(out->fIsNoInit = rlc_parser_consume(
						parser,
						NULL,
						kRlcTokNoinit)))
					{
						do {
							struct RlcParsedExpression * arg =
								rlc_parsed_expression_parse(
									parser,
									RLC_ALL_FLAGS(RlcParsedExpressionType));

							if(!arg)
								rlc_parser_fail(parser, "expected expression");

							rlc_parsed_variable_add_arg(out, arg);
						} while(isParenthese && rlc_parser_consume(
							parser,
							NULL,
							kRlcTokComma));
					}

					if(isParenthese)
						rlc_parser_expect(
							parser,
							NULL,
							1,
							kRlcTokParentheseClose);
				}
			} else if(force_initialiser)
			{
				rlc_parser_fail(parser, "expected ':=' or '('");
			}
		}
	}

	if(!allow_templates
	&& rlc_parsed_template_decl_exists(&out->fTemplates))
		rlc_parser_fail(parser, "variable had forbidden template declaration");

	rlc_parser_untrace(parser, &tracer);
	return 1;
}

void rlc_parsed_variable_print(
	struct RlcParsedVariable const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * printer)
{
	fputs("extern ", printer->fVars);
	rlc_parsed_variable_print_argument(this, file, printer->fVars, 0);
	fputs(";\n", printer->fVars);

	rlc_parsed_variable_print_argument(this, file, printer->fVarsImpl, 1);
	fputs(";\n", printer->fVarsImpl);
}

static void rlc_parsed_variable_print_argument_1(
	struct RlcParsedVariable const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	rlc_parsed_template_decl_print(&this->fTemplates, file, out);

	if(this->fHasType)
		rlc_parsed_type_name_print(
			&this->fType,
			file,
			out);
	else
	{
		fputs("auto", out);
		if(this->fTypeQualifier & kRlcTypeQualifierConst)
			fputs(" const ", out);
		if(this->fTypeQualifier & kRlcTypeQualifierVolatile)
			fputs(" volatile ", out);
		switch(this->fReference)
		{
		case kRlcReferenceTypeNone: break;
		case kRlcReferenceTypeReference: fputs("&", out); break;
		case kRlcReferenceTypeTempReference: fputs("&&", out); break;
		}
	}
}

static void rlc_parsed_variable_print_argument_2(
	struct RlcParsedVariable const * this,
	struct RlcSrcFile const * file,
	FILE * out,
	int print_initialiser)
{
	if(RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName.length)
	{
		fputc(' ', out);

		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
	}

	if(!print_initialiser)
		return;

	switch(this->fInitArgCount)
	{
	case 0:
		{
			if(!this->fIsNoInit)
				fputs(" = __rl::default_init", out);
		} break;
	case 1:
		{
			fputs(" = ", out);
			struct RlcParsedOperatorExpression * exp;
			int printClose = 1;
			if(!this->fHasType && this->fReference == kRlcReferenceTypeNone)
				fputs("__rl::mk_auto(", out);
			else if((exp = RLC_DYNAMIC_CAST(
				this->fInitArgs[0],
				RlcParsedExpression, RlcParsedOperatorExpression)))
			{
				if((printClose = exp->fOperator == kVariadicExpand))
					fputs("::__rl::single_ctor_arg(", out);
				else
					printClose = 0;
			} else
				printClose = 0;

			rlc_parsed_expression_print(this->fInitArgs[0], file, out);

			if(printClose)
				fputs(")", out);
		} break;
	default:
		{
			fputc('{', out);
			for(RlcSrcIndex i = 0; i < this->fInitArgCount; i++)
			{
				if(i)
					fprintf(out, ", ");
				rlc_parsed_expression_print(this->fInitArgs[i], file, out);
			}
			fputc('}', out);
		} break;
	}
}

void rlc_parsed_variable_print_argument(
	struct RlcParsedVariable const * this,
	struct RlcSrcFile const * file,
	FILE * out,
	int print_initialiser)
{
	rlc_parsed_variable_print_argument_1(this, file, out);
	rlc_parsed_variable_print_argument_2(this, file, out, print_initialiser);
}


void rlc_parsed_member_variable_create(
	struct RlcParsedMemberVariable * this,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(member != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberVariable,
		member);
}

void rlc_parsed_member_variable_destroy(
	struct RlcParsedMemberVariable * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_variable_destroy(
		RLC_BASE_CAST(this, RlcParsedVariable));

	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));
}

int rlc_parsed_member_variable_parse(
	struct RlcParsedMemberVariable * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	int const is_static = member->attribute == kRlcMemberAttributeStatic;

	if(!rlc_parsed_variable_parse(
		RLC_BASE_CAST(out, RlcParsedVariable),
		parser,
		&member->templates,
		is_static, // needs name
		is_static, // allow initialiser
		0, // if static, force name and allow initialiser.
		is_static, // if static, allow template declarations.
		0)) // forbid references.
	{
		return 0;
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);

	rlc_parsed_member_variable_create(
		out,
		member);

	return 1;
}

void rlc_parsed_member_variable_print(
	struct RlcParsedMemberVariable const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer)
{
	FILE * out = printer->fTypesImpl;
	rlc_visibility_print(
		RLC_BASE_CAST(this, RlcParsedMember)->fVisibility,
		1,
		out);

	switch(RLC_BASE_CAST(this, RlcParsedMember)->fAttribute)
	{
	case kRlcMemberAttributeStatic:
		fputs("static ", out);
		break;
	case kRlcMemberAttributeIsolated:
		fputs("mutable ", out);
		break;
	default:;
	}

	rlc_parsed_variable_print_argument(
		RLC_BASE_CAST(this, RlcParsedVariable),
		file,
		out,
		0);

	if(RLC_BASE_CAST2(this, RlcParsedVariable, RlcParsedScopeEntry)->fName.length == 0)
		fputs(" __rl_unnamed", out);

	fputs(";\n", out);

	if(RLC_BASE_CAST(this, RlcParsedMember)->fAttribute == kRlcMemberAttributeStatic)
	{
		out = printer->fVarsImpl;

		rlc_printer_print_ctx_tpl(printer, file, out);
		rlc_parsed_variable_print_argument_1(
			RLC_BASE_CAST(this, RlcParsedVariable),
			file,
			out);
		fputc(' ', out);
		rlc_printer_print_ctx_symbol(printer, file, out);
		fputs("::", out);
		rlc_parsed_variable_print_argument_2(
			RLC_BASE_CAST(this, RlcParsedVariable),
			file,
			out,
			1);
		fputs(";\n", out);
	}
}