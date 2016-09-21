/** @file parser.h
	Contains consants used by the parser. */
#ifndef __rlc_parser_data_parser_h_defined
#define __rlc_parser_data_parser_h_defined

#include <stddef.h>
#include "../preprocessor/preprocessor.h"

#ifdef __cplusplus
extern "C"
#endif


/** The error codes for parsing errors. */
enum RlcParseError
{
	/** Expected an identifier. */
	kRlcParseErrorExpectedIdentifier,
	/** Expected a type name. */
	kRlcParseErrorExpectedTypeName,
	/** Expected a semicolon. */
	kRlcParseErrorExpectedSemicolon,
	/** Expected a symbol. */
	kRlcParseErrorExpectedSymbol,
	
	RLC_ENUM_END(RlcParseError)
};

/** An error message used by the parser. */
struct RlcParseErrorMessage
{
	/** The error code. */
	enum RlcParseError fError;
	/** The error location (token index). */
	size_t fLocation;
};

/** The parser state. */
struct RlcParserData
{
	/** The error messages. */
	struct RlcParseErrorMessage * fErrors;
	/** The error count. */
	size_t fErrorCount;

	/** The preprocessed source file. */
	struct RlcPreprocessedFile * fFile;

	/** The current token index. */
	size_t fIndex;
};

/** Goes to the next token.
@param[in,out] this:
	The parser data.
@return
	Whether there is a next token. */
int rlc_parser_data_next(
	struct RlcParserData * this);

/** Returns the address of the current token, if not at the end of the token stream.
@param[in] this:
	The parser data.
	@dassert @nonnull */
struct RlcToken const * rlc_parser_data_current(
	struct RlcParserData const * this);

/** Matches a token.
@param[in] this:
	The parser data.
@param[in] type:
	The token type to match.
@return
	If matched, the address of the matched token, otherwise `null`. */
struct RlcToken const * rlc_parser_data_match(
	struct RlcParserData const * this,
	enum RlcTokenType type);

/** Tries to match a token of the given type, and on success, consumes it.
@param[in,out] this:
	The parser data.
@param[in] type:
	The token type to match.
@return
	The matched token, or `null`. */
struct RlcToken const * rlc_parser_data_consume(
	struct RlcParserData * this,
	enum RlcTokenType type);

/** Destroys parser data.
@param[in] this:
	The parser data to destroy.
	@dassert @nonnull */
void rlc_parser_data_destroy(
	struct RlcParserData * this);

/** Adds an error to a parser data.
@param[in,out] this:
	The parser data to add an error to.
@param[in] error_message:
	The error message to add. */
void rlc_parser_data_add_error(
	struct RlcParserData * this,
	enum RlcParseError error_message);

#ifdef __cplusplus
}
#endif


#endif