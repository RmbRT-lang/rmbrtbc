#include "member.h"
#include "scope.h"
#include "../assert.h"

void rlc_scoped_member_create(
	struct RlcScopedMember * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMember const * parsed,
	struct RlcScopedScope * parent,
	enum RlcScopedMemberType type,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcScopedMemberType));
	RLC_DASSERT((int)type == (int)RLC_DERIVING_TYPE(parsed));

	struct RlcSrcString const * name = rlc_parsed_member_name(parsed);
	struct RlcToken nameToken;
	if(name)
	{
		nameToken.type = kRlcTokIdentifier;
		nameToken.content = *name;
	} else switch(RLC_DERIVING_TYPE(parsed))
	{
	case kRlcParsedConstructor: nameToken.type = kRlcTokConstructor; break;
	case kRlcParsedDestructor: nameToken.type = kRlcTokDestructor; break;
	default:
		RLC_DASSERT(!"unhandled member type");
	}

	rlc_scoped_scope_item_create(
		RLC_BASE_CAST(this, RlcScopedScopeItem),
		rlc_scoped_scope_group(parent, &nameToken, file),
		1,
		kRlcScopedMember,
		file,
		templates ? templates : &kRlcParsedTemplateDeclNone);

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
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	typedef uint8_t * (*constructor_t)(
		struct RlcSrcFile const *,
		void const *,
		struct RlcScopedScopeItemGroup *);

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