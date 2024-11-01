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
	kRlcTokFloatLiteral,
	kRlcTokCharacterLiteral,
	kRlcTokStringLiteral,

	// Keywords.
	kRlcTokAbstract,
	kRlcTokAssert,
	kRlcTokBare,
	kRlcTokBreak,
	kRlcTokBswap,
	kRlcTokCatch,
	kRlcTokCommon,
	kRlcTokContinue,
	kRlcTokCopyRtti,
	kRlcTokDefault,
	kRlcTokDestructor,
	kRlcTokDie,
	kRlcTokDo,
	kRlcTokElse,
	kRlcTokEnum,
	kRlcTokExtern,
	kRlcTokFinal,
	kRlcTokFinally,
	kRlcTokFor,
	kRlcTokIf,
	kRlcTokInclude,
	kRlcTokInline,
	kRlcTokMask,
	kRlcTokNoinit,
	kRlcTokNull,
	kRlcTokNumber,
	kRlcTokOr,
	kRlcTokOverride,
	kRlcTokPopcnt,
	kRlcTokPrivate,
	kRlcTokProtected,
	kRlcTokPublic,
	kRlcTokReturn,
	kRlcTokSizeof,
	kRlcTokStatic,
	kRlcTokSwitch,
	kRlcTokTest,
	kRlcTokThis,
	kRlcTokThrow,
	kRlcTokTry,
	kRlcTokType,
	kRlcTokUnion,
	kRlcTokVirtual,
	kRlcTokVisit,
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

	kRlcTokTripleAnd,
	kRlcTokDoubleAndEqual,
	kRlcTokDoubleAnd,
	kRlcTokAndEqual,
	kRlcTokAnd,

	kRlcTokDoublePipeEqual,
	kRlcTokDoublePipe,
	kRlcTokPipeEqual,
	kRlcTokPipe,

	kRlcTokDoubleQuestionMark,
	kRlcTokQuestionMark,

	kRlcTokDoubleColonEqual,
	kRlcTokColonEqual,
	kRlcTokDoubleColon,
	kRlcTokColon,
	kRlcTokDoubleAt,
	kRlcTokAt,
	kRlcTokTripleDot,
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
	kRlcTokLessMinus,
	kRlcTokLessGreater,
	kRlcTokLess,

	kRlcTokTripleGreaterEqual,
	kRlcTokTripleGreater,
	kRlcTokDoubleGreaterEqual,
	kRlcTokDoubleGreater,
	kRlcTokGreaterEqual,
	kRlcTokGreater,

	kRlcTokDollar,
	kRlcTokTripleHash,
	kRlcTokDoubleHash,
	kRlcTokHash,
	RLC_ENUM_END(RlcTokenType)

};

#define RLC_INLINE_SHORTHAND kRlcTokDoubleMinus

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

#ifdef __cplusplus
}
#endif

#endif