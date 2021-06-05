/** @file concept.h
	Contains the definition for the concept type used by the parser. */
#ifndef __rlc_parser_concept_h_defined
#define __rlc_parser_concept_h_defined

#include "function.h"
#include "templatedecl.h"

/** A concept is a set of member functions that an object needs to have to fulfill the concept
@extends RlcParsedScopeEntry */
struct RlcParsedConcept
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The concept's template declarations. */
	struct RlcParsedTemplateDecl fTemplates;

	/** The concept's functions. */
	struct RlcParsedMemberFunction * fFunctions;
	/** The number of functions the concept has. */
	RlcSrcSize fFunctionCount;
};

/** Creates a parsed concept.
@memberof RlcParsedConcept
@param[out] this:
	The parsed concept to create.
	@dassert @nonnull
@param[in] name:
	The concept's name.
	@dassert @nonnull
@param[in] templates:
	The concept's templates. */
void rlc_parsed_concept_create(
	struct RlcParsedConcept * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates);

/** Destroys a parsed concept.
@memberof RlcParsedConcept
@param[in,out] this:
	The parsed concept to destroy.
	@dassert @nonnull */
void rlc_parsed_concept_destroy(
	struct RlcParsedConcept * this);

/** Parses a concept.
@memberof RlcParsedConcept
@param[out] out:
	The concept to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[in] templates:
	The concept's templates.
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_concept_parse(
	struct RlcParsedConcept * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates);

/** Prints a parsed concept.
@memberof RlcParsedConcept
@param[in] this:
	The concept to print.
	@dassert @nonnull
@param[in] file:
	The concept's source file.
	@dassert @nonnull
@param[in] printer:
	The printer.
	@dassert @nonnull */
void rlc_parsed_concept_print(
	struct RlcParsedConcept const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer);

#endif