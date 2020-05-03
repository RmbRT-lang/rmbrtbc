#include "externalsymbol.h"
#include "parser.h"
#include "../assert.h"

void rlc_parsed_external_symbol_create(
	struct RlcParsedExternalSymbol * this,
	struct RlcToken const * linkname,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedExternalSymbol,
		name);

	this->fHasCustomLinkName = linkname != NULL;
	if(linkname)
		this->fCustomLinkName = *linkname;

	rlc_parsed_type_name_create(&this->fType);
	this->fIsFunction = 0;
}

void rlc_parsed_external_symbol_destroy(
	struct RlcParsedExternalSymbol * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	if(this->fIsFunction)
		rlc_parsed_function_destroy(&this->fFunction);
	else
		rlc_parsed_type_name_destroy(&this->fType);
}

int rlc_parsed_external_symbol_parse(
	struct RlcParsedExternalSymbol * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokExtern))
		return 0;

	if(rlc_parsed_template_decl_exists(templates))
		rlc_parser_fail(parser, "external symbols must not have templates");

	struct RlcToken linkname;
	int hasLinkName = 0;
	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBracketOpen))
	{
		hasLinkName = 1;
		rlc_parser_expect(
			parser,
			&linkname,
			1,
			kRlcTokStringLiteral);

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokBracketClose);
	}

	if((out->fIsFunction = rlc_parsed_function_parse(
		&out->fFunction,
		parser,
		templates,
		0)))
	{
		RLC_ASSERT(!hasLinkName);
		rlc_parsed_scope_entry_create(
			RLC_BASE_CAST(out, RlcParsedScopeEntry),
			kRlcParsedExternalSymbol,
			&RLC_BASE_CAST(&out->fFunction, RlcParsedScopeEntry)->fName);

		out->fHasCustomLinkName = 0;
		if(hasLinkName)
			out->fCustomLinkName = linkname;
	} else
	{
		struct RlcToken name;
		rlc_parser_expect(
			parser,
			&name,
			1,
			kRlcTokIdentifier);

		rlc_parsed_external_symbol_create(
			out,
			hasLinkName ? &linkname : NULL,
			&name.content);

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokColon);

		if(!rlc_parsed_type_name_parse(
			&out->fType,
			parser,
			0))
			rlc_parser_fail(parser, "expected type name");

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokSemicolon);
	}

	return 1;
}

void rlc_parsed_external_symbol_print(
	struct RlcParsedExternalSymbol const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * printer)
{
	RLC_ASSERT(!this->fHasCustomLinkName);
	FILE * out;
	if(this->fIsFunction)
	{
		out = printer->fFuncs;
		fputs("extern \"C\" ", out);
		rlc_parsed_function_print_head(&this->fFunction, file, out, 0);
	} else
	{
		out = printer->fVars;
		fputs("extern \"C\" ", out);
		rlc_parsed_type_name_print(&this->fType, file, out);
		fputc(' ', out);
		rlc_src_string_print(
			&RLC_BASE_CAST(
				this,
				RlcParsedScopeEntry)->fName,
			file,
			out);
	}
	fputs(";\n", out);
}