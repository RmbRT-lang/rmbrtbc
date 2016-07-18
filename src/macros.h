#ifndef __rlc_macros_h_defined
#define __rlc_macros_h_defined

#ifndef _countof
#define _countof(x) (sizeof((x)) / sizeof(*(x)))
#endif

/** Defines the entry count of the enum.
@param Enum: The Enum name. */
#define RLC_ENUM_END(Enum) k##Enum##Count

#define RLC_COUNT(Enum) ((unsigned)k##Enum##Count)
#define RLC_IN_ENUM(x,Enum) (((unsigned) x) < RLC_COUNT(Enum))

#endif