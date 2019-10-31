/** @file variable.h
	Defines the variable struct. */
#ifndef __rlc_parser_variable_h_defined
#define __rlc_parser_variable_h_defined

#include "scopeentry.h"
#include "expression.h"
#include "typename.h"
#include "templatedecl.h"
#include "member.h"
#include "parser.h"
#include "../macros.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Reference types. */
enum RlcReferenceType
{
	/** Not a reference. */
	kRlcReferenceTypeNone,
	/** A normal const reference. */
	kRlcReferenceTypeReference,
	/** A temporary object reference. */
	kRlcReferenceTypeTempReference
};

/** Variables as used in the parser.
@extends RlcParsedScopeEntry */
struct RlcParsedVariable
{
	RLC_DERIVE(struct, RlcParsedScopeEntry);

	/** The template declaration. */
	struct RlcParsedTemplateDecl fTemplates;
	/** Whether the variable has an explicit type. */
	int fHasType;
	union {
		/** The variable type. */
		struct RlcParsedTypeName fType;
		/** The type qualifier, if no explicit type. */
		enum RlcTypeQualifier fTypeQualifier;
	};
	/** The variable's reference type. */
	enum RlcReferenceType fReferenceType;
	/** The initialising arguments, or null. */
	struct RlcParsedExpression ** fInitArgs;
	/** The initialising argument count. */
	size_t fInitArgCount;
};

/** Creates a variable.
@memberof RlcParsedVariable
@param[out] this:
	The variable to create.
	@dassert @nonnull */
void rlc_parsed_variable_create(
	struct RlcParsedVariable * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates);

/** Destroys a variable.
@memberof RlcParsedVariable
@param[in,out] this:
	The variable to destroy.
	@dassert @nonnull */
void rlc_parsed_variable_destroy(
	struct RlcParsedVariable * this);

/** Parses a variable.
	Syntax:

	Initialiser ::= ':=' Expression
	Initialiser ::= '(' Argument (',' Argument )* ')
		*identifier* : *type name expression* := 

@memberof RlcParsedVariable
@param[out] out:
	The variable to parse.
@param[in,out] parser:
	The parser data.
@param[in] needs_name:
	Whether a name is expected.
@param[in] allow_initialiser:
	Whether an initialiser is allowed.
@param[in] force_initialiser:
	Whether an initialiser is expected.
@param[in] allow_templates:
	Whether to allow a template declaration.
@param[in] allow_reference:
	Whether to allow the variable to be a reference.
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_variable_parse(
	struct RlcParsedVariable * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates,
	int needs_name,
	int allow_initialiser,
	int force_initialiser,
	int allow_templates,
	int allow_reference);

/** Member variable type.
@extends RlcParsedMember
@extends RlcParsedVariable */
struct RlcParsedMemberVariable
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedVariable);

	/** Whether this variable has a modifier like `static` or `isolated`. */
	enum RlcMemberAttribute fAttribute;
};

/** Creates a parsed member variable.
@param[out] this:
	The parsed member variable to create.
	@dassert @nonnull */
void rlc_parsed_member_variable_create(
	struct RlcParsedMemberVariable * this,
	enum RlcVisibility visibility,
	enum RlcMemberAttribute attribute,
	size_t start_index);

/** Destroys a parsed member variable.
@memberof RlcParsedMemberVariable
@param[in] this:
	The parsed member variable to destroy. */
void rlc_parsed_member_variable_destroy(
	struct RlcParsedMemberVariable * this);

/** Parses a member variable, but does not parse the `RlcParsedMember` base instance.
	To completely parse it, call `rlc_parsed_member_parse_base()` beforehand on a seperate `RlcParsedMember` instance and assign it to `out` afterwards.
@memberof RlcParsedMemberVariable
@param[out] out:
	The member variable to parse into. The `RlcParsedMember` will not be parsed.
@param[in,out] parser:
	The parser data.
@return
	Whether the parsing was successful. */
int rlc_parsed_member_variable_parse(
	struct RlcParsedMemberVariable * out,
	enum RlcVisibility * default_visibility,
	struct RlcParser * parser);

#ifdef __cplusplus
}
#endif

#endif