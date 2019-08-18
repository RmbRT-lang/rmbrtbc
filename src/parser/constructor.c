#include "constructor.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_constructor_create(
	struct RlcParsedConstructor * this,
	enum RlcVisibility visibility,
	size_t index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedConstructor,
		visibility,
		index);

	rlc_parsed_template_decl_create(&this->fTemplates);

	this->fArguments = NULL;
	this->fArgumentCount = 0;
	this->fIsDefinition = 0;
	this->fInitialisers = NULL;
	this->fInitialiserCount = 0;
}

void rlc_parsed_constructor_destroy(
	struct RlcParsedConstructor * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_destroy_base(RLC_BASE_CAST(this, RlcParsedMember));

	rlc_parsed_template_decl_destroy(&this->fTemplates);

	if(this->fArguments)
	{
		for(size_t i = this->fArgumentCount; i--;)
			rlc_parsed_variable_destroy(&this->fArguments[i]);
		rlc_free((void**)&this->fArguments);
		this->fArgumentCount = 0;
	}

	if(this->fInitialisers)
	{
		for(size_t i = this->fInitialiserCount; i--;)
			rlc_parsed_initialiser_destroy(&this->fInitialisers[i]);
		rlc_free((void**)&this->fInitialisers);
		this->fInitialiserCount = 0;
	}

	if(this->fIsDefinition)
		rlc_parsed_block_statement_destroy(&this->fBody);
	this->fIsInline = 0;
}

int rlc_parsed_constructor_parse(
	struct RlcParsedConstructor * out,
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start = parser->fIndex;


	enum RlcVisibility visibility = rlc_visibility_parse(
		default_visibility,
		parser);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokConstructor))
	{
		parser->fIndex = start;
		return 0;
	}

	rlc_parsed_constructor_create(
		out,
		visibility,
		start);

	enum RlcParseError error_code;

	if(!rlc_parsed_template_decl_parse(
		&out->fTemplates,
		parser))
	{
		error_code = kRlcParseErrorExpectedTemplateDeclaration;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseOpen))
	{
		error_code = kRlcParseErrorExpectedParentheseOpen;
		goto failure;
	}

	if(rlc_parser_data_match(
		parser,
		kRlcTokVoid)
	&& rlc_parser_data_match_ahead(
		parser,
		kRlcTokParentheseClose))
	{
		rlc_parser_data_next(parser);
		rlc_parser_data_next(parser);
	} else if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseClose))
	{
		do {
			struct RlcParsedVariable argument;

			if(!rlc_parsed_variable_parse(
				&argument,
				parser,
				0,
				0,
				0,
				0,
				1))
			{
				error_code = kRlcParseErrorExpectedArgument;
				goto failure;
			}

			rlc_parsed_constructor_add_argument(
				out,
				&argument);
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

	if(rlc_parser_data_consume(
		parser,
		kRlcTokInline))
	{
		out->fIsInline = 1;
	}

	if(rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		out->fIsDefinition = 0;
		goto success;
	}

	if(rlc_parser_data_consume(
		parser,
		kRlcTokColon))
	{
		do {
			struct RlcParsedInitialiser initialiser;

			if(!rlc_parsed_initialiser_parse(
				&initialiser,
				parser))
			{
				error_code = kRlcParseErrorExpectedInitialiser;
				goto failure;
			}

			rlc_parsed_constructor_add_initialiser(
				out,
				&initialiser);
		} while(rlc_parser_data_consume(
			parser,
			kRlcTokComma));

	}

	if(rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		goto success;
	}

	if(!rlc_parsed_block_statement_parse(
		&out->fBody,
		parser))
	{
		error_code = kRlcParseErrorExpectedBlockStatement;
		goto failure;
	}

	out->fIsDefinition = 1;

success:
	RLC_BASE_CAST(out, RlcParsedMember)->fLocation.fEnd = parser->fIndex;
	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_constructor_destroy(out);
	return 0;
}

void rlc_parsed_constructor_add_argument(
	struct RlcParsedConstructor * this,
	struct RlcParsedVariable * argument)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(argument != NULL);

	rlc_realloc(
		(void**)&this->fArguments,
		sizeof(struct RlcParsedVariable) * ++this->fArgumentCount);

	this->fArguments[this->fArgumentCount-1] = *argument;
}

void rlc_parsed_constructor_add_initialiser(
	struct RlcParsedConstructor * this,
	struct RlcParsedInitialiser * initialiser)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(initialiser != NULL);

	rlc_realloc(
		(void**)&this->fInitialisers,
		sizeof(struct RlcParsedVariable) * ++this->fInitialiserCount);

	this->fInitialisers[this->fInitialiserCount-1] = *initialiser;
}

void rlc_parsed_initialiser_create(
	struct RlcParsedInitialiser * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_symbol_child_create(&this->fMember);

	this->fArguments = NULL;
	this->fArgumentCount = 0;
}


void rlc_parsed_initialiser_destroy(
	struct RlcParsedInitialiser * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_symbol_child_destroy(&this->fMember);

	if(this->fArguments)
	{
		while(this->fArgumentCount--)
		{
			rlc_parsed_expression_destroy_virtual(this->fArguments[this->fArgumentCount]);
			rlc_free((void**)&this->fArguments[this->fArgumentCount]);
		}

		rlc_free((void**)&this->fArguments);
	}
}

int rlc_parsed_initialiser_parse(
	struct RlcParsedInitialiser * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcParseError error_code;

	rlc_parsed_initialiser_create(out);

	if(!rlc_parsed_symbol_child_parse(
		&out->fMember,
		parser))
	{
		error_code = kRlcParseErrorExpectedIdentifier;
		goto failure;
	}

	struct RlcParsedExpression * argument;

	if(rlc_parser_data_consume(
		parser,
		kRlcTokColonEqual))
	{
		if(!(argument = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType))))
		{
			error_code = kRlcParseErrorExpectedExpression;
			goto failure;
		}

		rlc_parsed_initialiser_add_argument(out, argument);
	} else if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseOpen))
	{
		error_code = kRlcParseErrorExpectedParentheseOpen;
		goto failure;
	} else if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseClose))
	{
		do {
			if(!(argument = rlc_parsed_expression_parse(
				parser,
				RLC_ALL_FLAGS(RlcParsedExpressionType))))
			{
				error_code = kRlcParseErrorExpectedExpression;
				goto failure;
			}

			rlc_parsed_initialiser_add_argument(out, argument);
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

	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_initialiser_destroy(out);
	return 0;
}

void rlc_parsed_initialiser_add_argument(
	struct RlcParsedInitialiser * this,
	struct RlcParsedExpression * argument)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(argument != NULL);

	rlc_realloc(
		(void**)&this->fArguments,
		sizeof(struct RlcParsedExpression*) * ++this->fArgumentCount);

	this->fArguments[this->fArgumentCount-1] = argument;
}