#include "resolver.h"
#include "../assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

static _Noreturn void va_fail(
	struct RlcSrcString const * string,
	struct RlcSrcFile const * file,
	char const * msg,
	va_list ap)
{
	RLC_DASSERT(string != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(msg != NULL);

	fflush(stdout);
	usleep(125 * 1000);

	fprintf(stderr, "%s:%u:%u: error: ",
		file->fName,
		string->line,
		string->column);
	vfprintf(stderr, msg, ap);
	fprintf(stderr, ".\n");
	fflush(stderr);

	exit(EXIT_FAILURE);
}

_Noreturn void rlc_resolver_fail(
	struct RlcSrcString const * string,
	struct RlcSrcFile const * file,
	char const * msg,
	...)
{
	va_list ap;
	va_start(ap, msg);
	va_fail(string, file, msg, ap);
}

_Noreturn void rlc_resolver_fail_ctx(
	struct RlcResolverFailContext const * ctx,
	char const * msg,
	...)
{
	va_list ap;
	va_start(ap, msg);
	va_fail(ctx->name, ctx->file, msg, ap);
}