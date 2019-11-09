/** @file tokens.h
	Contains the declarations of the token data type as well as constants related to tokenising / lexical analysis of source files. */

#ifndef __rlc_tokens_h_defined
#define __rlc_tokens_h_defined

#include "../unicode.h"
#include "../macros.h"
#include "../src/string.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** The token type. */
enum RlcTokenType
{
	kRlcTokIdentifier,
	kRlcTokNumberLiteral,
	kRlcTokCharacterLiteral,
	kRlcTokStringLiteral,

	// Keywords.
	kRlcTokBreak,
	kRlcTokCase,
	kRlcTokConstructor,
	kRlcTokContinue,
	kRlcTokDefault,
	kRlcTokDestructor,
	kRlcTokDo,
	kRlcTokElse,
	kRlcTokEnum,
	kRlcTokExtern,
	kRlcTokFor,
	kRlcTokIf,
	kRlcTokInclude,
	kRlcTokInline,
	kRlcTokIsolated,
	kRlcTokNamespace,
	kRlcTokNoinit,
	kRlcTokNumber,
	kRlcTokOperator,
	kRlcTokPrivate,
	kRlcTokProtected,
	kRlcTokPublic,
	kRlcTokReturn,
	kRlcTokSizeof,
	kRlcTokStatic,
	kRlcTokSwitch,
	kRlcTokThis,
	kRlcTokType,
	kRlcTokUnion,
	kRlcTokVirtual,
	kRlcTokVoid,
	kRlcTokWhile,

	// Non-identifier keywords.
	kRlcTokPlusEqual,
	kRlcTokDoublePlus,
	kRlcTokPlus,

	kRlcTokMinusEqual,
	kRlcTokMinusColon,
	kRlcTokDoubleMinus,
	kRlcTokMinusGreaterAsterisk,
	kRlcTokMinusGreater,
	kRlcTokMinus,

	kRlcTokAsteriskEqual,
	kRlcTokAsterisk,

	kRlcTokBackslash,

	kRlcTokForwardSlashEqual,
	kRlcTokForwardSlash,

	kRlcTokPercentEqual,
	kRlcTokPercent,

	kRlcTokExclamationMarkEqual,
	kRlcTokExclamationMarkColon,
	kRlcTokExclamationMark,

	kRlcTokCircumflexEqual,
	kRlcTokCircumflex,

	kRlcTokTildeColon,
	kRlcTokTilde,

	kRlcTokDoubleAndEqual,
	kRlcTokDoubleAnd,
	kRlcTokAndEqual,
	kRlcTokAnd,

	kRlcTokDoublePipeEqual,
	kRlcTokDoublePipe,
	kRlcTokPipeEqual,
	kRlcTokPipe,

	kRlcTokQuestionMark,

	kRlcTokDoubleColonEqual,
	kRlcTokColonEqual,
	kRlcTokDoubleColon,
	kRlcTokColon,
	kRlcTokDoubleAt,
	kRlcTokAt,
	kRlcTokDoubleDotExclamationMark,
	kRlcTokDoubleDotQuestionMark,
	kRlcTokDotAsterisk,
	kRlcTokDot,
	kRlcTokComma,
	kRlcTokSemicolon,
	kRlcTokDoubleEqual,

	kRlcTokBracketOpen,
	kRlcTokBracketClose,
	kRlcTokBraceOpen,
	kRlcTokBraceClose,
	kRlcTokParentheseOpen,
	kRlcTokParentheseClose,

	kRlcTokTripleLessEqual,
	kRlcTokTripleLess,
	kRlcTokDoubleLessEqual,
	kRlcTokDoubleLess,
	kRlcTokLessEqual,
	kRlcTokLess,

	kRlcTokTripleGreaterEqual,
	kRlcTokTripleGreater,
	kRlcTokDoubleGreaterEqual,
	kRlcTokDoubleGreater,
	kRlcTokGreaterEqual,
	kRlcTokGreater,

	kRlcTokDollar,
	kRlcTokHash,
	RLC_ENUM_END(RlcTokenType)

};

/** Looks up the name belonging to the given token type.
@param[in] type:
	The token type whose name to retrieve.
@return
	The token type's name. */
char const * rlc_token_type_name(
	enum RlcTokenType type);

/** A token as used by the tokenizer and preprocessor.
	It represents a substring of a source file. */
struct RlcToken
{
	struct RlcSrcString content;
	/** Stores a rlcTokenType. */
	enum RlcTokenType type;
};

/** Retrieves the text position of the token.
@param[in] this:
	The token whose position to retrieve.
	@dassert @nonnull
@param[out] line:
	The source line (0-based).
	@dassert @nonnull
@param[out] column:
	The source column (0-based).
	@dassert @nonnull */
void rlc_token_position(
	struct RlcToken const * this,
	size_t * line,
	size_t * column);

/** Retrieves the text position of the end of the token.
@param[in] this:
	The token whose position to retrieve.
	@dassert @nonnull
@param[out] line:
	The source line (0-based).
	@dassert @nonnull
@param[out] column:
	The source column (0-based).
	@dassert @nonnull */
void rlc_token_end(
	struct RlcToken const * this,
	size_t * line,
	size_t * column);

/** Returns the token's text content.
@param[in] this:
	The token whose contents to retrieve.
	@dassert @nonnull
@return
	The token's text.
	@free_pointer */
rlc_char_t * rlc_token_content(
	struct RlcToken const * this);

#ifdef __cplusplus
}
#endif

#endif