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
	kRlcTokAbstract,
	kRlcTokBreak,
	kRlcTokCase,
	kRlcTokCatch,
	kRlcTokConstructor,
	kRlcTokContinue,
	kRlcTokDefault,
	kRlcTokDestructor,
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
	kRlcTokNamespace,
	kRlcTokNoinit,
	kRlcTokNumber,
	kRlcTokOperator,
	kRlcTokOverride,
	kRlcTokPrivate,
	kRlcTokProtected,
	kRlcTokPublic,
	kRlcTokReturn,
	kRlcTokSizeof,
	kRlcTokStatic,
	kRlcTokSwitch,
	kRlcTokThis,
	kRlcTokTry,
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

#ifdef __cplusplus
}
#endif

#endif