#include "file.h"
#include "unicode.h"
#include "malloc.h"
#include "macros.h"
#include "assert.h"

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>

RlcFileResult rlc_read_text_file(char const * filename, rlc_char_t ** out)
{
	RLC_DASSERT(out != NULL);

	size_t len;
	rlc_utf8_t * dst = NULL;

	if(filename)
	{
		FILE * fin = fopen(filename, "r");
		if(!fin)
		{
			printf("%s: could not open.\n", filename);
			return kRlcFileFailRead;
		}
		fseek(fin, 0, SEEK_END);

		len = ftell(fin);
		rewind(fin);

		rlc_malloc((void**)&dst, len + 1);

		fread(dst, len, 1, fin);
		if(feof(fin) || ferror(fin))
		{
			printf("could not read source file %s\n", filename);
			return kRlcFileFailRead;
		}

		fclose(fin);
	} else // we don't know the length of our stdin yet, so we have to read it bit by bit.
	{
		enum { kBufferSize = 1024};
		rlc_utf8_t * buffer = NULL;
		rlc_malloc((void**) &buffer, sizeof(rlc_utf8_t) * kBufferSize);

		while(!feof(stdin))
		{
			size_t in = fread(buffer, sizeof(rlc_utf8_t), kBufferSize, stdin);
			if(ferror(stdin))
			{
				printf("could not read from stdin\n");
				rlc_free((void**)&buffer);
				return kRlcFileFailRead;
			}

			rlc_realloc((void**)&dst, sizeof(rlc_utf8_t) * (len + in + 1));

			memcpy(dst + len, buffer, in);
			len += in;
		}
	}

	dst[len] = 0;

	rlc_utf8_t * del = dst;
	if(dst [0] == 0xef && dst[1] == 0xbb && dst[2] == 0xbf)
	{
		//printf("%s: removing UTF-8 BOM.\n", filename);
		dst += 3, len -= 3;
	} else
	{
		//printf("%s: no UTF-8 BOM detected, still trying to interpret as UTF-8.\n", filename);
	}


	for(rlc_utf8_t * it = dst; *it; it++)
		if(it[0] == '\r')
		{
			if(it[1] == '\n')
			{
				// Remove the \r character from the file.
				for(rlc_utf8_t * jt = it; *jt; jt++)
				{
					jt[0] = jt[1];
				}
			}
			it[0] = '\n';
		}

	*out = rlc_utf8_to_utf32(dst);
	rlc_free((void**)&del);
	if(!*out)
	{
		fprintf(stderr, "%s: file is not UTF-8-encoded.\n", filename);
		return kRlcFileFailEncoding;
	}

	return kRlcFileSuccess;
}

char const * rlc_file_result_message(
	RlcFileResult result)
{
	static char const * s_messages[] = {
		"success",
		"could not open",
		"could not read",
		"invalid encoding"
	};
	RLC_DASSERT((size_t)result <= _countof(s_messages));

	return s_messages[(size_t)result];
}