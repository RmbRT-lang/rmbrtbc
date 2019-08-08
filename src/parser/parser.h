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
	/** Forbidden type qualifier after dynamic indirection. */
	kRlcParseErrorForbiddenTypeQualifier,
	kRlcParseErrorExpectedTypeModifier,
	kRlcParseErrorForbiddenDynamic,
	/** Expected a function signature. */
	kRlcParseErrorExpectedFunctionSignature,
	/** Expected a semicolon. */
	kRlcParseErrorExpectedSemicolon,
	kRlcParseErrorExpectedColon,
	/** Expected a ','. */
	kRlcParseErrorExpectedComma,
	/** Expected a symbol. */
	kRlcParseErrorExpectedSymbol,
	/** Expected a '{'. */
	kRlcParseErrorExpectedBraceOpen,
	/** Expected an enum constant. */
	kRlcParseErrorExpectedEnumConstant,
	/** Expected a '}'. */
	kRlcParseErrorExpectedBraceClose,

	kRlcParseErrorExpectedBracketOpen,
	kRlcParseErrorExpectedBracketClose,

	kRlcParseErrorExpectedColonEqual,
	kRlcParseErrorExpectedColonEqualOrColon,
	kRlcParseErrorForbiddenDoubleColonEqual,

	kRlcParseErrorExpectedParentheseOpen,
	/** Expected a ')'. */
	kRlcParseErrorExpectedParentheseClose,

	kRlcParseErrorExpectedTemplateDeclaration,
	kRlcParseErrorForbiddenTemplateDeclaration,

	/** Expected an expression. */
	kRlcParseErrorExpectedExpression,

	kRlcParseErrorExpectedNumberExpression,
	kRlcParseErrorExpectedStringExpression,
	kRlcParseErrorExpectedSymbolExpression,
	kRlcParseErrorExpectedSymbolChildExpression,
	kRlcParseErrorExpectedTypeNameExpression,
	kRlcParseErrorExpectedOperatorExpression,

	kRlcParseErrorExpectedArgument,

	kRlcParseErrorExpectedMemberVariable,
	kRlcParseErrorExpectedMemberFunction,
	kRlcParseErrorExpectedMemberClass,
	kRlcParseErrorExpectedMemberUnion,
	kRlcParseErrorExpectedMemberStruct,
	kRlcParseErrorExpectedMemberRawtype,
	kRlcParseErrorExpectedMemberTypedef,
	kRlcParseErrorExpectedConstructor,
	kRlcParseErrorExpectedDestructor,

	kRlcParseErrorExpectedClassMember,
	kRlcParseErrorExpectedStructMember,

	kRlcParseErrorExpectedExpressionOrVariable,
	kRlcParseErrorExpectedVariable,
	kRlcParseErrorExpectedFunction,
	kRlcParseErrorExpectedClass,
	kRlcParseErrorExpectedUnion,
	kRlcParseErrorExpectedStruct,
	kRlcParseErrorExpectedRawtype,
	kRlcParseErrorExpectedTypedef,
	kRlcParseErrorExpectedNamespace,
	kRlcParseErrorExpectedEnum,

	kRlcParseErrorExpectedScopeEntry,

	kRlcParseErrorExpectedStatement,
	kRlcParseErrorExpectedTemplateDeclType,
	kRlcParseErrorExpectedExpressionStatement,
	kRlcParseErrorExpectedBlockStatement,
	kRlcParseErrorExpectedIfStatement,
	kRlcParseErrorExpectedLoopStatement,
	kRlcParseErrorExpectedVariableStatement,
	kRlcParseErrorExpectedReturnStatement,

	kRlcParseErrorExpectedLoopInitial,
	kRlcParseErrorExpectedForHead,
	kRlcParseErrorExpectedWhileHead,
	kRlcParseErrorExpectedLoopHead,

	/** If/else body. */
	kRlcParseErrorExpectedBodyStatement,

	kRlcParseErrorExpectedInitialiser,

	kRlcParseErrorDoubleDestructor,
	kRlcParseErrorDestructorTemplate,

	RLC_ENUM_END(RlcParseError)
};

char const * rlc_parse_error_msg(
	enum RlcParseError error);

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
/** Creates a parser for a file.
@memberof RlcParserData
@param[out] this:
	The parser to create.
	@dassert @nonnull
@param[in] file:
	The file to parse.
	@dassert @nonnull */
void rlc_parser_data_create(
	struct RlcParserData * this,
	struct RlcPreprocessedFile * file);

/** Goes to the next token.
@memberof RlcParserData
@param[in,out] this:
	The parser data.
@return
	Whether there is a next token. */
int rlc_parser_data_next(
	struct RlcParserData * this);

/** Returns the address of the current token, if not at the end of the token stream.
@memberof RlcParserData
@param[in] this:
	The parser data.
	@dassert @nonnull */
struct RlcToken const * rlc_parser_data_current(
	struct RlcParserData const * this);

struct RlcToken const * rlc_parser_data_ahead(
	struct RlcParserData const * this);

/** Matches a token.
@memberof RlcParserData
@param[in] this:
	The parser data.
@param[in] type:
	The token type to match.
@return
	Nonzero if matched. */
int rlc_parser_data_match(
	struct RlcParserData const * this,
	enum RlcTokenType type);

struct RlcToken const * rlc_parser_data_match_ahead(
	struct RlcParserData const * this,
	enum RlcTokenType type);

/** Tries to match a token of the given type, and on success, consumes it.
@memberof RlcParserData
@param[in,out] this:
	The parser data.
@param[in] type:
	The token type to match.
@return
	Nonzero if a token was matched. */
int rlc_parser_data_consume(
	struct RlcParserData * this,
	enum RlcTokenType type);

/** Destroys parser data.
@memberof RlcParserData
@param[in] this:
	The parser data to destroy.
	@dassert @nonnull */
void rlc_parser_data_destroy(
	struct RlcParserData * this);

/** Adds an error to a parser data.
@memberof RlcParserData
@param[in,out] this:
	The parser data to add an error to.
@param[in] error_message:
	The error message to add. */
void rlc_parser_data_add_error(
	struct RlcParserData * this,
	enum RlcParseError error_message);

/** Clears all errors from the parser.
@param[in,out] this:
	The parser data whose errors to clear. */
void rlc_parser_data_clear_errors(
	struct RlcParserData * this);

/** Returns the index of the matched token.
@memberof RlcParserData
@param[in] parser:
	The parser that matched a token.
	@dassert @nonnull
@return
	The matched token's index. */
size_t rlc_parser_data_matched_index(
	struct RlcParserData * parser);

/** Returns the index of the consumed token.
@memberof RlcParserData
@param[in] parser:
	The parser that consumed a token.
	@dassert @nonnull
@return
	The consumed token's index. */
size_t rlc_parser_data_consumed_index(
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif


#endif