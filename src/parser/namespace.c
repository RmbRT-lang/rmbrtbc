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

	// Nested namespace?
	if(rlc_parser_is_current(
		parser,
		kRlcTokDoubleColon))
	{
		struct RlcParsedNamespace * inner = NULL;
		rlc_malloc(
			(void**)&inner,
			sizeof(struct RlcParsedNamespace));

		RLC_ASSERT(rlc_parsed_namespace_parse(inner, parser, templates));

		rlc_parsed_scope_entry_list_add(
			&out->fEntryList,
			RLC_BASE_CAST(inner, RlcParsedScopeEntry));
	} else
	{
		rlc_parser_expect(
			parser,
			NULL,
			2,
			kRlcTokBraceOpen,
			kRlcTokDoubleColon);

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
	}

	rlc_parser_untrace(parser, &tracer);
	return 1;
}