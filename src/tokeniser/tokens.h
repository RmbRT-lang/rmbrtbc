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
	kRlcTokDotAsterisk,
	kRlcTokTripleDot,
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
	kRlcTokFn,
	kRlcTokInline,
	kRlcTokVoid,
	kRlcTokNamespace,
	kRlcTokClass,
	kRlcTokStruct,
	kRlcTokRawtype,
	kRlcTokUnion,
	kRlcTokTypedef,
	kRlcTokEnum,
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
	kRlcTokResultUnclosedComment,

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
	char const * fName;
	/** The file's contents. */
	rlc_char_t * fContents;
	/** The file's content's length. */
	size_t fContentLength;
	/** The file's tokens. */
	struct RlcToken * fTokens;
	/** The token count. */
	size_t fTokenCount;
};

void rlc_file_create(
	struct RlcFile * this);

void rlc_file_destroy(
	struct RlcFile * this);

void rlc_file_set_name(
	struct RlcFile * this,
	rlc_char_t const * fName);

void rlc_file_add_token(
	struct RlcFile * this,
	struct RlcToken const * token);

/** Retrieves a line of code of a source file.
@param[in] this:
	The file whose line to read.
	@dassert @nonnull
@param[in] begin:
	The character index the line starts with.
	@dassert `begin < rlc_strlen(this->contents)`
@return
	A copy of the source line. The newline character is excluded. */
rlc_char_t * rlc_file_get_line_contents(
	struct RlcFile const * this,
	size_t begin);

/** A token as used by the tokenizer and preprocessor.
	It represents a substring of a source file. */
struct RlcToken
{
	/** The beginning inde of the substring. */
	size_t fBegin;
	/** The byte length of the substring. */
	size_t fLength;
	/** Stores a rlcTokenType. */
	enum RlcTokenType fType;
	/** The source file of the token. */
	struct RlcFile const * fFile;
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

rlc_char_t * rlc_token_content(
	struct RlcToken const * this);

#ifdef __cplusplus
}
#endif

#endif