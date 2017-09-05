# Typename

A typename is built using the following syntax:

	Typename ::= Scope-qualified-identifier Templates? Qualifier (Indirection Qualifier)*;
	Qualifier ::= ('#'? '$'?| '$'? '#'?);
	Indirection ::= ('*'|'\');

Where *Scope-qualified-identifier* is a possibly scope-qualified identifier (such as `int` or `std::vector` etc.) and *Templates* is a template instantiation (such as `{Float}`).

'`*`' modifies a type to be a pointer to the preceeding part of the type name. '`\`' modifies a type to be a reference to the preceeding part of the type name, which means it always has to have a valid value, but in contrast to languages like C/C++, it is not constant. '`#`' qualifies a type to be a constant, '`$`' qualifies a type to be volatile.

## Example

The equivalent of an Object reference (`Type &`) from C++ is `Type \#`, since it is a non-null and constant pointer. Note though that a reference in RL is treated as an address, not as an object.