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
	struct RlcParsedTemplateDecl const * templates)
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

	rlc_parser_skip(parser);

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

	int accept_block_statement = 1;
	int accept_expression = 0;
	int expect_semicolon = 1;

	out->fIsAsync = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokAt);

	if((out->fHasReturnType = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokColon)))
	{
		// parse return type.

		if(!rlc_parsed_type_name_parse(
			&out->fReturnType,
			parser))
		{
			rlc_parser_fail(parser, "expected type name.");
		}

		if(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokSemicolon))
		{
			out->fHasBody = 0;
			return 1;
		}

		if(!rlc_parser_consume(
			parser,
			NULL,
			kRlcTokColonEqual))
		{
			rlc_parser_fail(parser,
				(out->fHasReturnType)
				? "expected ':='"
				: "expected '::=' or ':'");
		}

		accept_expression = 1;
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokDoubleColonEqual))
	{
		accept_expression = 1;
		accept_block_statement = 0;
	} else expect_semicolon = 0;

	out->fIsInline = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokInline);


	// parse function body.

	if(accept_block_statement
	&& rlc_parsed_block_statement_parse(
		&out->fBodyStatement,
		parser))
	{
		out->fHasBody = 1;
		out->fIsShortHandBody = 0;
	} else if(accept_expression
	&& (out->fReturnValue = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
	{
		out->fHasBody = 1;
		out->fIsShortHandBody = 1;
	} else
	{
		if(!accept_expression)
			rlc_parser_fail(parser,
				accept_expression
				? "expected expression"
				: "expected block statement");
	}

	if(expect_semicolon)
		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokSemicolon);
	return 1;
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
	struct RlcParsedMemberCommon const * member,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(member != NULL);

	if(!rlc_parsed_function_parse(
		RLC_BASE_CAST(out, RlcParsedFunction),
		parser,
		&member->templates))
		return 0;

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