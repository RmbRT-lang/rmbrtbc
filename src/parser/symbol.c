#include "symbol.h"
#include "typename.h"

#include "../malloc.h"
#include "../assert.h"
#include "../printer.h"

void rlc_parsed_symbol_child_create(
	struct RlcParsedSymbolChild * this)
{
	RLC_DASSERT(this != NULL);

	this->fTemplates = NULL;
	this->fTemplateCount = 0;
}

static int rlc_parsed_symbol_child_template_parse(
	struct RlcParsedSymbolChild * out,
	struct RlcParser * parser)
{
	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBracketOpen))
		return 0;

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBracketClose))
		return 1;

	struct RlcParsedSymbolChildTemplate template;
	do {
		template.fSize = 0;
		template.fExpressions = NULL;

		if(rlc_parser_is_current(parser, kRlcTokSemicolon))
		{
			template.fIsExpression = 0;
		} else if((template.fIsExpression = rlc_parser_consume(
			parser,
			NULL,
			kRlcTokHash)))
		{
			do {
				rlc_realloc(
					(void**)&template.fExpressions,
					sizeof(struct RlcParsedExpression *) * ++template.fSize);
				if(!(template.fExpressions[template.fSize-1]
					= rlc_parsed_expression_parse(
						parser,
						RLC_ALL_FLAGS(RlcParsedExpressionType))))
				{
					rlc_parser_fail(parser, "expected expression");
				}
			} while(rlc_parser_consume(
				parser,
				NULL,
				kRlcTokComma));
		} else
		{
			do {
				rlc_realloc(
					(void**)&template.fTypeNames,
					sizeof(struct RlcParsedTypeName) * ++template.fSize);

				if(!rlc_parsed_type_name_parse(
					&template.fTypeNames[template.fSize-1],
					parser,
					0))
				{
					rlc_parser_fail(parser, "expected type name");
				}
			} while(rlc_parser_consume(
				parser,
				NULL,
				kRlcTokComma));
		}
		rlc_parsed_symbol_child_add_template(
			out,
			&template);
	} while(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokSemicolon));

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBracketClose);

	return 1;
}

int rlc_parsed_symbol_child_parse(
	struct RlcParsedSymbolChild * out,
	struct RlcParser * parser,
	int allowSpecialIdentifiers)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_parsed_symbol_child_create(out);
	int hasTemplate = rlc_parsed_symbol_child_template_parse(out, parser);

	struct RlcToken name;
	if(rlc_parser_consume(
		parser,
		&name,
		kRlcTokIdentifier))
	{
		out->fType = kRlcParsedSymbolChildTypeIdentifier;
	} else if(allowSpecialIdentifiers
	&& !hasTemplate
	&& rlc_parser_consume(
		parser,
		&name,
		kRlcTokDestructor))
	{
		out->fType = kRlcParsedSymbolChildTypeDestructor;
	} else if(hasTemplate)
	{
		rlc_parser_fail(parser, "expected name");
	} else
		return 0;

	out->fName = name.content;

	return 1;
}

void rlc_parsed_symbol_child_add_template(
	struct RlcParsedSymbolChild * this,
	struct RlcParsedSymbolChildTemplate * template_argument)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(template_argument != NULL);

	rlc_realloc(
		(void**)&this->fTemplates,
		sizeof(struct RlcParsedSymbolChildTemplate) * ++ this->fTemplateCount);

	this->fTemplates[this->fTemplateCount-1] = *template_argument;
}

void rlc_parsed_symbol_child_destroy(
	struct RlcParsedSymbolChild * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fTemplates)
	{
		for(size_t i = 0; i < this->fTemplateCount; i++)
		{
			for(RlcSrcIndex j = 0; j < this->fTemplates[i].fSize; j++)
			{
				if(this->fTemplates[i].fIsExpression)
				{
					rlc_parsed_expression_destroy_virtual(
						this->fTemplates[i].fExpressions[j]);
					rlc_free((void**)&this->fTemplates[i].fExpressions[j]);
				}
				else
				{
					rlc_parsed_type_name_destroy(
						&this->fTemplates[i].fTypeNames[j]);
				}
			}
			if(this->fTemplates[i].fTypeNames)
				rlc_free((void**)&this->fTemplates[i].fTypeNames);
		}
		rlc_free((void**)&this->fTemplates);
	}
	this->fTemplateCount = 0;
}

void rlc_parsed_symbol_child_print(
	struct RlcParsedSymbolChild const * this,
	struct RlcSrcFile const * file,
	FILE * out,
	int templateAllowed)
{
	if(this->fTemplateCount && templateAllowed)
		fprintf(out, "template ");
	switch(this->fType)
	{
	case kRlcParsedSymbolChildTypeIdentifier:
		{
			rlc_src_string_print(&this->fName, file, out);
		} break;
	case kRlcParsedSymbolChildTypeConstructor:
		{
			fprintf(out, " __rl_constructor ");
		} break;
	case kRlcParsedSymbolChildTypeDestructor:
		{
			fprintf(out, " __rl_destructor ");
		} break;
	}

	if(this->fTemplateCount)
	{
		fputc('<', out);

		for(RlcSrcIndex i = 0; i < this->fTemplateCount; i++)
		{
			if(i)
				fputs(", ", out);
			for(RlcSrcIndex j = 0; j < this->fTemplates[i].fSize; j++)
			{
				if(j)
					fputs(", ", out);
				if(this->fTemplates[i].fIsExpression)
				{
					rlc_parsed_expression_print(
						this->fTemplates[i].fExpressions[j],
						file,
						out);
				} else
				{
					rlc_parsed_type_name_print(
						&this->fTemplates[i].fTypeNames[j],
						file,
						out);
				}
			}
		}

		fputc('>', out);
	}
}

void rlc_parsed_symbol_destroy(
	struct RlcParsedSymbol * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fChildren)
	{
		for(size_t i = 0; i < this->fChildCount; i++)
			rlc_parsed_symbol_child_destroy(&this->fChildren[i]);
		rlc_free((void**)&this->fChildren);
	}

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
	struct RlcParser * parser,
	int allowSpecialIdentifiers)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_parsed_symbol_create(out);

	out->fIsRoot = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokDoubleColon);

	int parsed_any = 0;

	do {
		struct RlcParsedSymbolChild child;
		if(rlc_parsed_symbol_child_parse(
			&child,
			parser,
			allowSpecialIdentifiers))
		{
			parsed_any = 1;
			rlc_parsed_symbol_add_child(
				out,
				&child);
		} else if(parsed_any || out->fIsRoot)
		{
			rlc_parser_fail(parser, "expected symbol");
		}
	} while(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokDoubleColon));

	return parsed_any;
}


void rlc_parsed_symbol_print(
	struct RlcParsedSymbol const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(this->fIsRoot)
		fprintf(out, "::");

	for(RlcSrcIndex i = 0; i < this->fChildCount; i++)
	{
		if(i)
			fprintf(out, "::");

		rlc_parsed_symbol_child_print(
			&this->fChildren[i],
			file,
			out,
			i);
	}
}

void rlc_parsed_symbol_print_no_template(
	struct RlcParsedSymbol const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(this->fIsRoot)
		fprintf(out, "::");

	for(RlcSrcIndex i = 0; i < this->fChildCount; i++)
	{
		if(i)
			fprintf(out, "::");

		rlc_parsed_symbol_child_print(
			&this->fChildren[i],
			file,
			out,
			0);
	}
}