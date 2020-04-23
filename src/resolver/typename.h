/** @file typename.h
	Contains resolved type names. */
#ifndef __rlc_resolver_typename_h_defined
#define __rlc_resolver_typename_h_defined

#include "../src/string.h"
#include "scopeitem.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcScopedTypeName;
struct RlcParsedTypeName;
struct RlcScopedScope;

enum RlcResolvedTypeKind
{
	kRlcResolvedTypeKindVoid,
	kRlcResolvedTypeKindScopeItem,
	kRlcResolvedTypeKindSignature
};

/** A resolved function signature type. */
struct RlcResolvedSignature
{
	/** The signature's arguments and return type.
		The first types are the arguments, and the last type is the return type. */
	struct RlcResolvedTypeName * types;
	/** The signature's type count. */
	RlcSrcSize typeCount;
};

/** A resolved type name. */
struct RlcResolvedTypeName
{
	/** The scoped type name. Contains the type's modifiers. */
	struct RlcScopedTypeName const * scoped;

	/** The type's kind. */
	enum RlcResolvedTypeKind kind;
	union
	{
		struct RlcResolvedScopeItem scopeItem;
		struct RlcResolvedSignature signature;
	} type;
};

void rlc_resolved_type_name_create(
	struct RlcResolvedTypeName * this,
	struct RlcScopedTypeName const * name,
	struct RlcScopedScope * scope);

void rlc_resolved_type_name_destroy(
	struct RlcResolvedTypeName * this);

#ifdef __cplusplus
}
#endif

#endif