#include "namespace.h"

#include "../assert.h"

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
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokNamespace))
		return 0;

	struct RlcToken name;
	rlc_parser_expect(
		parser,
		&name,
		1,
		kRlcTokIdentifier);

	rlc_parsed_namespace_create(
		out,
		&name.content);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceOpen);

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

	return 1;
}