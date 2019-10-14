/** @file parser.h
	Contains consants used by the parser. */
#ifndef __rlc_parser_parser_h_defined
#define __rlc_parser_parser_h_defined

#include <stddef.h>

#include "../tokeniser/tokeniser.h"

#ifdef __cplusplus
extern "C"
#endif

/** Tracks the context in which the parser currently is. */
struct RlcParserTracer
{
	/** The parser's previous context. */
	struct RlcParserTracer * fPrevious;
	/** The context's name. */
	char const * fContext;
};

/** The parser state. */
struct RlcParser
{
	/** The token stream. */
	struct RlcTokeniser fTokeniser;
	/** The LL(2) token buffer. */
	struct RlcToken fLookahead[2];
	/** The amount of tokens in the buffer. */
	uint8_t fLookaheadSize;
	/** The current index in the buffer.*/
	uint8_t fToken;
	/** Whether the end of the token stream has been reached. */
	uint8_t fEnd;
	/** The parser's tracer. */
	struct RlcParserTracer * fTracer;
};

/** Creates a parser for a file.
@memberof RlcParser
@param[out] this:
	The parser to create.
	@dassert @nonnull
@param[in] file:
	The file to parse.
	@dassert @nonnull */
void rlc_parser_create(
	struct RlcParser * this,
	struct RlcSrcFile const * file);

/** Destroys parser data.
@memberof RlcParser
@param[in] this:
	The parser data to destroy.
	@dassert @nonnull */
void rlc_parser_destroy(
	struct RlcParser * this);

/** Makes the parser enter a context.
@memberof RlcParserTracer */
void rlc_parser_trace(
	struct RlcParser * this,
	char const * context,
	struct RlcParserTracer * tracer);

/** Destroys the parser's current context.
@memberof RlcParserTracer */
void rlc_parser_untrace(
	struct RlcParser * this);

/** Retrieves a parser's current context.
@memberof RlcParser */
_Nodiscard char const * rlc_parser_context(
	struct RlcParser const * this);

/** Whether the end of the file is reached.
@memberof RlcParser */
_Nodiscard inline int rlc_parser_eof(
	struct RlcParser const * this);

/** Whether the end of the file is directly after the current token.
@memberof RlcParser */
_Nodiscard inline int rlc_parser_ahead_eof(
	struct RlcParser const * this);

/** Returns the address of the current token, if not at the end of the token stream.
@memberof RlcParser
@param[in] this:
	The parser data.
	@dassert @nonnull */
_Nodiscard inline struct RlcToken const * rlc_parser_current(
	struct RlcParser const * this);

/** Retrieves the parser's next token.
@memberof RlcParser */
_Nodiscard inline struct RlcToken const * rlc_parser_ahead(
	struct RlcParser const * this);

/** Retrieves the parser's current source index.
@memberof RlcParser */
_Nodiscard RlcSrcIndex rlc_parser_index(
	struct RlcParser const * this);

/** Matches a token.
@memberof RlcParser
@param[in] this:
	The parser data.
@param[in] type:
	The token type to match.
@return
	Nonzero if matched. */
_Nodiscard int rlc_parser_is_current(
	struct RlcParser const * this,
	enum RlcTokenType type);

_Nodiscard struct RlcToken const * rlc_parser_is_ahead(
	struct RlcParser const * this,
	enum RlcTokenType type);

/** Terminates with an error message.
@memberof RlcParser */
_Noreturn void rlc_parser_fail(
	struct RlcParser * parser,
	char const * reason);

/** Tries to match a token of the given type, and on success, consumes it.
@memberof RlcParser
@param[in,out] this:
	The parser data.
	@dassert @nonnull
@param[out] token:
	If nonnull, saves the matched token in `token`.
@param[in] type:
	The token type to match.
@return
	Nonzero if a token was matched. */
_Nodiscard int rlc_parser_consume(
	struct RlcParser * this,
	struct RlcToken * token,
	enum RlcTokenType type);

/** Tries to match a token of any of the given types, and on success, consumes it.
	On failure, displays an error message and aborts the program.
@memberof RlcParser
@param[in,out] this:
	The parser data.
	@dassert @nonnull
@param[out] token:
	If nonnull, saves the matched token in `token`.
@return
	The matched token type. */
enum RlcTokenType rlc_parser_expect(
	struct RlcParser * this,
	struct RlcToken * token,
	size_t count,
	enum RlcTokenType types, ...);

/** Skips the parser's current token.
@memberof RlcParser
@param[in,out] this:
	The parser data.
	@dassert @nonnull */
void rlc_parser_skip(
	struct RlcParser * this);

#include "parser.inl"

#ifdef __cplusplus
}
#endif


#endif