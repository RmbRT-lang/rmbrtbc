#ifndef __rlc_error_h_defined
#define __rlc_error_h_defined

#include "tokeniser/tokens.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Error callback type for internal errors.
@param[in] message:
	The error message. */
typedef void (*rlc_internal_error_fn_t)(
	char const * message);

/** Reports an internal error.
	Invokes the registered internal error callback.
@param[in] message:
	The error message. */
void rlc_report_internal_error(
	char const * message);

/** Registers an internal error handler.
	This is to be used for error reporting. */
void rlc_register_internal_error_function(
	rlc_internal_error_fn_t callback);

/** Error callback type for lexical errors.
	These errors are sent to the callback function to enable the tracking of errors more easily for external libraries.
@param[in] file:
	The name of the file in which the error occured.
@param[in] line:
	The line number (1-based) of the error.
@param[in] column:
	The number (1-based) of the first character of the error inside the line in which it occured.
@param[in] line_string:
	The contents of the line in which the error occured.
@param[in] error:
	The constant describing the error cause.*/
typedef void (*rlc_lexical_error_fn_t)(
	char const * file,
	size_t line,
	size_t column,
	rlc_char_t const * line_string,
	enum RlcTokResult cause);

/** Reports a lexical error.
	Invokes the registered lexical error callback.
@param[in] file:
	The name of the file in which the error occured.
@param[in] line:
	The line number (1-based) of the error.
@param[in] column:
	The number (1-based) of the first character of the error inside the line in which it occured.
@param[in] line_string:
	The contents of the line in which the error occured.
@param[in] error:
	The constant describing the error cause. */
void rlc_report_lexical_error(
	char const * file,
	size_t line,
	size_t column,
	rlc_char_t const * line_string,
	enum RlcTokResult cause);

/** Registers a lexical error handler.
@param[in] callback:
	The callback to be called for lexical errors. */
void rlc_register_lexical_error_function(
	rlc_lexical_error_fn_t callback);

#ifdef __cplusplus
}
#endif

#endif