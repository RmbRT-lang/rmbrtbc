#include "parser.h"

#include "../malloc.h"
#include "../assert.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void rlc_parser_create(
	struct RlcParser * this,
	struct RlcSrcFile const * file)
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
			&pos,
			rlc_parser_index(this));

		fprintf(stderr, "%s:%u:%u: error: unexpected '%s'.\n",
			this->fTokeniser.fSource->fName,
			pos.line,
			pos.column,
			rlc_src_string_cstr(
				&rlc_parser_current(this)->content,
				this->fTokeniser.fSource));
		exit(1);
	}
}

void rlc_parser_trace(
	struct RlcParser * this,
	char const * context,
	struct RlcParserTracer * tracer)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(context != NULL);
	RLC_DASSERT(tracer != NULL);

	tracer->fPrevious = this->fTracer;
	tracer->fContext = context;
	this->fTracer = tracer;
}

void rlc_parser_untrace(
	struct RlcParser * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->fTracer != NULL);

	this->fTracer = this->fTracer->fPrevious;
}

char const * rlc_parser_context(
	struct RlcParser const * this)
{
	RLC_DASSERT(this != NULL);

	return this->fTracer
		? this->fTracer->fContext
		: "global scope";
}

RlcSrcIndex rlc_parser_index(
	struct RlcParser const * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(!rlc_parser_eof(this));

	return rlc_parser_current(this)->content.start;
}

int rlc_parser_is_current(
	struct RlcParser const * this,
	enum RlcTokenType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(!rlc_parser_eof(this));

	return rlc_parser_current(this)->type == type;
}

int rlc_parser_is_ahead(
	struct RlcParser const * this,
	enum RlcTokenType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(!rlc_parser_ahead_eof(this));

	return rlc_parser_ahead(this)->type == type;
}

_Noreturn void rlc_parser_fail(
	struct RlcParser * parser,
	char const * reason)
{
	struct RlcSrcPosition pos;
	if(rlc_parser_eof(parser))
	{
		rlc_src_file_position(
			parser->fTokeniser.fSource,
			&pos,
			parser->fTokeniser.fSource->fContentLength);

		fprintf(stderr, "%s:%u:%u: error: unexpected end of file in %s: %s.\n",
			parser->fTokeniser.fSource->fName,
			pos.line,
			pos.column,
			rlc_parser_context(parser),
			reason);
	} else
	{
		rlc_src_file_position(
			parser->fTokeniser.fSource,
			&pos,
			rlc_parser_index(parser));

		fprintf(stderr, "%s:%u:%u: error: unexpected '%s' in %s: %s.\n",
			parser->fTokeniser.fSource->fName,
			pos.line,
			pos.column,
			rlc_src_string_cstr(
				&rlc_parser_current(parser)->content,
				parser->fTokeniser.fSource),
			rlc_parser_context(parser),
			reason);
	}

	fflush(stderr);
	exit(EXIT_FAILURE);
}

int rlc_parser_consume(
	struct RlcParser * this,
	struct RlcToken * token,
	enum RlcTokenType type)
{
	RLC_DASSERT(this != NULL);

	int ret = rlc_parser_is_current(
		this,
		type);

	if(ret)
	{
		if(token)
			*token = *rlc_parser_current(this);
		rlc_parser_skip(this);
	}

	return ret;
}

enum RlcTokenType rlc_parser_expect(
	struct RlcParser * this,
	struct RlcToken * token,
	size_t count,
	enum RlcTokenType types, ...)
{
	va_list args;
	va_start(args, types);

	struct RlcToken const * current = rlc_parser_current(this);


	size_t const count_ = count;
check:
	if(current->type == types)
	{
		if(token)
			*token = *current;
		return types;
	}

	if(count--)
	{
		types = va_arg(args, enum RlcTokenType);
		goto check;
	}

	struct RlcSrcPosition pos;
	if(rlc_parser_eof(this))
	{
		rlc_src_file_position(
			this->fTokeniser.fSource,
			&pos,
			this->fTokeniser.fSource->fContentLength);

		fprintf(stderr, "%s:%u:%u: error: unexpected end of file",
			this->fTokeniser.fSource->fName,
			pos.line,
			pos.column);
	} else
	{
		rlc_src_file_position(
			this->fTokeniser.fSource,
			&pos,
			rlc_parser_index(this));

		fprintf(stderr, "%s:%u:%u: error: unexpected '%s'",
			this->fTokeniser.fSource->fName,
			pos.line,
			pos.column,
			rlc_src_string_cstr(
				&rlc_parser_current(this)->content,
				this->fTokeniser.fSource));
	}

	fprintf(stderr, " in %s: expected %s",
		rlc_parser_context(this),
		rlc_token_type_name(types));

	va_start(args, types);
	for(size_t i = 0; i < count_; i++)
	{
		types = va_arg(args, enum RlcTokenType);
		fprintf(
			stderr,
			(i == count-1)
				? ", or %s"
				: ", %s",
			rlc_token_type_name(types));
	}

	exit(EXIT_FAILURE);
}

void rlc_parser_skip(
	struct RlcParser * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(!rlc_parser_eof(this));

	if(this->fEnd)
	{
		this->fLookaheadSize--;
	} else {
		this->fEnd = !rlc_tokeniser_read(
			&this->fTokeniser,
			&this->fLookahead[this->fToken]);
	}
	this->fToken ^= 1;
}

_Nodiscard int rlc_parser_equal_tokens(
	struct RlcParser const * parser,
	struct RlcToken const * lhs,
	struct RlcToken const * rhs)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(lhs != NULL);
	RLC_DASSERT(rhs != NULL);

	return 0 == rlc_src_string_cmp(
		parser->fTokeniser.fSource,
		&lhs->content,
		&rhs->content);
}