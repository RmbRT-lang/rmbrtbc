#include "constructor.h"
#include "../assert.h"
#include "../malloc.h"
#include "symbolconstantexpression.h"

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

	this->fCallsOtherCtor = 0;
	this->fType = kRlcDefaultConstructor;
	this->fIsDefinition = 0;
	this->fInit.fInits.fBaseInits = NULL;
	this->fInit.fInits.fBaseInitCount = 0;
	this->fInit.fInits.fInitialisers = NULL;
	this->fInit.fInits.fInitialiserCount = 0;
}

void rlc_parsed_constructor_destroy(
	struct RlcParsedConstructor * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_destroy_base(RLC_BASE_CAST(this, RlcParsedMember));

	rlc_parsed_template_decl_destroy(&this->fTemplates);

	if(this->fType == kRlcCustomConstructor)
	{
		if(this->fIsVariant)
		{
			rlc_parsed_symbol_constant_type_destroy(&this->fVariant);
			this->fIsVariant = 0;
		}
		for(size_t i = this->fArgumentCount; i--;)
			rlc_parsed_variable_destroy(&this->fArguments[i]);
		if(this->fArguments)
			rlc_free((void**)&this->fArguments);
		this->fArgumentCount = 0;
	}

	if(this->fCallsOtherCtor)
	{
		if(this->fInit.fOtherCtorCallArgs)
		{
			for(RlcSrcIndex i = 0; i < this->fInit.fOtherCtorCallArgCount; i++)
			{
				rlc_parsed_expression_destroy_virtual(this->fInit.fOtherCtorCallArgs[i]);
				rlc_free((void**)&this->fInit.fOtherCtorCallArgs[i]);
			}

			rlc_free((void**)&this->fInit.fOtherCtorCallArgs);
		}
	}
	else
	{
		if(this->fInit.fInits.fBaseInits)
		{
			for(size_t i = this->fInit.fInits.fBaseInitCount; i--;)
				rlc_parsed_base_init_destroy(&this->fInit.fInits.fBaseInits[i]);
			rlc_free((void**)&this->fInit.fInits.fBaseInits);
			this->fInit.fInits.fBaseInitCount = 0;
		}
		if(this->fInit.fInits.fInitialisers)
		{
			for(size_t i = this->fInit.fInits.fInitialiserCount; i--;)
				rlc_parsed_initialiser_destroy(&this->fInit.fInits.fInitialisers[i]);
			rlc_free((void**)&this->fInit.fInits.fInitialisers);
			this->fInit.fInits.fInitialiserCount = 0;
		}
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

	if(member->attribute != kRlcMemberAttributeNone)
		return 0;

	rlc_parsed_constructor_create(
		out,
		member);

	out->fIsVariant = rlc_parsed_symbol_constant_type_parse(
		&out->fVariant, parser);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBraceOpen))
		return 0;

	struct RlcToken argNameToken;
	if(!out->fIsVariant && rlc_parser_consume(parser, NULL, kRlcTokDoubleAnd))
	{
		out->fType = kRlcMoveConstructor;
		if(rlc_parser_consume(parser, &argNameToken, kRlcTokIdentifier))
			out->fArgName = argNameToken.content;
		else
			out->fArgName = kRlcSrcStringEmpty;
	}
	else if(!out->fIsVariant && rlc_parser_consume(parser, NULL, kRlcTokHash))
	{
		rlc_parser_expect(parser, NULL, 1, kRlcTokAnd);
		out->fType = kRlcCopyConstructor;
		if(rlc_parser_consume(parser, &argNameToken, kRlcTokIdentifier))
			out->fArgName = argNameToken.content;
		else
			out->fArgName = kRlcSrcStringEmpty;
	} else if(!out->fIsVariant && rlc_parser_is_current(parser, kRlcTokBraceClose))
	{
		out->fType = kRlcDefaultConstructor;
	} else if(!out->fIsVariant && rlc_parser_consume(parser, NULL, kRlcTokTripleDot))
	{
		out->fType = kRlcStructuralConstructor;
	} else if (!out->fIsVariant && rlc_parser_consume(parser, NULL, kRlcTokBare))
	{
		out->fType = kRlcBareConstructor;
	} else
	{
		out->fType = kRlcCustomConstructor;
		out->fArguments = NULL;
		out->fArgumentCount = 0;

		if(!out->fIsVariant || !rlc_parser_is_current(parser, kRlcTokBraceClose))
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

	if(out->fType == kRlcStructuralConstructor)
	{
		rlc_parser_expect(parser, NULL, 1, kRlcTokSemicolon);
		return 1;
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

	if((out->fCallsOtherCtor = rlc_parser_consume(
			parser, NULL, kRlcTokColonEqual)))
	{
		out->fInit.fOtherCtorCallArgCount = 1;
		rlc_malloc((void**)&out->fInit.fOtherCtorCallArgs, sizeof(void*));
		if(!(out->fInit.fOtherCtorCallArgs[0] = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType))))
			rlc_parser_fail(parser, "expected expression");
	} else if((out->fCallsOtherCtor = rlc_parser_consume(
		parser, NULL, kRlcTokParentheseOpen)))
	{
		if(!rlc_parser_consume(parser, NULL, kRlcTokParentheseClose))
		{
			do
			{
				struct RlcParsedExpression * expr = rlc_parsed_expression_parse(
					parser,
					RLC_ALL_FLAGS(RlcParsedExpressionType));
				if(!expr)
					rlc_parser_fail(parser, "expected expression");
				rlc_realloc(
					(void**)&out->fInit.fOtherCtorCallArgs,
					sizeof(void*) * ++out->fInit.fOtherCtorCallArgCount);
				out->fInit.fOtherCtorCallArgs[out->fInit.fOtherCtorCallArgCount-1] = expr;
			} while(kRlcTokComma == rlc_parser_expect(
				parser, NULL,
				2, kRlcTokComma, kRlcTokParentheseClose));
		}
	} else
	{
		if(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokMinusGreater))
		{
			do {
				struct RlcParsedBaseInit initialiser;

				rlc_parsed_base_init_parse(
					&initialiser,
					parser);

				rlc_parsed_constructor_add_base_init(
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

void rlc_parsed_constructor_add_base_init(
	struct RlcParsedConstructor * this,
	struct RlcParsedBaseInit * base_init)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(base_init != NULL);

	rlc_realloc(
		(void**)&this->fInit.fInits.fBaseInits,
		sizeof(struct RlcParsedBaseInit) * ++this->fInit.fInits.fBaseInitCount);

	this->fInit.fInits.fBaseInits[this->fInit.fInits.fBaseInitCount-1] = *base_init;
}

void rlc_parsed_constructor_add_initialiser(
	struct RlcParsedConstructor * this,
	struct RlcParsedInitialiser * initialiser)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(initialiser != NULL);

	rlc_realloc(
		(void**)&this->fInit.fInits.fInitialisers,
		sizeof(struct RlcParsedInitialiser) * ++this->fInit.fInits.fInitialiserCount);

	this->fInit.fInits.fInitialisers[this->fInit.fInits.fInitialiserCount-1] = *initialiser;
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

	struct RlcToken name;
	rlc_parser_expect(parser, &name, 1, kRlcTokIdentifier);
	out->fMember = name.content;

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokColonEqual))
	{
		if(rlc_parser_consume(parser, NULL, kRlcTokNoinit))
		{
			out->fInitType = kRlcInitTypeNoInit;
		} else if(rlc_parser_consume(parser, NULL, kRlcTokBare))
		{
			out->fInitType = kRlcInitTypeBare;
		} else
		{
			out->fInitType = kRlcInitTypeArguments;
			struct RlcParsedExpression * arg = rlc_parsed_expression_parse(
				parser,
				RLC_ALL_FLAGS(RlcParsedExpressionType));
			if(!arg)
				rlc_parser_fail(parser, "expected expression or 'NOINIT'");

			rlc_parsed_initialiser_add_argument(out, arg);
		}
		return;
	}

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
		out->fInitType = kRlcInitTypeArguments;
	} else
	{
		if(rlc_parser_consume(parser, NULL, kRlcTokNoinit))
		{
			out->fInitType = kRlcInitTypeNoInit;
		} else if(rlc_parser_consume(parser, NULL, kRlcTokBare))
		{
			out->fInitType = kRlcInitTypeBare;
		} else
		{
			out->fInitType = kRlcInitTypeArguments;
			do {
				struct RlcParsedExpression * arg = rlc_parsed_expression_parse(
					parser,
					RLC_ALL_FLAGS(RlcParsedExpressionType));
				if(!arg)
					rlc_parser_fail(parser, "expected expression");

				rlc_parsed_initialiser_add_argument(out, arg);
			} while(rlc_parser_consume(
				parser,
				NULL,
				kRlcTokComma));
		}

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



void rlc_parsed_base_init_create(
	struct RlcParsedBaseInit * this)
{
	RLC_DASSERT(this != NULL);

	this->fArguments = NULL;
	this->fArgumentCount = 0;
}


void rlc_parsed_base_init_destroy(
	struct RlcParsedBaseInit * this)
{
	RLC_DASSERT(this != NULL);

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

void rlc_parsed_base_init_parse(
	struct RlcParsedBaseInit * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_parsed_base_init_create(out);

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
		if(rlc_parser_consume(parser, NULL, kRlcTokNoinit))
		{
			out->fInitType = kRlcInitTypeNoInit;
		} else if(rlc_parser_consume(parser, NULL, kRlcTokBare))
		{
			out->fInitType = kRlcInitTypeBare;
		} else
		{
			out->fInitType = kRlcInitTypeArguments;
			do {
				struct RlcParsedExpression * arg = rlc_parsed_expression_parse(
					parser,
					RLC_ALL_FLAGS(RlcParsedExpressionType));
				if(!arg)
					rlc_parser_fail(parser, "expected expression");

				rlc_parsed_base_init_add_argument(out, arg);
			} while(rlc_parser_consume(
				parser,
				NULL,
				kRlcTokComma));
		}

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokParentheseClose);
	} else {
		out->fInitType = kRlcInitTypeArguments;
	}
}

void rlc_parsed_base_init_add_argument(
	struct RlcParsedBaseInit * this,
	struct RlcParsedExpression * argument)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(argument != NULL);

	rlc_realloc(
		(void**)&this->fArguments,
		sizeof(struct RlcParsedExpression*) * ++this->fArgumentCount);

	this->fArguments[this->fArgumentCount-1] = argument;
}