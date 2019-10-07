#include "../assert.h"

int rlc_parser_eof(
	struct RlcParser const * this)
{
	RLC_DASSERT(this != NULL);
	return !this->fLookaheadSize;
}

int rlc_parser_ahead_eof(
	struct RlcParser const * this)
{
	RLC_DASSERT(this != NULL);
	return this->fLookaheadSize != 2;
}

struct RlcToken const * rlc_parser_current(
	struct RlcParser const * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(!rlc_parser_eof(this));

	return &this->fLookahead[this->fToken];
}

struct RlcToken const * rlc_parser_ahead(
	struct RlcParser const * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(!rlc_parser_ahead_eof(this));

	return &this->fLookahead[this->fToken ^ 1];
}