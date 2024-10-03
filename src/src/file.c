#include "file.h"
#include "../assert.h"
#include "../malloc.h"
#include "../unicode.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

	size_t const k_file_limit = ((size_t)1u << (8*sizeof(RlcSrcIndex)));
	if(size >= k_file_limit)
	{
		fprintf(stderr, "%s:1:1: error: file exceeds limit of %zu KiB.\n",
			file, k_file_limit / 1024);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	this->fContentLength = size;
	this->fContentData = NULL;
	rlc_malloc((void**)&this->fContentData, size+1);

	size_t const read = fread(
		this->fContentData,
		1,
		size,
		f);
	fclose(f);

	if(read != size)
	{
		rlc_free((void**)&this->fContentData);
		return 0;
	}

	this->fContentData[this->fContentLength] = '\0';

	this->fLineCount = 0;
	for(
		char const * start = strchr(this->fContentData, '\n');
		start != NULL;
		start = strchr(start+1, '\n')
	) {
		this->fLineCount++;
	}
	if(this->fContentData[this->fContentLength-1] != '\n')
		this->fLineCount++;


	// Detect and skip UTF-8 BOM.
	if(!strncmp(this->fContentData, "\xEF\xBB\xBF", 3))
	{
		this->fContents = this->fContentData + 3;
		this->fContentLength -= 3;
	}
	else
		this->fContents = this->fContentData;

	if(!rlc_utf8_is_valid_string((rlc_utf8_t const *)this->fContents))
	{
		fprintf(stderr, "%s:1:1: error: file is not UTF-8 encoded.\n", file);
		fflush(stderr);
		exit(EXIT_FAILURE);
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
	rlc_free((void**)&this->fContentData);
	this->fContents = NULL;
	this->fContentLength = 0;
}

struct RlcSrcString rlc_src_file_line(
	struct RlcSrcFile const * this,
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

	RlcSrcIndex const length = end - index;
	struct RlcSrcString ret = { index, length, index, 1 };
	return ret;
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
		line_start = found+1 - this->fContents;
		frame = found + 1;
		frame_length = frame_length - (skip + 1);
	}

	out->column = index - line_start + 1;
}