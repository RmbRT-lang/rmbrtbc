/** @file mask.h
	Contains the definition for the mask type used by the parser. */
#ifndef __rlc_parser_mask_h_defined
#define __rlc_parser_mask_h_defined

#include "function.h"
#include "templatedecl.h"

/** A mask is a set of member functions that an object needs to have to fulfill the mask
@extends RlcParsedScopeEntry */
struct RlcParsedMask
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The mask's template declarations. */
	struct RlcParsedTemplateDecl fTemplates;

	/** The mask's functions. */
	struct RlcParsedMemberFunction * fFunctions;
	/** The number of functions the mask has. */
	RlcSrcSize fFunctionCount;
};

/** Creates a parsed mask.
@memberof RlcParsedMask
@param[out] this:
	The parsed mask to create.
	@dassert @nonnull
@param[in] name:
	The mask's name.
	@dassert @nonnull
@param[in] templates:
	The mask's templates. */
void rlc_parsed_mask_create(
	struct RlcParsedMask * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates);

/** Destroys a parsed mask.
@memberof RlcParsedMask
@param[in,out] this:
	The parsed mask to destroy.
	@dassert @nonnull */
void rlc_parsed_mask_destroy(
	struct RlcParsedMask * this);

/** Parses a mask.
@memberof RlcParsedMask
@param[out] out:
	The mask to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[in] templates:
	The mask's templates.
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_mask_parse(
	struct RlcParsedMask * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates);

/** Prints a parsed mask.
@memberof RlcParsedMask
@param[in] this:
	The mask to print.
	@dassert @nonnull
@param[in] file:
	The mask's source file.
	@dassert @nonnull
@param[in] printer:
	The printer.
	@dassert @nonnull */
void rlc_parsed_mask_print(
	struct RlcParsedMask const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer);

#endif