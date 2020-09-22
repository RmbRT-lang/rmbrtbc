#ifndef __rlc_macros_h_defined
#define __rlc_macros_h_defined

#ifndef _countof
#define _countof(x) (sizeof((x)) / sizeof(*(x)))
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define _Nodiscard __attribute__((warn_unused_result))

/** @def RLC_ENUM_END(Enum)
	Defines the entry count of the enum.
@param Enum: The Enum name. */
#define RLC_ENUM_END(Enum) k##Enum##Count

#define RLC_COUNT(Enum) ((unsigned)k##Enum##Count)
#define RLC_IN_ENUM(x,Enum) (((unsigned) x) < RLC_COUNT(Enum))
#define RLC_COVERS_ENUM(Array, Enum) (_countof((Array)) == RLC_COUNT(Enum))

#define RLC_FLAG(x) (1<<(x))
#define RLC_ALL_FLAGS(enum) (RLC_FLAG(RLC_COUNT(enum))-1)

#define RLC_OFFSETOF_PTR(p,m) ((unsigned)((char *)&(p)->m - (char *)(p)))
#define RLC_OFFSETOF(t,m) RLC_OFFSETOF_PTR((t *)NULL, m)


/** @def RLC_DERIVE_CAST(a,b,d)
	Casts the base type `b` address `a` to deriving type `d`. Caution: does not work with typedefs, as the type name passed to the macro must be the same as used with `RLC_DERIVE`.
@param a:
	The address to cast from.
@param b:
	The base type (no tag).
@param d:
	The type name to cast to. */
#define RLC_DERIVE_CAST(a,b,d) ((d*)((a) ? (void*)((char*)(a) - RLC_OFFSETOF(d, fDerived##b)) : NULL))

/** Base + RLC_DERIVE_OFFSET = Derived */
#define RLC_DERIVE_OFFSET(b,d) (-(intptr_t)(RLC_OFFSETOF(d, fDerived##b)))

/** Derived + RLC_BASE_OFFSET = Base */
#define RLC_BASE_OFFSET(b,d) RLC_OFFSETOF(d, fDerived##b)

/** Retrieves the base instance of `a`. */
#define RLC_BASE_CAST(a,b) ((a) ? &(a)->fDerived##b : NULL)
#define RLC_BASE_CAST2(a,b1,b2) ((a) ? &(a)->fDerived##b1.fDerived##b2 : NULL)
#define RLC_BASE(a,b) (&(a)->fDerived##b)

/** @def RLC_DERIVE(tag, b)
	Derives the current type from the type `b` (if any tags, put them in `tag`). */
#define RLC_DERIVE(tag, b) tag b fDerived##b

/** @def RLC_ABSTRACT(type)
	Enables dynamic casting.
	Creates an enum  */
#define RLC_ABSTRACT(type) enum type##Type fDerivingType

/** @def RLC_DERIVING_TYPE(a)
	Returns the deriving type of an abstract type. */
#define RLC_DERIVING_TYPE(a) ((a)->fDerivingType)

/** @def RLC_DYNAMIC_CAST(a,b,d)
	Performs a dynamic cast to a deriving type. */
#define RLC_DYNAMIC_CAST(a,b,d) ((struct d*) \
	((a) && ((a)->fDerivingType == k##d) \
		? (void*)((char*)(a) - RLC_OFFSETOF(struct d, fDerived##b)) \
		: NULL))

#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

#endif