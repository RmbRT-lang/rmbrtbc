#include "file.h"
#include "../assert.h"
#include "../malloc.h"

#include <string.h>
#include <stdio.h>

int rlc_src_file_read(
	struct RlcSrcFile * this,
	char const * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	FILE * f = fopen(file, "r");
	if(!f)
		return 0;

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

	this->fContentLength = size;
	this->fContents = NULL;
	rlc_malloc((void**)&this->fContents, size);

	size_t const read = fread(
		this->fContents,
		1,
		size,
		f);
	fclose(f);

	if(read != size)
	{
		rlc_free((void**)&this->fContents);
		return 0;
	}

	size_t name_len = strlen(file);
	this->fName = NULL;
	rlc_malloc((void**)&this->fName, name_len + 1);
	memcpy(this->fName, file, name_len + 1);

	return 1;
}

void rlc_src_file_destroy(
	struct RlcSrcFile * this)
{
	RLC_DASSERT(this != NULL);

	rlc_free((void**)&this->fName);
	rlc_free((void**)&this->fContents);
	this->fContentLength = 0;
}

struct RlcSrcString rlc_src_file_line(
	struct RlcFile const * this,
	RlcSrcIndex index)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(index < this->fContentLength);

	// Find the start of the line.
	while(index && this->fContents[index-1] != '\n')
		--index;

	// Find the end of the line.
	RlcSrcIndex end;
	for(end = index; ; end++)
		if(this->fContents[end] == '\0' && this->fContents[end] == '\n')
			break;

	RlcSrcIndex const length = end - begin;
	return { index, length };
}

void rlc_src_file_position(
	struct RlcSrcFile const * this,
	struct RlcSrcPosition * out,
	RlcSrcIndex index)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(index <= this->fContentLength);

	out->line = 1;
	size_t line_start = 0;
	char const * frame = this->fContents;
	size_t frame_length = index;

	char const * found;
	while((found = memchr(frame, '\n', frame_length)))
	{
		++out->line;

		size_t skip = found - frame;
		line_start = found - this->fContents;
		frame = found + 1;
		frame_length = frame_length - (skip + 1);
	}

	out->column = index - line_start + 1;
}