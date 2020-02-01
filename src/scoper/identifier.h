/** @file identifier.h
	Contains the scoped identifier type. */
#ifndef __rlc_scoper_identifier_h_defined
#define __rlc_scoper_identifier_h_defined

#ifdef __cplusplus
extern "C" {
#endif

struct RlcSrcFile;
struct RlcSrcString;

/** A scoped symbol child's type. */
enum RlcScopedIdentifierType
{
	kRlcScopedIdentifierTypeIdentifier,
	kRlcScopedIdentifierTypeConstructor,
	kRlcScopedIdentifierTypeDestructor
};

/** A scoped identifier. */
const struct RlcScopedIdentifier
{
	/** Whether the identifier is a keyword. */
	enum RlcScopedIdentifierType type;
	/** NULL if the type is a keyword. */
	char const * name;
}	kRlcScopedIdentifierConstructor,
	kRlcScopedIdentifierDestructor;

/** Creates a scoped identifier from a source string.
	To create special identifiers (constructor, etc.), use the global constant variables.
@memberof RlcScopedIdentifier
@param[out] this:
	The identifier to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] name:
	The source string.
	@dassert @nonnull */
void rlc_scoped_identifier_create(
	struct RlcScopedIdentifier * this,
	struct RlcSrcFile const * file,
	struct RlcSrcString const * name);

/** Destroys a scoped identifier.
@memberof RlcScopedIdentifier
@param[in, out] this:
	The identifier to destroy.
	@dassert @nonnull */
void rlc_scoped_identifier_destroy(
	struct RlcScopedIdentifier * this);

/** Compares two identifiers.
@memberof RlcScopedIdentifier
@param[in] a:
	The left-hand side identifier.
	@dassert @nonnull
@param[in] b:
	The right-hand side identifier.
	@dassert @nonnull
@return
	The relation between a and b. */
int rlc_scoped_identifier_compare(
	struct RlcScopedIdentifier const * a,
	struct RlcScopedIdentifier const * b);

#ifdef __cplusplus
}
#endif

#endif