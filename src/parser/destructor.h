#ifndef __rlc_parser_destructor_h_defined
#define __rlc_parser_destructor_h_defined

#include "blockstatement.h"
#include "member.h"
#include "parser.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/** A destructor as used by the parser. */
struct RlcParsedDestructor
{
	RLC_DERIVE(struct,RlcParsedMember);

	/** Whether the destructor is a definition. */
	int fIsDefinition;
	/** Whether the destructor is inline. */
	int fIsInline;
	/** The destructor's function body. */
	struct RlcParsedBlockStatement fBody;
};

/** Creates a destructor.
@memberof RlcParsedDestructor
@param[out] this:
	The destructor to create.
	@dassert @nonnull
@param[in] visibility:
	The destructor's visibility. */
void rlc_parsed_destructor_create(
	struct RlcParsedDestructor * this,
	struct RlcParsedMemberCommon const * member);

/** Destroys a destructor.
@memberof RlcParsedDestructor
@param[in,out] this:
	The destructor to destroy.
	@dassert @nonnull */
void rlc_parsed_destructor_destroy(
	struct RlcParsedDestructor * this);

/** Parses a destructor.
@memberof RlcParsedDestructor
@param[out] out:
	The destructor to parse into.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_destructor_parse(
	struct RlcParsedDestructor * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member);

#ifdef __cplusplus
}
#endif

#endif