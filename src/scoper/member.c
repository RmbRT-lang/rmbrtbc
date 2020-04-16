#include "member.h"
#include "scope.h"

#include "function.h"
#include "variable.h"
#include "rawtype.h"
#include "union.h"
#include "class.h"
#include "enum.h"
#include "typedef.h"
#include "constructor.h"
#include "destructor.h"
#include "../parser/function.h"
#include "../parser/variable.h"
#include "../parser/rawtype.h"
#include "../parser/union.h"
#include "../parser/class.h"
#include "../parser/enum.h"
#include "../parser/typedef.h"
#include "../parser/constructor.h"
#include "../parser/destructor.h"

#include "../assert.h"
#include "../malloc.h"

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

	typedef void (*constructor_t)(
		void *,
		struct RlcSrcFile const *,
		void const *,
		struct RlcScopedScopeItemGroup *);

#define ENTRY(constructor, type) { \
		(constructor_t)&constructor, \
		RLC_DERIVE_OFFSET(RlcScopedMember, struct RlcScoped##type), \
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsed##type), \
		sizeof(struct RlcScoped##type), \
		kRlcParsed##type \
	}
#define NOENTRY(type) { \
		NULL, 0, 0, 0, kRlcParsed##type \
	}

	static struct {
		constructor_t constructor;
		intptr_t scopedOffset;
		intptr_t parsedOffset;
		size_t size;
		enum RlcParsedMemberType parsedType;
	} k_constructors[] = {
		ENTRY(rlc_scoped_member_function_create, MemberFunction),
		ENTRY(rlc_scoped_member_variable_create, MemberVariable),
		ENTRY(rlc_scoped_member_rawtype_create, MemberRawtype),
		ENTRY(rlc_scoped_member_union_create, MemberUnion),
		ENTRY(rlc_scoped_member_class_create, MemberClass),
		ENTRY(rlc_scoped_member_enum_create, MemberEnum),
		ENTRY(rlc_scoped_member_typedef_create, MemberTypedef),
		ENTRY(rlc_scoped_constructor_create, Constructor),
		ENTRY(rlc_scoped_destructor_create, Destructor)
	};
#undef ENTRY
#undef NOENTRY

	enum RlcScopedMemberType type = RLC_DERIVING_TYPE(parsed);

	static_assert(RLC_COVERS_ENUM(k_constructors, RlcParsedMemberType),
		"Incomplete vtable.");
	RLC_DASSERT(k_constructors[type].constructor);
	RLC_DASSERT((unsigned)k_constructors[type].parsedType == (unsigned)type);

	uint8_t * ret = NULL;
	rlc_malloc((void**)&ret, k_constructors[type].size);
	k_constructors[RLC_DERIVING_TYPE(parsed)].constructor(
		ret,
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
		RLC_DERIVE_OFFSET(RlcScopedMember, struct RlcScoped##type), \
		kRlcParsed##type, \
	}
#define NOENTRY(type) { \
		NULL, 0, kRlcParsed##type \
	}

	typedef void (*destructor_t)(void *);
	static struct {
		destructor_t dtor;
		intptr_t deriveOffset;
		enum RlcScopedMemberType type;
	} const k_dtors[] = {
		ENTRY(rlc_scoped_member_function_destroy, MemberFunction),
		ENTRY(rlc_scoped_member_variable_destroy, MemberVariable),
		ENTRY(rlc_scoped_member_rawtype_destroy, MemberRawtype),
		ENTRY(rlc_scoped_member_union_destroy, MemberUnion),
		ENTRY(rlc_scoped_member_class_destroy, MemberClass),
		ENTRY(rlc_scoped_member_enum_destroy, MemberEnum),
		ENTRY(rlc_scoped_member_typedef_destroy, MemberTypedef),
		ENTRY(rlc_scoped_constructor_destroy, Constructor),
		ENTRY(rlc_scoped_destructor_destroy, Destructor)
	};
#undef ENTRY
#undef NOENTRY

	enum RlcScopedMemberType type = RLC_DERIVING_TYPE(this);

	static_assert(RLC_COVERS_ENUM(k_dtors, RlcParsedMemberType),
		"Incomplete vtable.");
	RLC_DASSERT(k_dtors[type].dtor);
	RLC_DASSERT((unsigned)k_dtors[type].type == (unsigned)type);

	k_dtors[RLC_DERIVING_TYPE(this)].dtor(
		((char *)this) + k_dtors[RLC_DERIVING_TYPE(this)].deriveOffset);
}