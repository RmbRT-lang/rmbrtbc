#ifndef __rlc_parser_lifetime_h_defined
#define __rlc_parser_lifetime_h_defined

#include "../src/string.h"
#include "blockstatement.h"
#include "parser.h"
#include "templatedecl.h"
#include "variable.h"

#include "../macros.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A constructor as used in the parser.
@extends RlcParsedMember */
struct RlcParsedConstructor
{
	RLC_DERIVE(struct,RlcParsedMember);

	/** The template parameters. */
	struct RlcParsedTemplateDecl fTemplates;

	/** The arguments. */
	struct RlcParsedVariable * fArguments;
	/** The argument count. */
	size_t fArgumentCount;

	/** Whether the constructor is a definition. */
	int fIsDefinition;

	/** The initialisers. */
	struct RlcParsedInitialiser * fInitialisers;
	/** The initialiser count. */
	size_t fInitialiserCount;

	/** The body statement. */
	struct RlcParsedBlockStatement fBody;
	/** Whether the constructor is inline. */
	int fIsInline;
};

/** Creates a constructor.
@memberof RlcParsedConstructor
@param[out] this:
	The constructor to create.
	@dassert @nonnull
@param[in] visibility:
	The constructor's visibility. */
void rlc_parsed_constructor_create(
	struct RlcParsedConstructor * this,
	enum RlcVisibility visibility);

/** Destroys a constructor.
@memberof RlcParsedConstructor
@param[in,out] this:
	The constructor to destroy.
	@dassert @nonnull */
void rlc_parsed_constructor_destroy(
	struct RlcParsedConstructor * this);

/** Parses a constructor.
@memberof RlcParsedConstructor
@param[out] out:
	The constructor to parse into.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_constructor_parse(
	struct RlcParsedConstructor * out,
	enum RlcVisibility visibility,
	struct RlcParser * parser);

/** Adds an argument to a constructor.
@memberof RlcParsedConstructor
@param[in,out] this:
	The constructor to add an argument to.
	@dassert @nonnull
@param[in] argument:
	The argument to add.
	@pass_ownership
	@dassert @nonnull */
void rlc_parsed_constructor_add_argument(
	struct RlcParsedConstructor * this,
	struct RlcParsedVariable * argument);

/** Adds an initialiser to a constructor.
@memberof RlcParsedConstructor
@param[in,out] this:
	The constructor to add an initialiser to.
	@dassert @nonnull
@param[in] initialiser:
	The initialiser to add.
	@pass_ownership
	@dassert @nonnull */
void rlc_parsed_constructor_add_initialiser(
	struct RlcParsedConstructor * this,
	struct RlcParsedInitialiser * initialiser);

/** A member initaliser inside a constructor as used by the parser.
@related RlcParsedConstructor */
struct RlcParsedInitialiser
{
	/** The member's name. */
	struct RlcSrcString fMember;
	/** The arguments. */
	struct RlcParsedExpression ** fArguments;
	/** The argument count. */
	size_t fArgumentCount;
};

/** Creates an initialiser.
@memberof RlcParsedInitialiser
@param[out] this:
	The initialiser to create.
	@dassert @nonnull */
void rlc_parsed_initialiser_create(
	struct RlcParsedInitialiser * this);

/** Destroys an initialiser.
@memberof RlcParsedInitialiser
@param[in,out] this:
	The initialiser to destroy.
	@dassert @nonnull */
void rlc_parsed_initialiser_destroy(
	struct RlcParsedInitialiser * this);

/** Parses an initialiser.
@memberof RlcParsedInitialiser
@param[out] out:
	The initialiser to parse into.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull */
void rlc_parsed_initialiser_parse(
	struct RlcParsedInitialiser * out,
	struct RlcParser * parser);

/** Adds an argument to an initaliser.
@memberof RlcParsedInitialiser
@param[in,out] this:
	The intialiser to add an argument to.
	@dassert @nonnul
@param[in] argument:
	The argument to add.
	@pass_pointer_ownership
	@dassert @nonnull */
void rlc_parsed_initialiser_add_argument(
	struct RlcParsedInitialiser * this,
	struct RlcParsedExpression * argument);

#ifdef __cplusplus
}
#endif

#endif