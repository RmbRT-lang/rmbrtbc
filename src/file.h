/** @file file.h Contains functions to manipulate files. */
#ifndef __rlc_file_h_defined
#define __rlc_file_h_defined

#include "unicode.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Result of a file operation. */
typedef enum RlcFileResult
{
	kRlcFileSuccess,
	kRlcFileFailRead,
	kRlcFileFailEncoding
} RlcFileResult;

/** Returns a string representing the error code. */
char const * rlc_file_result_message(
	RlcFileResult result);

/** Reads the whole (text) file and returns the text contained inside.
	The file must be formatted in either UTF-8 with BOM or UTF-8 without BOM.
@param[in] filename:
	The file to load.
@param[out] content:
	If successful, will hold a copy of the file contents.
@return
	* kRlcFileSuccess, if the file was loaded and converted successfully.
	* kRlcFileFailRead, if the file could not be opened or read.
	* kRlcFileFailEncoding, if the file was not encoded in UTF-8. */
RlcFileResult rlc_read_text_file(char const * filename, rlc_char_t ** content);



#ifdef __cplusplus
}
#endif

#endif