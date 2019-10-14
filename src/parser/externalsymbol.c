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
}

void rlc_parsed_external_symbol_destroy(
	struct RlcParsedExternalSymbol * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	rlc_parsed_type_name_destroy(&this->fType);
}

int rlc_parsed_external_symbol_parse(
	struct RlcParsedExternalSymbol * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokExtern))
		return 0;

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
			kRlcTokString);

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokBracketClose);
	}


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
		parser))
		rlc_parser_fail(parser, "expected type name");

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);

	return 1;
}