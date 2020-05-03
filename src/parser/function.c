#include "function.h"

#include "../assert.h"
#include "../malloc.h"

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

	this->fHasReturnType = 0;

	this->fArguments = NULL;
	this->fArgumentCount = 0;

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

	struct RlcToken name;
	if(!rlc_parser_is_ahead(
		parser,
		kRlcTokParentheseOpen)
	|| !rlc_parser_consume(
		parser,
		&name,
		kRlcTokIdentifier))
		return 0;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "function", &tracer);
	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);


	rlc_parsed_function_create(
		out,
		&name.content,
		templates);

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
		kRlcTokParentheseClose);

	out->fIsAsync = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokAt);

	out->fIsInline = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokInline);

	if((out->fHasReturnType = rlc_parsed_type_name_parse(
		&out->fReturnType,
		parser,
		1)))
	{
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

	if((out->fIsShortHandBody = (!out->fHasReturnType || !rlc_parsed_block_statement_parse(
		&out->fBodyStatement,
		parser))))
	{
		rlc_parser_expect(
			parser,
			NULL,
			1,
			(out->fHasReturnType)
				? kRlcTokColonEqual
				: kRlcTokDoubleColonEqual);

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


	if(this->fHasReturnType)
		rlc_parsed_type_name_print(&this->fReturnType, file, out);
	else
		fputs("auto", out);

	fputc(' ', out);
}

static void rlc_parsed_function_print_head_2(
	struct RlcParsedFunction const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);

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
	fprintf(out, ")");
}

static void rlc_parsed_function_print_head_3(
	struct RlcParsedFunction const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(!this->fHasReturnType)
	{
		RLC_ASSERT(this->fIsShortHandBody);
		fputs(" -> decltype(", out);
		rlc_parsed_expression_print(this->fReturnValue, file, out);
		fputs(")\n", out);
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

	if(this->fIsShortHandBody)
	{
		fprintf(out, "\n{ return ");
		rlc_parsed_expression_print(this->fReturnValue, file, out);
		fprintf(out, "; }\n");
	} else
	{
		rlc_parsed_block_statement_print(&this->fBodyStatement, file, out);
	}
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

	static struct {
		enum RlcTokenType token;
		enum RlcMemberFunctionAbstractness value;
	} k_lookup[] = {
		{ kRlcTokVirtual, kRlcMemberFunctionAbstractnessVirtual },
		{ kRlcTokAbstract, kRlcMemberFunctionAbstractnessAbstract },
		{ kRlcTokOverride, kRlcMemberFunctionAbstractnessOverride },
		{ kRlcTokFinal, kRlcMemberFunctionAbstractnessFinal }
	};

	int any = 0;
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
}