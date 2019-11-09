/** @file assert.h
	Contains the assertion macros RLC_ASSERT and RLC_DASSERT. */
#ifndef __rlc_assert_h_defined
#define __rlc_assert_h_defined

#include "macros.h"

/** @def RLC_ASSERT(x)
	Asserts that `x` be true.
@param x:
	The expression to assert. */
#define RLC_ASSERT(x) ((void)((x) || (int)(rlc_assertion_failure(#x, __FILE__, __LINE__, __func__),0)))

/** @def RLC_DASSERT(x)
	If RLC_DEBUG is defined, asserts that `x` be true.
	Otherwise, generates no code.
@param x:
	The expression to assert. */
#ifdef RLC_DEBUG
#define RLC_DASSERT RLC_ASSERT
#else
#define RLC_DASSERT(x) ((void)0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Reports an assertion failure.
@param[in] expression:
	The expression that failed.
@param[in] file:
	The file name.
@param[in] line:
	The line.
@param[in] function:
	The function. */
_Noreturn void rlc_assertion_failure(
	char const * expression,
	char const * file,
	int line,
	char const * function);

/** @def RLC_EVAL(x)
	Evaluates expression `x`.
	Used as helper macro for `RLC_STATIC_ASSERT`. */
#define RLC_EVAL(x) x
/** @def RLC_PASTE(a,b)
	Pastes a and b.
	Used as helper macro for `RLC_PASTE_EVAL`. */
#define RLC_PASTE(a,b) a ## b
/** @def RLC_PASTE_EVAL(a,b)
	Pastes a and b after evaluating them once.
	Used as a helper macro for `RLC_STATIC_ASSERT`. */
#define RLC_PASTE_EVAL(a,b) RLC_PASTE(a,b)
/** @def RLC_STATIC_ASSERT
	Creates the name for fallback `static_assert`s in case the C standard is older than C11.
	Used as a helper macro for `static_assert`. */
#define RLC_STATIC_ASSERT RLC_PASTE_EVAL(static,RLC_EVAL(__COUNTER__))
#if __STDC_VERSION__ < 201112L
/** @def static_assert(x, msg)
	Statically asserts expression x. */
#define static_assert(x, msg) static char const RLC_STATIC_ASSERT[(x) ? 1: -1] = {'\0'};
#elif !defined(static_assert)
#define static_assert _Static_assert
#endif

#ifdef __cplusplus
}
#endif

#endif