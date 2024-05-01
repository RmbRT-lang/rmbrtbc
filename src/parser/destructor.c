#include "destructor.h"

#include "../assert.h"

void rlc_parsed_destructor_create(
	struct RlcParsedDestructor * this,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(member != NULL);
	RLC_DASSERT(member->attribute == kRlcMemberAttributeNone);
	RLC_DASSERT(!rlc_parsed_template_decl_exists(&member->templates));

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedDestructor,
		member);

	this->fIsDefinition = 0;
	this->fIsInline = 0;
}


void rlc_parsed_destructor_destroy(
	struct RlcParsedDestructor * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fIsDefinition)
	{
		rlc_parsed_block_statement_destroy(&this->fBody);
		this->fIsDefinition = 0;
	}
}

int rlc_parsed_destructor_parse(
	struct RlcParsedDestructor * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);


	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokDestructor))
		return 0;

	rlc_parsed_destructor_create(out, member);

	out->fIsInline = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokInline)
	|| rlc_parser_consume(
		parser,
		NULL,
		RLC_INLINE_SHORTHAND);

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokSemicolon))
	{
		return 1;
	}

	if(!rlc_parsed_block_statement_parse(
		&out->fBody,
		parser))
	{
		rlc_parser_fail(parser, "expected destructor body");
	}

	out->fIsDefinition = 1;

	return 1;
}