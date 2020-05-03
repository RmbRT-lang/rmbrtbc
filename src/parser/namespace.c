#include "namespace.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_namespace_create(
	struct RlcParsedNamespace * this,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedNamespace,
		name);
	rlc_parsed_scope_entry_list_create(&this->fEntryList);
}

void rlc_parsed_namespace_destroy(
	struct RlcParsedNamespace * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_list_destroy(&this->fEntryList);
	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));
}

int rlc_parsed_namespace_parse(
	struct RlcParsedNamespace * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokDoubleColon))
		return 0;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "namespace", &tracer);

	if(rlc_parsed_template_decl_exists(templates))
		rlc_parser_fail(parser, "namespaces must not have template declarations");

	struct RlcToken name;
	rlc_parser_expect(
		parser,
		&name,
		1,
		kRlcTokIdentifier);

	rlc_parsed_namespace_create(
		out,
		&name.content);


	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBraceOpen))
	{
		for(struct RlcParsedScopeEntry * scopeEntry;
			(scopeEntry = rlc_parsed_scope_entry_parse(parser));)
		{
			rlc_parsed_scope_entry_list_add(
				&out->fEntryList,
				scopeEntry);
		}

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokBraceClose);
	} else
	{
		struct RlcParsedScopeEntry * inner;
		if(!(inner = rlc_parsed_scope_entry_parse(parser)))
			rlc_parser_fail(parser, "expected scope entry");

		rlc_parsed_scope_entry_list_add(
			&out->fEntryList,
			inner);
	}

	rlc_parser_untrace(parser, &tracer);
	return 1;
}

void rlc_parsed_namespace_print(
	struct RlcParsedNamespace const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * printer)
{
	FILE * out;
	for(int i = 0; i<6; i++)
	{
		switch(i)
		{
		case 0: out = printer->fTypes; break;
		case 1: out = printer->fVars; break;
		case 2: out = printer->fFuncs; break;
		case 3: out = printer->fTypesImpl; break;
		case 4: out = printer->fVarsImpl; break;
		case 5: out = printer->fFuncsImpl; break;
		}
		fprintf(out, "namespace ");
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fprintf(out, " {\n");
	}

	rlc_parsed_scope_entry_list_print(
		&this->fEntryList,
		file,
		printer);


	for(int i = 0; i < 6; i++)
	{
		switch(i)
		{
		case 0: out = printer->fTypes; break;
		case 1: out = printer->fVars; break;
		case 2: out = printer->fFuncs; break;
		case 3: out = printer->fTypesImpl; break;
		case 4: out = printer->fVarsImpl; break;
		case 5: out = printer->fFuncsImpl; break;
		}
		fputs("}\n", out);
	}
}