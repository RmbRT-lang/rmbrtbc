#include "enum.h"

#include "../tokeniser/tokens.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_enum_constant_create(
	struct RlcParsedEnumConstant * this,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedEnumConstant,
		name);

	this->fAliasTokens = NULL;
	this->fAliasCount = 0;
}

void rlc_parsed_enum_constant_add_name(
	struct RlcParsedEnumConstant * this,
	struct RlcSrcString const * nameToken)
{
	RLC_DASSERT(this != NULL);

	rlc_realloc(
		(void**)&this->fAliasTokens,
		sizeof(*this->fAliasTokens) * ++this->fAliasCount);

	this->fAliasTokens[this->fAliasCount-1] = *nameToken;
}

void rlc_parsed_enum_constant_destroy(
	struct RlcParsedEnumConstant * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	if(this->fAliasTokens)
	{
		rlc_free((void**)&this->fAliasTokens);
		this->fAliasCount = 0;
	}
}

int rlc_parsed_enum_constant_parse(
	struct RlcParsedEnumConstant * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	struct RlcToken name;
	if(!rlc_parser_consume(
		parser,
		&name,
		kRlcTokIdentifier))
		return 0;

	rlc_parsed_enum_constant_create(out, &name.content);

	while(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokColonEqual))
	{
		rlc_parser_expect(
			parser,
			&name,
			1,
			kRlcTokIdentifier);

		rlc_parsed_enum_constant_add_name(
			out,
			&name.content);
	}

	return 1;
}

void rlc_parsed_enum_create(
	struct RlcParsedEnum * this,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedEnum,
		name);

	this->fConstants = NULL;
	this->fConstantCount = 0;
}

void rlc_parsed_enum_destroy(
	struct RlcParsedEnum * this)
{
	RLC_DASSERT(this != NULL);

	for(size_t i = 0; i < this->fConstantCount; i++)
		rlc_parsed_enum_constant_destroy(
			&this->fConstants[i]);

	if(this->fConstants)
	{
		rlc_free((void**)&this->fConstants);
		this->fConstantCount = 0;
	}
}

void rlc_parsed_enum_add_constant(
	struct RlcParsedEnum * this,
	struct RlcParsedEnumConstant const * constant)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(constant != NULL);

	rlc_realloc(
		(void**)&this->fConstants,
		sizeof(struct RlcParsedEnumConstant) * ++this->fConstantCount);
	this->fConstants[this->fConstantCount-1] = *constant;
}

int rlc_parsed_enum_parse(
	struct RlcParsedEnum * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokEnum))
		return 0;

	if(rlc_parsed_template_decl_exists(templates))
		rlc_parser_fail(parser, "enums must not have template declarations");

	struct RlcToken name;
	rlc_parser_expect(
		parser,
		&name,
		1,
		kRlcTokIdentifier);

	rlc_parsed_enum_create(
		out,
		&name.content);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceOpen);

	do {
		struct RlcParsedEnumConstant constant;
		if(!rlc_parsed_enum_constant_parse(
			&constant,
			parser))
			rlc_parser_fail(parser, "expected enum constant");

		rlc_parsed_enum_add_constant(
			out,
			&constant);

	} while(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokComma));

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceClose);

	return 1;
}