#include "symbol.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_symbol_child_create(
	struct RlcParsedSymbolChild * this)
{
	RLC_DASSERT(this != NULL);

	this->fNameToken = 0;
	this->fTemplates = NULL;
	this->fTemplateCount = 0;
}

int rlc_parsed_symbol_child_parse(
	struct RlcParsedSymbolChild * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcParseError error_code;

	rlc_parsed_symbol_child_create(out);

	if(rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier))
	{
		out->fNameToken = rlc_parser_data_consumed_index(parser);
	} else
		goto nonfatal_failure;

	if(rlc_parser_data_consume(
		parser,
		kRlcTokBraceOpen))
	{
		struct RlcParsedExpression * expression;
		do {
			if(expression = rlc_parsed_expression_parse(
				parser,
				RLC_ALL_FLAGS(RlcParsedExpressionType)))
			{
				rlc_parsed_symbol_child_add_template(
					out,
					expression);
			} else
			{
				error_code = kRlcParseErrorExpectedExpression;
				goto failure;
			}
		} while(rlc_parser_data_consume(
			parser,
			kRlcTokComma));

		if(!rlc_parser_data_consume(
			parser,
			kRlcTokBraceClose))
		{
			error_code = kRlcParseErrorExpectedBraceClose;
			goto failure;
		}
	}

	return 1;

failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
nonfatal_failure:
	rlc_parsed_symbol_child_destroy(out);

	return 0;
}

void rlc_parsed_symbol_child_add_template(
	struct RlcParsedSymbolChild * this,
	struct RlcParsedExpression * template_argument)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(template_argument != NULL);

	rlc_realloc(
		(void**)&this->fTemplates,
		sizeof(struct RlcParsedExpression *) * ++ this->fTemplateCount);

	this->fTemplates[this->fTemplateCount-1] = template_argument;
}

void rlc_parsed_symbol_child_destroy(
	struct RlcParsedSymbolChild * this)
{
	RLC_DASSERT(this != NULL);

	this->fNameToken = 0;
	if(this->fTemplates)
	{
		for(size_t i = 0; i < this->fTemplateCount; i++)
		{
			rlc_parsed_expression_destroy_virtual(this->fTemplates[i]);
			rlc_free((void**)&this->fTemplates[i]);
		}
		rlc_free((void**)&this->fTemplates);
	}
	this->fTemplateCount = 0;
}

void rlc_parsed_symbol_destroy(
	struct RlcParsedSymbol * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fChildren)
		rlc_free((void**)&this->fChildren);

	this->fChildCount = 0;
	this->fIsRoot = 0;
}


void rlc_parsed_symbol_add_child(
	struct RlcParsedSymbol * this,
	struct RlcParsedSymbolChild * child)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(child != NULL);

	rlc_realloc(
		(void**)&this->fChildren,
		sizeof (struct RlcParsedSymbolChild) * ++ this->fChildCount);

	this->fChildren[this->fChildCount-1] = *child;
#ifdef RLC_DEBUG
	// invalidate the old data.
	rlc_parsed_symbol_child_create(child);
#endif
}

void rlc_parsed_symbol_create(
	struct RlcParsedSymbol * this)
{
	RLC_DASSERT(this != NULL);

	this->fChildren = NULL;
	this->fChildCount = 0;
	this->fIsRoot = 0;
}

int rlc_parsed_symbol_parse(
	struct RlcParsedSymbol * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);


	enum RlcParseError error_code;

	rlc_parsed_symbol_create(out);

	out->fIsRoot = rlc_parser_data_consume(
		parser,
		kRlcTokDoubleColon)
	? 1
	: 0;

	do {
		struct RlcParsedSymbolChild child;
		rlc_parsed_symbol_child_create(&child);

		if(rlc_parser_data_consume(
			parser,
			kRlcTokIdentifier))
		{
			child.fNameToken = rlc_parser_data_consumed_index(parser);
		} else
		{
			rlc_parsed_symbol_child_destroy(
				&child);

			if(out->fChildCount || out->fIsRoot)
			{
				error_code = kRlcParseErrorExpectedIdentifier;
				goto failure;
			}
			else
				goto nonfatal_failure;
		}

		if(rlc_parser_data_consume(
			parser,
			kRlcTokBraceOpen))
		{
			struct RlcParsedExpression * expression;
			do {
				if(expression = rlc_parsed_expression_parse(
					parser,
					RLC_ALL_FLAGS(RlcParsedExpressionType)))
				{
					rlc_parsed_symbol_child_add_template(
						&child,
						expression);
				} else
				{
					rlc_parsed_symbol_child_destroy(
						&child);
					error_code = kRlcParseErrorExpectedExpression;
					goto failure;
				}
			} while(rlc_parser_data_consume(
				parser,
				kRlcTokComma));

			if(!rlc_parser_data_consume(
				parser,
				kRlcTokBraceClose))
			{
				error_code = kRlcParseErrorExpectedBraceClose;
			}
		}

		rlc_parsed_symbol_add_child(
			out,
			&child);
	} while(rlc_parser_data_consume(
		parser,
		kRlcTokDoubleColon));

success:
	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
nonfatal_failure:
	rlc_parsed_symbol_destroy(
		out);
	return 0;
}