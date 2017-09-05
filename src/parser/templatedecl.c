#include "templatedecl.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_template_decl_create(
	struct RlcTemplateDecl * this)
{
	RLC_DASSERT(this != NULL);

	this->fChildren = NULL;
	this->fChildCount = 0;
}

void rlc_template_decl_add_child(
	struct RlcTemplateDecl * this,
	struct RlcTemplateDeclChild const * child)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(child != NULL);

	rlc_realloc(
		(void**)&this->fChildren,
		sizeof(struct RlcTemplateDeclChild) * ++ this->fChildCount);

	this->fChildren[this->fChildCount-1] = *child;
}

void rlc_template_decl_destroy(
	struct RlcTemplateDecl * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fChildren)
	{
		rlc_free((void**)&this->fChildren);
		this->fChildCount = 0;
	}
}

int rlc_template_decl_parse(
	struct RlcTemplateDecl * decl,
	struct RlcParserData * parser)
{
	RLC_DASSERT(decl != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcParseError error_code;

	rlc_template_decl_create(decl);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceOpen))
	{
		return 1;
	}

	int first = 1;
	do
	{
		struct RlcTemplateDeclChild child;

		if(!rlc_parser_data_consume(
			parser,
			kRlcTokIdentifier))
		{
			if(!first)
			{
				error_code = kRlcParseErrorExpectedIdentifier;
				goto failure;
			}
			else
				break;
		}

		if(first)
			first = 0;

		child.fNameToken = rlc_parser_data_consumed_index(parser);

		if(!rlc_parser_data_consume(
			parser,
			kRlcTokColon))
		{
			error_code = kRlcParseErrorExpectedColon;
			goto failure;
		}

		if(rlc_parser_data_consume(
			parser,
			kRlcTokType))
		{
			child.fType = kRlcTemplateDeclTypeType;
		} else if(rlc_parser_data_consume(
			parser,
			kRlcTokNumber))
		{
			child.fType = kRlcTemplateDeclTypeNumber;
		} else
		{
			error_code = kRlcParseErrorExpectedTemplateDeclType;
			goto failure;
		}

		rlc_template_decl_add_child(
			decl,
			&child);
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

success:
	return 1;
failure:
	rlc_template_decl_destroy(decl);
	rlc_parser_data_add_error(
		parser,
		error_code);
	return 0;
}