#pragma once

RlcSrcIndex rlc_src_string_end(
	struct RlcSrcString const * this)
{
	RLC_DASSERT(this != NULL);
	return this->start + this->length;
}