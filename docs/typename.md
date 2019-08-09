# Typename

A typename is built using the following syntax:

	Typename ::= Basetype Qualifier (Indirection Qualifier)*;
	Basetype ::= 'void' | Symbol | Signature;
	Symbol ::= Scope-qualified-identifier Templates?;
	Signature ::= '@'? '(' '(' (void | (Typename (',' Typename)*)) ')' ':' Typename ')';
	Qualifier ::= ('#'? '$'?| '$'? '#'?);
	Indirection ::= ('*'|'\'|'@'|'...!'|'...');

Where *Scope-qualified-identifier* is a possibly scope-qualified identifier (such as `int` or `std::vector` etc.) and *Templates* is a template instantiation (such as `{Float}` or `{3, Int}`).

'`*`' modifies a type to be a pointer to the preceeding part of the type name.
'`\`' modifies a type to be a reference to the preceeding part of the type name, which means it always has to have a valid value, but in contrast to languages like C/C++, it is not constant.
'`#`' qualifies a type to be a constant, '`$`' qualifies a type to be volatile.
'`@`' qualifies a type to be a future value.
`...`, and `...!` qualify a type to be dynamic, and expected dynamic, respectively.
Asynchronous function signatures are preceeded with an `@`.

## Example

The equivalent of an Object reference (`Type &`) from C++ is `Type \#`, since it is a non-null and constant pointer. Note though that a reference in RL is treated as an address, not as an object.

A function that takes two integers and returns an integer would have the signature `((int,int):int)`. Note that a signature type is not a pointer or reference type, the respective qualifiers have to be added to be instantiatable.