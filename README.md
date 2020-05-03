# RmbRT Language Compiler Readme

The RmbRT Language Compiler a work in progress implementation of a compiler for a programming language invented by me. The language is not yet fully specified. The documentation can be found [here](docs/index.md).

## How to install

### Cloning

	git clone git@github.com:RmbRT/rlc.git

### Compiling

Navigate into the cloned directory, then execute:

	cmake .

to generate the necessary project / make files your machine needs to build the library. This also copies all header files into the directory ```rlc/include/rlc/```.

### Generating documentation

You need to have doxygen installed on your machine. Navigate into the ```rlc/doxygen/``` directory, and issue:

	doxygen Doxyfile

This generates a HTML documentation based on the comments in the code, and places it in ```rlc/doxygen/html/```.

### Using

**Include paths**&emsp;
To set up include paths, set the environment variable `RLINCLUDE`, which is a `:`-separated list of directory names. Files included using apostrophe strings (i.e. `include 'std/something'`) will be searched in `RLINCLUDE` paths. Double quotes will be interpreted as relative paths.

**Invoking**&emsp;
To compile files, call the following:
```
path/to/rlc source1 source2 sourceN
```
This will compile all sources and output everything in a single executable. Note that it currently works best if you use a single source file that includes all other files needed. The generated executable will be generated in `a.out` in the current working directory.
To get all invocation options, run:
```
rlc --help
```

## License

The RmbRT Language Compiler is free (as in freedom, or libre) software, and released under the GNU Affero General Public License, version 3, which can be found in the file ```rlc/LICENSE```.

## The Project

	... more to come ...