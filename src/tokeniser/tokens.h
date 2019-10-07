/** @file tokens.h
	Contains the declarations of the token data type as well as constants related to tokenising / lexical analysis of source files. */

#ifndef __rlc_tokens_h_defined
#define __rlc_tokens_h_defined

#include "../unicode.h"
#include "../macros.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** The token type. */
enum RlcTokenType
{
	kRlcTokWhitespace,
	kRlcTokComment,
	kRlcTokIdentifier,
	kRlcTokHexNumber,
	kRlcTokDecimalNumber,
	kRlcTokOctalNumber,
	kRlcTokCharNumber,
	kRlcTokUtf8CharNumber,
	kRlcTokUtf16CharNumber,
	kRlcTokUtf32CharNumber,
	kRlcTokUtf16leCharNumber,
	kRlcTokUtf16beCharNumber,
	kRlcTokUtf32leCharNumber,
	kRlcTokUtf32beCharNumber,
	kRlcTok0,
	kRlcTokString,
	kRlcTokUtf8String,
	kRlcTokUtf16String,
	kRlcTokUtf32String,
	kRlcTokUtf16leString,
	kRlcTokUtf16beString,
	kRlcTokUtf32leString,
	kRlcTokUtf32beString,

	kRlcTokBin16,
	kRlcTokBin32,
	kRlcTokBin64,
	kRlcTokBin128,

	kRlcTokBackslash,
	kRlcTokComma,
	kRlcTokDot,
	kRlcTokDotAsterisk,
	kRlcTokTripleDotExclamationMark,
	kRlcTokTripleDot,
	kRlcTokAt,
	kRlcTokDoubleAt,
	kRlcTokPlus,
	kRlcTokPlusEqual,
	kRlcTokDoublePlus,
	kRlcTokDoubleMinus,
	kRlcTokMinus,
	kRlcTokMinusColon,
	kRlcTokMinusEqual,
	kRlcTokMinusGreater,
	kRlcTokMinusGreaterAsterisk,
	kRlcTokAsterisk,
	kRlcTokAsteriskEqual,
	kRlcTokAnd,
	kRlcTokDoubleAnd,
	kRlcTokDoubleAndEqual,
	kRlcTokAndEqual,
	kRlcTokDoublePipe,
	kRlcTokDoublePipeEqual,
	kRlcTokPipe,
	kRlcTokPipeEqual,
	kRlcTokGreater,
	kRlcTokGreaterEqual,
	kRlcTokLess,
	kRlcTokLessEqual,
	kRlcTokDoubleColonEqual,
	kRlcTokColonEqual,
	kRlcTokDoubleEqual,
	kRlcTokExclamationMarkEqual,
	kRlcTokExclamationMarkColon,
	kRlcTokForwardSlash,
	kRlcTokForwardSlashEqual,
	kRlcTokPercent,
	kRlcTokPercentEqual,
	kRlcTokSemicolon,
	kRlcTokColon,
	kRlcTokDoubleColon,
	kRlcTokQuestionMark,
	kRlcTokTilde,
	kRlcTokTildeColon,
	kRlcTokExclamationMark,
	kRlcTokDoubleGreater,
	kRlcTokDoubleGreaterEqual,
	kRlcTokDoubleLess,
	kRlcTokDoubleLessEqual,
	kRlcTokTripleGreaterEqual,
	kRlcTokTripleGreater,
	kRlcTokTripleLessEqual,
	kRlcTokTripleLess,
	kRlcTokCircumflexEqual,
	kRlcTokCircumflex,

	kRlcTokDollar,
	kRlcTokHash,

	kRlcTokBracketOpen,
	kRlcTokBracketClose,
	kRlcTokParentheseOpen,
	kRlcTokParentheseClose,
	kRlcTokBraceOpen,
	kRlcTokBraceClose,

	kRlcTokIf,
	kRlcTokElse,
	kRlcTokDo,
	kRlcTokWhile,
	kRlcTokFor,
	kRlcTokContinue,
	kRlcTokBreak,
	kRlcTokReturn,
	kRlcTokSwitch,
	kRlcTokCase,
	kRlcTokDefault,
	kRlcTokInline,
	kRlcTokVoid,
	kRlcTokNamespace,
	kRlcTokClass,
	kRlcTokStruct,
	kRlcTokRawtype,
	kRlcTokUnion,
	kRlcTokEnum,
	kRlcTokPublic,
	kRlcTokProtected,
	kRlcTokPrivate,
	kRlcTokAsm,
	kRlcTokStatic,
	kRlcTokVirtual,
	kRlcTokConst,
	kRlcTokVolatile,
	kRlcTokIsolated,
	kRlcTokThis,
	kRlcTokExtern,
	kRlcTokInclude,
	kRlcTokSizeof,

	kRlcTokConstructor,
	kRlcTokDestructor,

	kRlcTokNumber,
	kRlcTokType,

	kRlcTokOperator,

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