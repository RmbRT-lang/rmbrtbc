#ifndef __rlc_resolver_resolver_h_defined
#define __rlc_resolver_resolver_h_defined

#include "../src/file.h"
#include "../tokeniser/tokens.h"
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

_Noreturn void rlc_resolver_fail(
	struct RlcSrcString const * token,
	struct RlcSrcFile const * file,
	char const * msg,
	...);

struct RlcResolverFailContext
{
	struct RlcSrcString const * name;
	struct RlcSrcFile const * file;
};

_Noreturn void rlc_resolver_fail_ctx(
	struct RlcResolverFailContext const * ctx,
	char const * msg,
	...);


#ifdef __cplusplus
}
#endif

#endif