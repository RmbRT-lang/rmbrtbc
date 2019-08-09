#include "parser.h"

#include "../malloc.h"
#include "../assert.h"

char const * rlc_parse_error_msg(
	enum RlcParseError error)
{
	RLC_DASSERT(RLC_IN_ENUM(error, RlcParseError));
	static char * error_msg[] = {
		"Expected Identifier",
		"Expected Type Name",
		"Forbidden Type Qualifier After Dynamic Indirection",
		"Expected Type Modifier",
		"Forbidden Dynamic Indirection",
		"Expected Function Signature",
		"Expected ';'",
		"Expected ':'",
		"Expected ','",
		"Expected Symbol",
		"Expected '{'",
		"Expected Enum Constant",
		"Expected '}'",
		"Expected '['",
		"Expected ']'",
		"Expected ':='",
		"Expected ':=' or ':'",
		"Forbidden '::='",
		"Expected '('",
		"Expected ')'",
		"Expected Template Declaration",
		"Forbidden Template Declaration",
		"Expected Expression",
		"Expected Number Expression",
		"Expected String Expression",
		"Expected Symbol Expression",
		"Expected Symbol Child Expression",
		"Expected Type Name Expression",
		"Expected Operator Expression",
		"Expected Argument",
		"Expected Member Variable",
		"Expected Member Function",
		"Expected Member Class",
		"Expected Member Union",
		"Expected Member Struct",
		"Expected Member Rawtype",
		"Expected Member Typedef",
		"Expected Constructor",
		"Expected Destructor",
		"Expected Class Member",
		"Expected Struct Member",
		"Expected Expression Or Variable",
		"Expected Variable",
		"Expected Function",
		"Expected Class",
		"Expected Union",
		"Expected Struct",
		"Expected Rawtype",
		"Expected Typedef",
		"Expected Namespace",
		"Expected Enum",
		"Expected Scope Entry",
		"Expected Statement",
		"Expected Template Decl Type",
		"Expected Expression Statement",
		"Expected Block Statement",
		"Expected If Statement",
		"Expected Loop Statement",
		"Expected Variable Statement",
		"Expected Return Statement",
		"Expected Control Label",
		"Unexpected Control Label",
		"Mismatched Control Label Name",
		"Expected Loop Initial",
		"Expected For Head",
		"Expected While Head",
		"Expected For/While Head",
		"Expected Body Statement",
		"Expected Initialiser",
		"Redefined Destructor",
		"Destructor Template Arguments"
	};

	static_assert(RLC_COVERS_ENUM(error_msg, RlcParseError), "lookup table");

	return error_msg[error];
}

void rlc_parser_data_create(
	struct RlcParserData * this,
	struct RlcPreprocessedFile * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	this->fErrors = NULL;
	this->fErrorCount = 0;

	this->fFile = file;
	this->fIndex = 0;
	this->fLatestIndex = 0;
}

int rlc_parser_data_next(
	struct RlcParserData * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->fFile != NULL);

	if(this->fIndex < this->fFile->fTokenCount)
	{
		++ this->fIndex;
		if(this->fLatestIndex < this->fIndex)
			this->fLatestIndex = this->fIndex;
		return 1;
	} else
		return 0;
}


struct RlcToken const * rlc_parser_data_current(
	struct RlcParserData const * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fIndex < this->fFile->fTokenCount)
		return this->fFile->fTokens[this->fIndex];
	else
		return NULL;
}

struct RlcToken const * rlc_parser_data_latest(
	struct RlcParserData const * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fLatestIndex < this->fFile->fTokenCount)
		return this->fFile->fTokens[this->fLatestIndex];
	else
		return NULL;
}

struct RlcToken const * rlc_parser_data_ahead(
	struct RlcParserData const * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fIndex + 1 < this->fFile->fTokenCount)
		return this->fFile->fTokens[this->fIndex + 1];
	else
		return NULL;
}

static struct RlcToken const * static_match_token(
	struct RlcToken const * token,
	enum RlcTokenType type)
{
	RLC_DASSERT(RLC_IN_ENUM(type, RlcTokenType));

	if(token != NULL
	&& token->fType == type)
		return token;
	else
		return NULL;
}

int rlc_parser_data_match(
	struct RlcParserData const * this,
	enum RlcTokenType type)
{
	RLC_DASSERT(this != NULL);

	return NULL != static_match_token(
		rlc_parser_data_current(this),
		type);
}

struct RlcToken const * rlc_parser_data_match_ahead(
	struct RlcParserData const * this,
	enum RlcTokenType type)
{
	RLC_DASSERT(this != NULL);

	return static_match_token(
		rlc_parser_data_ahead(this),
		type);
}

int rlc_parser_data_consume(
	struct RlcParserData * this,
	enum RlcTokenType type)
{
	RLC_DASSERT(this != NULL);

	int ret = rlc_parser_data_match(
		this,
		type);

	if(ret)
		rlc_parser_data_next(this);

	return ret;
}

void rlc_parser_data_destroy(
	struct RlcParserData * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parser_data_clear_errors(this);

	if(this->fFile)
	{
		rlc_preprocessed_file_destroy(this->fFile);
		rlc_free((void**)&this->fFile);
	}

	this->fIndex = 0;
	this->fLatestIndex = 0;
}

void rlc_parser_data_add_error(
	struct RlcParserData * this,
	enum RlcParseError error_message)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(error_message, RlcParseError));

	rlc_realloc(
		(void**)&this->fErrors,
		sizeof(struct RlcParseErrorMessage) * ++this->fErrorCount);

	struct RlcParseErrorMessage * msg = &this->fErrors[this->fErrorCount-1];

	msg->fError = error_message;
	msg->fLocation = this->fIndex;
}

void rlc_parser_data_clear_errors(
	struct RlcParserData * this)
{
	if(this->fErrors)
		rlc_free((void**)&this->fErrors);
	this->fErrorCount = 0;
}

size_t rlc_parser_data_matched_index(
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);

	return parser->fIndex;
}

size_t rlc_parser_data_consumed_index(
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(parser->fIndex > 0);

	return parser->fIndex - 1;
}

int rlc_parser_data_equal_tokens(
	struct RlcParserData const * parser,
	size_t lhs,
	size_t rhs)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(lhs < parser->fFile->fTokenCount);
	RLC_DASSERT(rhs < parser->fFile->fTokenCount);

	if(lhs == rhs)
		return 1;

	struct RlcToken const * lt, * rt;
	lt = parser->fFile->fTokens[lhs];
	rt = parser->fFile->fTokens[rhs];

	if(lt->fType != rt->fType
	|| lt->fLength != rt->fLength)
	{
		return 0;
	} else
	{
		rlc_char_t * file = 0;
		return 0 == rlc_strncmp(
			lt->fFile->fContents+lt->fBegin,
			rt->fFile->fContents+rt->fBegin,
			lt->fLength);
	}
}