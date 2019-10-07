#include "parser.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_parser_create(
	struct RlcParser * this,
	struct RlcSrcFile * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	rlc_tokeniser_create(
		&this->fTokeniser,
		file);

	this->fTracer = NULL;

	if(rlc_tokeniser_read(&this->fTokeniser, &this->fLookahead[0]))
	{
		this->fEnd = !rlc_tokeniser_read(&this->fTokeniser, &this->fLookahead[1]);
		this->fLookaheadSize = 2;
	} else
	{
		this->fEnd = 1;
		this->fLookaheadSize = 1;
	}
}

void rlc_parser_destroy(
	struct RlcParser * this)
{
	RLC_DASSERT(this != NULL);

	if(!rlc_parser_eof(this))
	{
		struct RlcSrcPosition pos;
		rlc_src_file_position(
			this->fTokeniser.fSource,
			pos,
			rlc_parser_current(this)->content.start);

		fprintf(stderr, "%s:%u:%u: error: unexpected '%s'.\n",
			this->fTokeniser.fSource->fName,
			pos.line,
			pos.column,
			rlc_src_string_cstr(&rlc_parser_current(this)->content));
		exit(1);
	}
}

int rlc_parser_next(
	struct RlcParser * this)
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


struct RlcToken const * rlc_parser_current(
	struct RlcParser const * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fIndex < this->fFile->fTokenCount)
		return this->fFile->fTokens[this->fIndex];
	else
		return NULL;
}

struct RlcToken const * rlc_parser_latest(
	struct RlcParser const * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fLatestIndex < this->fFile->fTokenCount)
		return this->fFile->fTokens[this->fLatestIndex];
	else
		return NULL;
}

struct RlcToken const * rlc_parser_ahead(
	struct RlcParser const * this)
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

int rlc_parser_match(
	struct RlcParser const * this,
	enum RlcTokenType type)
{
	RLC_DASSERT(this != NULL);

	return NULL != static_match_token(
		rlc_parser_current(this),
		type);
}

struct RlcToken const * rlc_parser_match_ahead(
	struct RlcParser const * this,
	enum RlcTokenType type)
{
	RLC_DASSERT(this != NULL);

	return static_match_token(
		rlc_parser_ahead(this),
		type);
}

int rlc_parser_consume(
	struct RlcParser * this,
	enum RlcTokenType type)
{
	RLC_DASSERT(this != NULL);

	int ret = rlc_parser_match(
		this,
		type);

	if(ret)
		rlc_parser_next(this);

	return ret;
}

void rlc_parser_destroy(
	struct RlcParser * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parser_clear_errors(this);

	if(this->fFile)
	{
		rlc_preprocessed_file_destroy(this->fFile);
		rlc_free((void**)&this->fFile);
	}

	this->fIndex = 0;
	this->fLatestIndex = 0;
}

void rlc_parser_add_error(
	struct RlcParser * this,
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

void rlc_parser_clear_errors(
	struct RlcParser * this)
{
	if(this->fErrors)
		rlc_free((void**)&this->fErrors);
	this->fErrorCount = 0;
}

size_t rlc_parser_matched_index(
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);

	return parser->fIndex;
}

size_t rlc_parser_consumed_index(
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(parser->fIndex > 0);

	return parser->fIndex - 1;
}

int rlc_parser_equal_tokens(
	struct RlcParser const * parser,
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