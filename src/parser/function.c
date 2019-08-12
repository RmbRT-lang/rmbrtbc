#include "function.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_function_create(
	struct RlcParsedFunction * this,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedFunction,
		start_index);

	this->fHasReturnType = 0;

	this->fArguments = NULL;
	this->fArgumentCount = 0;

	this->fIsInline = 0;
	this->fIsAsync = 0;

	rlc_parsed_block_statement_create(&this->fBodyStatement);
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

#ifdef RLC_DEBUG
	rlc_parsed_variable_create(variable, 0);
#endif
}

int rlc_parsed_function_parse(
	struct RlcParsedFunction * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);


	size_t const start_index = parser->fIndex;

	enum RlcParseError error_code;

	rlc_parsed_function_create(
		out,
		parser->fIndex);

	if(!rlc_parsed_template_decl_parse(
		&out->fTemplates,
		parser))
	{
		RLC_DASSERT(parser->fErrorCount > 0);
		error_code = kRlcParseErrorExpectedTemplateDeclaration;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier))
	{
		goto nonfatal_failure;
	}


	rlc_parsed_scope_entry_add_name(
		RLC_BASE_CAST(out, RlcParsedScopeEntry),
		rlc_parser_data_consumed_index(parser));

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseOpen))
	{
		goto nonfatal_failure;
	}

	// parse arguments.

	do
	{
		struct RlcParsedVariable argument;
		if(rlc_parsed_variable_parse(
			&argument,
			parser,
			0,
			0,
			0,
			0))
		{
			rlc_parsed_function_add_argument(
				out,
				&argument);
		} else if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedArgument;
			goto failure;
		} else break;

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

	int accept_block_statement = 1;
	int accept_expression = 0;
	int expect_semicolon = 1;

	if((out->fHasReturnType = rlc_parser_data_consume(
		parser,
		kRlcTokColon)))
	{
		// parse return type.

		if(!rlc_parsed_type_name_parse(
			&out->fReturnType,
			parser))
		{
			error_code = kRlcParseErrorExpectedTypeName;
			goto failure;
		}

		if(!rlc_parser_data_consume(
			parser,
			kRlcTokColonEqual))
		{
			error_code =
				(out->fHasReturnType)
				? kRlcParseErrorExpectedColonEqual
				: kRlcParseErrorExpectedColonEqualOrColon;
			goto failure;
		}

		accept_expression = 1;
	} else if(rlc_parser_data_consume(
		parser,
		kRlcTokDoubleColonEqual))
	{
		accept_expression = 1;
		accept_block_statement = 0;
	} else expect_semicolon = 0;

	out->fIsAsync = rlc_parser_data_consume(
		parser,
		kRlcTokAt);

	out->fIsInline = rlc_parser_data_consume(
		parser,
		kRlcTokInline);

	// parse function body.

	if(accept_block_statement
	&& rlc_parsed_block_statement_parse(
		&out->fBodyStatement,
		parser))
	{
		out->fIsShortHandBody = 0;
	} else if(parser->fErrorCount)
	{
		// prepare values so that the destructor can be called.
		//*
		out->fIsShortHandBody = 1;
		out->fReturnValue = NULL; //*/
		error_code = kRlcParseErrorExpectedBlockStatement;
		goto failure;
	} else if(accept_expression
	&& (out->fReturnValue = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType)
		&~RLC_FLAG(kRlcParsedTypeNameExpression))))
	{
		out->fIsShortHandBody = 1;
	} else
	{
		if(!accept_expression)
			error_code = kRlcParseErrorExpectedBlockStatement;
		else
			error_code = kRlcParseErrorExpectedExpression;
		goto failure;
	}

	if(expect_semicolon
	&& !rlc_parser_data_consume(
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
nonfatal_failure:
	rlc_parsed_function_destroy(out);
	parser->fIndex = start_index;
	return 0;
}

void rlc_parsed_member_function_create(
	struct RlcParsedMemberFunction * this,
	enum RlcVisibility visiblity,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberFunction,
		visiblity,
		0);

	rlc_parsed_function_create(
		RLC_BASE_CAST(this, RlcParsedFunction),
		0);
}

int rlc_parsed_member_function_parse(
	struct RlcParsedMemberFunction * out,
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(default_visibility != NULL);

	size_t const start_index = parser->fIndex;
	enum RlcParseError error_code;

	enum RlcVisibility visibility = rlc_visibility_parse(
		default_visibility,
		parser);

	rlc_parsed_member_function_create(
		out,
		start_index,
		visibility);

	out->fAttribute = rlc_member_attribute_parse(parser);

	if(!rlc_parsed_function_parse(
		RLC_BASE_CAST(out, RlcParsedFunction),
		parser))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedFunction;
			goto fatal_failure;
		} else goto nonfatal_failure;
	}

success:
	RLC_BASE_CAST(out, RlcParsedMember)->fLocation.fEnd = parser->fIndex;
	return 1;
fatal_failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
nonfatal_failure:
	rlc_parsed_member_function_destroy(out);
	parser->fIndex = start_index;
	return 0;
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