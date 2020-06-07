#include "scoper/fileregistry.h"
#include "printer.h"
#include "unicode.h"
#include "malloc.h"
#include "fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <linux/limits.h>
#include <unistd.h>
#include <sys/socket.h>

void pipe_file(
	char const * src,
	int pipe)
{
	char buf[1024];
	FILE * f = fopen(src, "r");
	if(!f)
	{
		perror("fopen");
		puts(src);
		puts("\n");
		abort();
	}

	while(!feof(f))
	{
		size_t size = fread(buf, 1, sizeof(buf), f);
		if(!size)
		{
			perror("fread");
			exit(1);
		}

		char const * p = buf;
		while(size)
		{
			size_t sent = write(pipe, p, size);
			if(0 == sent)
			{
				perror("send");
				exit(1);
			}
			p += sent;
			size -= sent;
		}
	}
}

void read_into_pipe_and_close(FILE* fd, char ** contents, size_t * size, int pipe)
{
	fclose(fd);
	char const * p = *contents;
	while(*size)
	{
		size_t sent = write(pipe, p, *size);
		if(0 == sent)
		{
			perror("send");
			exit(1);
		}
		p += sent;
		*size -= sent;
	}
	free(*contents);
}

int main(
	int argc,
	char ** argv)
{
	if(argc == 1
	|| (argc == 2 && !strcmp(argv[1], "--help")))
	{
		fprintf(argc == 2 ? stdout : stderr,
			"usage:\n"
			"\t%s f1 f2 ... fN\n"
			"\t\tcompiles f1...fN into executable 'a.out'.\n"
			"\t%s --help\n"
				"\t\tprints this message.\n"
			"\t%s --license\n"
				"\t\tprints the license information.\n"
			"\t%s --find path\n"
				"\t\tresolves the requested include path.\n",
			argv[0],
			argv[0],
			argv[0],
			argv[0]);

		return argc == 2;
	}
	if(argc == 2 && !strcmp(argv[1], "--license"))
	{
		puts(
			"github.com/RmbRT-lang/rmbrtbc - RmbRT language compiler (C bootstrap ver.)\n"
			"Copyright (C) 2020 Steffen \"RmbRT\" Rattay <steffen@sm2.network>\n"
			"\n"
			"This program is free software: you can redistribute it and/or modify\n"
			"it under the terms of the GNU Affero General Public License as published by\n"
			"the Free Software Foundation, either version 3 of the License, or\n"
			"(at your option) any later version.\n"
			"\n"
			"\nThis program is distributed in the hope that it will be useful,\n"
			"\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\n"
			"\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
			"\nGNU Affero General Public License for more details.\n"
			"\n"
			"\nYou should have received a copy of the GNU Affero General Public License\n"
			"along with this program.  If not, see <https://www.gnu.org/licenses/>.");
		return 0;
	}
	if(argc >= 2 && !strcmp(argv[1], "--find"))
	{
		if(argc != 3)
		{
			fprintf(stderr, "usage: %s --find path\n", argv[0]);
			return 1;
		}

		struct RlcScopedFileRegistry r;
		rlc_scoped_file_registry_create(&r);
		char const * path = rlc_scoped_file_registry_resolve_global(
			&r,
			argv[2],
			strlen(argv[2]));
		if(!path)
		{
			fputs("error: could not resolve file.\n", stderr);
			return 1;
		}
		printf("%s\n", path);
		rlc_free((void**)&path);
		rlc_scoped_file_registry_destroy(&r);
		return 0;
	}

	struct RlcScopedFileRegistry scoped_registry;
	rlc_scoped_file_registry_create(&scoped_registry);


	char * typesBuf, * typesImplBuf;
	size_t typesLen, typesImplLen;
	char * funcsBuf, * funcsImplBuf;
	size_t funcsLen, funcsImplLen;
	char * varsBuf, * varsImplBuf;
	size_t varsLen, varsImplLen;

	struct RlcPrinter printer = {
		0,
		open_memstream(&typesBuf, &typesLen),
		open_memstream(&varsBuf, &varsLen),
		open_memstream(&funcsBuf, &funcsLen),
		open_memstream(&typesImplBuf, &typesImplLen),
		open_memstream(&varsImplBuf, &varsImplLen),
		open_memstream(&funcsImplBuf, &funcsImplLen),
		NULL,
		NULL
	};

	int status = 1;
	for(int i = 1; i < argc; i++)
	{
		char const * abs = to_absolute_path(argv[i]);
		struct RlcScopedFile * file;
		if((file = rlc_scoped_file_registry_get(
			&scoped_registry,
			abs)))
		{
			fprintf(
				stdout, "parsed %s\n",
				argv[i]);
			rlc_scoped_file_print(file, &scoped_registry, &printer);
		} else
		{
			fprintf(
				stderr, "%s not found\n",
				argv[i]);
			status = 0;
		}
	}

	rlc_scoped_file_registry_destroy(&scoped_registry);

	int pipefd;
	static char pipename[] = "/tmp/.rlc_pipe_XXXXXX.cpp\0";
	if(-1 == (pipefd = mkstemps(pipename, 4)))
	{
		perror("mkstemp");
		return 1;
	}
	char out_file[PATH_MAX];
	char rlc_dir[PATH_MAX];
	ssize_t rlc_dir_len;
	if((rlc_dir_len = readlink("/proc/self/exe", rlc_dir, sizeof(rlc_dir))) == -1)
	{
		perror("readlink");
		return 1;
	}
	rlc_dir[rlc_dir_len] = '\0';
	snprintf(out_file, sizeof(out_file), "%.*s/out/helper.cpp", parent_dir(rlc_dir), rlc_dir);
	pipe_file(out_file, pipefd);
	read_into_pipe_and_close(printer.fTypes, &typesBuf, &typesLen, pipefd);
	read_into_pipe_and_close(printer.fFuncs, &funcsBuf, &funcsLen, pipefd);
	read_into_pipe_and_close(printer.fTypesImpl, &typesImplBuf, &typesImplLen, pipefd);
	read_into_pipe_and_close(printer.fVars, &varsBuf, &varsLen, pipefd);
	read_into_pipe_and_close(printer.fFuncsImpl, &funcsImplBuf, &funcsImplLen, pipefd);
	read_into_pipe_and_close(printer.fVarsImpl, &varsImplBuf, &varsImplLen, pipefd);
	shutdown(pipefd, SHUT_WR);

	fflush(stdout);

	char command[PATH_MAX+128];
	snprintf(command, sizeof(command), "c++ -std=gnu++2a -x c++ -Werror %s -o a.out -g",
		pipename);
	if((status = !system(command)))
		puts("compiled!");
	close(pipefd);

	size_t allocs;
	if((allocs = rlc_allocations()))
	{
		fprintf(stderr, "Warning: leaked allocations: %zu.\n", allocs);
	}

	fflush(stdout);
	fflush(stderr);
	return status ? 0 : 1;
}