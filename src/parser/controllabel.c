#include "controllabel.h"
#include "../scoper/string.h"
#include "../assert.h"

void rlc_parsed_control_label_parse(
	struct RlcParsedControlLabel * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);

	out->fExists = 0;
	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBracketOpen))
		return;

	rlc_parser_expect(
		parser,
		&out->fLabel,
		2,
		kRlcTokIdentifier, kRlcTokStringLiteral);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBracketClose);

	out->fExists = 1;
}

static char const hex[] = "0123456789abcdef";
void rlc_parsed_control_label_print_name(
	struct RlcParsedControlLabel const * this,
	struct RlcSrcFile const * file,
	FILE * out,
	char const * suffix)
{
	if(!this->fExists)
		return;

	if(this->fLabel.type == kRlcTokStringLiteral)
	{
		fputc('_', out);
		struct RlcScopedText text;
		rlc_scoped_text_create(&text, file, &this->fLabel);
		for(int i = 0; i < text.fElements * text.fSymbolSize; i++)
		{
			char const * byte = ((char*)text.fRaw)+i;
			fputc(hex[*byte & 0xf], out);
			fputc(hex[(*byte >> 4) & 0xf], out);
		}
		rlc_scoped_text_destroy(&text);
	} else
	{
		rlc_src_string_print(&this->fLabel.content, file, out);
	}

	if(suffix)
		fputs(suffix, out);
}

void rlc_parsed_control_label_print(
	struct RlcParsedControlLabel const * this,
	struct RlcSrcFile const * file,
	FILE * out,
	char const * suffix)
{
	if(!this->fExists)
		return;

	rlc_parsed_control_label_print_name(this, file, out, suffix);

	fputs(":{;}\n", out);
}