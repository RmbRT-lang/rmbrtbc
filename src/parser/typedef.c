#include "typedef.h"
#include "parser.h"
#include "../assert.h"
#include "../printer.h"


int rlc_parsed_typedef_parse(
	struct RlcParsedTypedef * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(templates != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokType))
	{
		return 0;
	}

	struct RlcToken name;
	rlc_parser_expect(
		parser,
		&name,
		1,
		kRlcTokIdentifier);

	rlc_parsed_typedef_create(out, &name.content, templates);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokColonEqual);

	if(!rlc_parsed_type_name_parse(
		&out->fType,
		parser,
		0))
	{
		rlc_parser_fail(parser, "exected type name");
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);

	return 1;
}

void rlc_parsed_typedef_create(
	struct RlcParsedTypedef * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);
	RLC_DASSERT(templates != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedTypedef,
		name);

	this->fTemplates = *templates;

	rlc_parsed_type_name_create(
		&this->fType);
}

void rlc_parsed_typedef_destroy(
	struct RlcParsedTypedef * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_type_name_destroy(
		&this->fType);

	rlc_parsed_template_decl_destroy(
		&this->fTemplates);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));
}


void rlc_parsed_member_typedef_create(
	struct RlcParsedMemberTypedef * this,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberTypedef,
		member);
}

void rlc_parsed_member_typedef_destroy(
	struct RlcParsedMemberTypedef * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_typedef_destroy(
		RLC_BASE_CAST(this, RlcParsedTypedef));

	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));
}

int rlc_parsed_member_typedef_parse(
	struct RlcParsedMemberTypedef * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * common)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(common != NULL);

	if(!rlc_parsed_typedef_parse(
		RLC_BASE_CAST(out, RlcParsedTypedef),
		parser,
		&common->templates))
	{
		return 0;
	}

	rlc_parsed_member_typedef_create(
		out,
		common);

	return 1;
}

void rlc_parsed_typedef_print(
	struct RlcParsedTypedef const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * printer)
{
	rlc_parsed_template_decl_print(&this->fTemplates, file, printer->fTypes);
	fputs("using ", printer->fTypes);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		printer->fTypes);
	fputs(" = ", printer->fTypes);
	rlc_parsed_type_name_print(&this->fType, file, printer->fTypes);
	fputs(";\n", printer->fTypes);
}