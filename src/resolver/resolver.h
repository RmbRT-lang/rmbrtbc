#ifndef __rlc_resolver_resolver_h_defined
#define __rlc_resolver_resolver_h_defined

#include "../src/file.h"
#include "../tokeniser/tokens.h"
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

_Noreturn void rlc_resolver_fail(
	struct RlcToken const * token,
	struct RlcSrcFile const * file,
	char const * msg);


#ifdef __cplusplus
}
#endif

#endif