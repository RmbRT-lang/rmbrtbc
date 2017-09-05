#include "error.h"
#include <stdlib.h>

static rlc_internal_error_fn_t rlc_internal_error_fn = NULL;
static rlc_lexical_error_fn_t rlc_lexical_error_fn = NULL;

void rlc_report_internal_error(
	char const * message)
{
	if(rlc_internal_error_fn)
		rlc_internal_error_fn(message);
}

void rlc_register_internal_error_function(
	rlc_internal_error_fn_t callback)
{
	rlc_internal_error_fn = callback;
}

void rlc_register_lexical_error_function(
	rlc_lexical_error_fn_t callback)
{
	rlc_lexical_error_fn = callback;
}
void rlc_report_lexical_error(
	char const * file,
	size_t line,
	size_t column,
	rlc_char_t const * line_string,
	enum RlcTokResult cause)
{
	if(rlc_lexical_error_fn)
		rlc_lexical_error_fn(
			file,
			line,
			column,
			line_string,
			cause);
}