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
	kRlcTokIdentifier,
	kRlcTokHexNumber,
	kRlcTokDecimalNumber,
	kRlcTokOctalNumber,
	kRlcTokCharNumber,
	kRlcTokUtf8CharNumber,
	kRlcTokUtf16CharNumber,
	kRlcTokUtf32CharNumber,
	kRlcTok0,
	kRlcTokString,
	kRlcTokUtf8String,
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
	kRlcTokTripleDot,
	kRlcTokPlus,
	kRlcTokPlusEqual,
	kRlcTokDoublePlus,
	kRlcTokDoubleMinus,
	kRlcTokMinus,
	kRlcTokMinusColon,
	kRlcTokMinusEqual,
	kRlcTokMinusGreater,
	kRlcTokAsterisk,
	kRlcTokAsteriskEqual,
	kRlcTokAnd,
	kRlcTokDoubleAnd,
	kRlcTokAndEqual,
	kRlcTokDoublePipe,
	kRlcTokPipe,
	kRlcTokPipeEqual,
	kRlcTokGreater,
	kRlcTokGreaterEqual,
	kRlcTokLess,
	kRlcTokLessEqual,
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

	kRlcTokBracketL,
	kRlcTokBracketR,
	kRlcTokParentheseL,
	kRlcTokParentheseR,
	kRlcTokBraceL,
	kRlcTokBraceR,

	kRlcTokIf,
	kRlcTokElse,
	kRlcTokWhile,
	kRlcTokFor,
	kRlcTokContinue,
	kRlcTokBreak,
	kRlcTokReturn,
	kRlcTokSwitch,
	kRlcTokCase,
	kRlcTokFn,
	kRlcTokInline,
	kRlcTokVoid,
	kRlcTokNamespace,
	kRlcTokClass,
	kRlcTokStruct,
	kRlcTokRawtype,
	kRlcTokUnion,
	kRlcTokTypedef,
	kRlcTokPublic,
	kRlcTokProtected,
	kRlcTokPrivate,
	kRlcTokAsm,
	kRlcTokStatic,
	kRlcTokDynamic,
	kRlcTokVirtual,
	kRlcTokConst,
	kRlcTokVolatile,
	kRlcTokIsolated,
	kRlcTokThis,

	RLC_ENUM_END(RlcTokenType)

};

/** Looks up the name belonging to the given token type.
@param[in] type:
	The token type whose name to retrieve.
@return
	The token type's name. */
char const * rlc_token_type_name(
	enum RlcTokenType type);

/** The result code of a tokenising function. */
enum RlcTokResult
{
	kRlcTokResultOk,
	kRlcTokResultEos,
	kRlcTokResultInvalidHexDigit,
	kRlcTokResultInvalidOctalDigit,
	kRlcTokResultInvalidDecimalDigit,
	kRlcTokResultInvalidFloatingSpecifier,
	kRlcTokResultUnclosedLiteral,
	kRlcTokResultInvalidHexEscape,
	kRlcTokResultInvalidOctalEscape,
	kRlcTokResultInvalidDecimalEscape,
	kRlcTokResultInvalidEscape,
	kRlcTokResultInvalidUtf8,
	kRlcTokResultEmptyCharLiteral,

	kRlcTokResultUnexpected,

	RLC_ENUM_END(RlcTokResult)
};

/** Retrieves a description of an error code.
	The description is a short english string that names the cause of the error. */
char const * rlc_tok_result_message(
	enum RlcTokResult result);

/** Stores the contents of a tokenised file. */
struct RlcFile
{
	/** The file name. */
	rlc_char_t const * fName;
	/** The file's tokens. */
	struct RlcToken * fTokens;
	/** The token count. */
	size_t fTokenCount;
};

/** A token as used by the tokenizer.
	It represents a substring of a source file. */
struct RlcToken
{
	/** The beginning inde of the substring. */
	size_t fBegin;
	/** The byte length of the substring. */
	size_t fLength;
	/** Stores a rlcTokenType. */
	unsigned char fType;
	/** The source file of the token. */
	struct RlcFile const * fFile;
};

#ifdef __cplusplus
}
#endif

#endif