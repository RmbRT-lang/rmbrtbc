#include "member.h"
#include "../assert.h"

void rlc_scoped_member_create(
	struct RlcScopedMember * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMember const * parsed,
	struct RlcScopedScope * parent,
	enum RlcScopedMemberType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcScopedMemberType));
	RLC_DASSERT((int)type == (int)RLC_DERIVING_TYPE(parsed));

	struct RlcScopedIdentifier ident;
	struct RlcScopedIdentifier const * pident = NULL;
	struct RlcSrcString const * name = rlc_parsed_member_name(parsed);
	if(name)
		rlc_scoped_identifier_create(&ident, file, name);
	else switch(RLC_DERIVING_TYPE(parsed))
	{
	case kRlcParsedConstructor: pident = &kRlcScopedIdentifierConstructor; break;
	case kRlcParsedDestructor: pident = &kRlcScopedIdentifierDestructor; break;
	default:
		RLC_DASSERT(!"unhandled member type");
	}

	rlc_scoped_scope_item_create(
		RLC_BASE_CAST(this, RlcScopedScopeItem),
		pident,
		parent,
		1,
		kRlcScopedMember);

	RLC_DERIVING_TYPE(this) = type;

	this->parsed = parsed;
}

void rlc_scoped_member_destroy_base(
	struct RlcScopedMember * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_item_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeItem));
}

struct RlcScopedMember * rlc_scoped_member_new(
	struct RlcSrcFile const * file,
	struct RlcParsedMember const * parsed,
	struct RlcScopedScopeItem * parent)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	typedef uint8_t * (*constructor_t)(
		struct RlcSrcFile const *,
		void const *,
		struct RlcScopedScopeItem *);

#define ENTRY(constructor, type) { \
		(constructor_t)&constructor, \
		RLC_DERIVE_OFFSET(RlcScopedMember, struct RlcScoped##type), \
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcScoped##type), \
		kRlcParsed##type \
	}

	static struct {
		constructor_t constructor;
		intptr_t scopedOffset;
		intptr_t parsedOffset;
		enum RlcParsedMemberType parsedType;
	} k_constructors[] = {
	};
#undef ENTRY

	/*static_assert(RLC_COVERS_ENUM(k_constructors, RlcParsedMemberType),
		"Incomplete vtable."); // */
	RLC_DASSERT(k_constructors[RLC_DERIVING_TYPE(parsed)].constructor);

	uint8_t * ret;
	ret = k_constructors[RLC_DERIVING_TYPE(parsed)].constructor(
		file,
		(uint8_t *)parsed + k_constructors[RLC_DERIVING_TYPE(parsed)].parsedOffset,
		parent);
	ret += k_constructors[RLC_DERIVING_TYPE(parsed)].scopedOffset;

	return (struct RlcScopedMember *) ret;
}

void rlc_scoped_member_destroy_virtual(
	struct RlcScopedMember * this)
{
	RLC_DASSERT(this != NULL);

#define ENTRY(dtor, type) { \
		(destructor_t) &dtor, \
		RLC_DERIVE_OFFSET(RlcScopedMember, struct type), \
		k##type, \
	}
	typedef void (*destructor_t)(void *);
	static struct {
		destructor_t dtor;
		intptr_t deriveOffset;
		enum RlcScopedMemberType type;
	} const k_dtors[] = {
	};
#undef ENTRY

	RLC_DASSERT(RLC_DERIVING_TYPE(this) < _countof(k_dtors));
	RLC_DASSERT(k_dtors[RLC_DERIVING_TYPE(this)].type == RLC_DERIVING_TYPE(this));

	k_dtors[RLC_DERIVING_TYPE(this)].dtor(
		((char *)this) + k_dtors[RLC_DERIVING_TYPE(this)].deriveOffset);
}