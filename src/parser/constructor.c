#include "constructor.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_constructor_create(
	struct RlcParsedConstructor * this,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(member != NULL);
	RLC_DASSERT(member->attribute == kRlcMemberAttributeNone);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedConstructor,
		member);

	this->fTemplates = member->templates;

	this->fType = kRlcDefaultConstructor;
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

	if(this->fType == kRlcCustomConstructor)
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
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(member->attribute != kRlcMemberAttributeNone
	|| !rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBraceOpen))
		return 0;

	rlc_parsed_constructor_create(
		out,
		member);

	struct RlcToken argNameToken;
	if(rlc_parser_consume(parser, NULL, kRlcTokDoubleAnd))
	{
		out->fType = kRlcMoveConstructor;
		if(rlc_parser_consume(parser, &argNameToken, kRlcTokIdentifier))
			out->fArgName = argNameToken.content;
		else
			out->fArgName = kRlcSrcStringEmpty;
	}
	else if(rlc_parser_consume(parser, NULL, kRlcTokHash))
	{
		rlc_parser_expect(parser, NULL, 1, kRlcTokAnd);
		out->fType = kRlcCopyConstructor;
		if(rlc_parser_consume(parser, &argNameToken, kRlcTokIdentifier))
			out->fArgName = argNameToken.content;
		else
			out->fArgName = kRlcSrcStringEmpty;
	} else if(rlc_parser_is_current(parser, kRlcTokBraceClose))
	{
		out->fType = kRlcDefaultConstructor;
	} else
	{
		out->fType = kRlcCustomConstructor;
		out->fArguments = NULL;
		out->fArgumentCount = 0;

		do {
			struct RlcParsedVariable argument;

			if(!rlc_parsed_variable_parse(
				&argument,
				parser,
				NULL,
				0,
				0,
				0,
				0,
				1))
			{
				rlc_parser_fail(parser, "expected argument");
			}

			rlc_parsed_constructor_add_argument(
				out,
				&argument);
		} while(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokComma));
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceClose);

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokInline))
	{
		out->fIsInline = 1;
	}

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokSemicolon))
	{
		out->fIsDefinition = 0;
		return 1;
	}

	out->fIsDefinition = 1;
	rlc_parsed_block_statement_create(&out->fBody);

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokMinusGreater))
	{
		do {
			struct RlcParsedInitialiser initialiser;

			rlc_parsed_initialiser_parse(
				&initialiser,
				parser);

			rlc_parsed_constructor_add_initialiser(
				out,
				&initialiser);
		} while(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokComma));
	}

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokColon))
	{
		do {
			struct RlcParsedInitialiser initialiser;

			rlc_parsed_initialiser_parse(
				&initialiser,
				parser);

			rlc_parsed_constructor_add_initialiser(
				out,
				&initialiser);
		} while(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokComma));
	}

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokSemicolon))
		return 1;

	if(!rlc_parsed_block_statement_parse(
		&out->fBody,
		parser))
		rlc_parser_fail(parser, "expected constructor body");

	return 1;
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

	this->fArguments = NULL;
	this->fArgumentCount = 0;
}


void rlc_parsed_initialiser_destroy(
	struct RlcParsedInitialiser * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_symbol_destroy(&this->fMember);

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

void rlc_parsed_initialiser_parse(
	struct RlcParsedInitialiser * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_parsed_initialiser_create(out);

	if(!rlc_parsed_symbol_parse(&out->fMember, parser, 0))
		rlc_parser_fail(parser, "expected symbol");

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokColonEqual))
	{
		rlc_parsed_initialiser_add_argument(
			out,
			rlc_parsed_expression_parse(
				parser,
				RLC_ALL_FLAGS(RlcParsedExpressionType)));
		return;
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokParentheseClose))
	{
		do {
			rlc_parsed_initialiser_add_argument(
				out,
				rlc_parsed_expression_parse(
					parser,
					RLC_ALL_FLAGS(RlcParsedExpressionType)));
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