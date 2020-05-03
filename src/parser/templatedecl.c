#include "templatedecl.h"

#include "../printer.h"
#include "../malloc.h"
#include "../assert.h"

struct RlcParsedTemplateDecl const kRlcParsedTemplateDeclNone = {
	NULL, 0
};

void rlc_parsed_template_decl_create(
	struct RlcParsedTemplateDecl * this)
{
	RLC_DASSERT(this != NULL);

	this->fChildren = NULL;
	this->fChildCount = 0;
}

void rlc_parsed_template_decl_add_child(
	struct RlcParsedTemplateDecl * this,
	struct RlcParsedTemplateDeclChild const * child)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(child != NULL);

	rlc_realloc(
		(void**)&this->fChildren,
		sizeof(struct RlcParsedTemplateDeclChild) * ++ this->fChildCount);

	this->fChildren[this->fChildCount-1] = *child;
}

void rlc_parsed_template_decl_destroy(
	struct RlcParsedTemplateDecl * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fChildren)
	{
		for(size_t i = 0; i < this->fChildCount; i++)
			rlc_parsed_template_decl_child_destroy(&this->fChildren[i]);
		rlc_free((void**)&this->fChildren);
		this->fChildCount = 0;
	}
}

void rlc_parsed_template_decl_parse(
	struct RlcParsedTemplateDecl * decl,
	struct RlcParser * parser)
{
	RLC_DASSERT(decl != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_parsed_template_decl_create(decl);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBracketOpen))
		return;

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBracketClose))
		return;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "template declaration", &tracer);

	do
	{
		struct RlcParsedTemplateDeclChild child;
		struct RlcToken name;
		rlc_parser_expect(
			parser,
			&name,
			1,
			kRlcTokIdentifier);

		child.fName = name.content;

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokColon);

		// Expect either `type`, `number`, or a type name.
		if(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokType))
		{
			child.fType = kRlcParsedTemplateDeclTypeType;
		} else if(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokNumber))
		{
			child.fType = kRlcParsedTemplateDeclTypeNumber;
		} else if(rlc_parsed_type_name_parse(
			&child.fValueType,
			parser,
			0))
		{
			child.fType = kRlcParsedTemplateDeclTypeValue;
		} else
			rlc_parser_fail(parser, "expected template type");

		rlc_parsed_template_decl_add_child(
			decl,
			&child);
	} while(kRlcTokComma == rlc_parser_expect(
		parser,
		NULL,
		2,
		kRlcTokComma,
		kRlcTokBracketClose));

	rlc_parser_untrace(parser, &tracer);
}

void rlc_parsed_template_decl_child_destroy(
	struct RlcParsedTemplateDeclChild * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(this->fType, RlcParsedTemplateDeclType));

	if(this->fType == kRlcParsedTemplateDeclTypeValue)
	{
		rlc_parsed_type_name_destroy(&this->fValueType);
		this->fType = kRlcParsedTemplateDeclTypeNumber;
	}
}

void rlc_parsed_template_decl_print(
	struct RlcParsedTemplateDecl const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(!this->fChildCount)
		return;

	fprintf(out, "template<");

	for(RlcSrcIndex i = 0; i < this->fChildCount; i++)
	{
		if(i)
			fprintf(out, ", ");

		switch(this->fChildren[i].fType)
		{
		case kRlcParsedTemplateDeclTypeType:
			{
				fputs("class ", out);
				rlc_src_string_print(&this->fChildren[i].fName, file, out);
			} break;
		case kRlcParsedTemplateDeclTypeNumber:
			{
				fprintf(out, "int ");
				rlc_src_string_print(&this->fChildren[i].fName, file, out);
			} break;
		case kRlcParsedTemplateDeclTypeValue:
		default:
			RLC_ASSERT(!"not implemented");
		}
	}

	fprintf(out, "> ");
}