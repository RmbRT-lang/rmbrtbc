#ifndef __rlc_macros_h_defined
#define __rlc_macros_h_defined

#ifndef _countof
#define _countof(x) (sizeof((x)) / sizeof(*(x)))
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

/** @def RLC_ENUM_END(Enum)
	Defines the entry count of the enum.
@param Enum: The Enum name. */
#define RLC_ENUM_END(Enum) k##Enum##Count

#define RLC_COUNT(Enum) ((unsigned)k##Enum##Count)
#define RLC_IN_ENUM(x,Enum) (((unsigned) x) < RLC_COUNT(Enum))
#define RLC_COVERS_ENUM(Array, Enum) (_countof((Array)) == RLC_COUNT(Enum))

#ifndef _offsetof
#define _offsetof(t,m) ((unsigned)(&(t *)NULL)->m)
#endif

/** @def RLC_DERIVE_CAST(a,b,tag,d)
	Casts the base type `b` address `a` to deriving type `d`. Caution: does not work with typedefs, as the type name passed to the macro must be the same as used with `RLC_DERIVE`.
@param a:
	The address to cast from.
@param b:
	The base type (tag qualified).
@param d:
	The type name to cast to. */
#define RLC_DERIVE_CAST(a,b,tag,d) ((d*)(rlc_derive_cast((a), _offsetof(d, fDerived##b))))

/** Retrieves the base instance of `a`. */
#define RLC_BASE_CAST(a,b) ((a) ? &(a)->fDerived##b : 0)

/** @def RLC_DERIVE(tag, b)
	Derives the current type from the type `b` (if any tags, put them in `tag`). */
#define RLC_DERIVE(tag, b) tag b fDerived##b

/** Converts a base address into its deriving type.
	This is a helper function for `RLC_DERIVE`.
@param[in] a:
	The base address.
@param[in] offset:
	The offset of */
inline void * rlc_derive_cast(void * a, unsigned offset)
{
	return a ? (char*)a - offset : NULL;
}

inline void * rlc_base_cast(void * a, unsigned offset)
{
	return a ? (char *)a + offset : NULL;
}

#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

#endif