#include "enum.h"

#include "../tokeniser/tokens.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_enum_constant_create(
	struct RlcParsedEnumConstant * this)
{
	RLC_DASSERT(this != NULL);

	this->fNameTokens = NULL;
	this->fNameCount = 0;
}

void rlc_parsed_enum_constant_add_name(
	struct RlcParsedEnumConstant * this,
	struct RlcSrcString const * nameToken)
{
	RLC_DASSERT(this != NULL);

	rlc_realloc(
		(void**)&this->fNameTokens,
		sizeof(*this->fNameTokens) * ++this->fNameCount);

	this->fNameTokens[this->fNameCount-1] = *nameToken;
}

void rlc_parsed_enum_constant_destroy(
	struct RlcParsedEnumConstant * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fNameTokens)
	{
		rlc_free((void**)&this->fNameTokens);
		this->fNameCount = 0;
	}
}

int rlc_parsed_enum_constant_parse(
	struct RlcParsedEnumConstant * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	rlc_parsed_enum_constant_create(out);

	do {
		struct RlcToken name;
		rlc_parser_expect(
			parser,
			&name,
			1,
			kRlcTokIdentifier);

		rlc_parsed_enum_constant_add_name(
			out,
			&name.content);
	} while(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokColonEqual));

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
		rlc_parsed_enum_constant_parse(
			&constant,
			parser);

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