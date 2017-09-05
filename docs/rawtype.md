# Rawtype

A "rawtype" is an atomic type, which consists of only raw data and has no accessible member variables. A rawtype can only contain member functions. A rawtype is described using the following syntax:

	"rawtype" name templates? "[" size "]" (":=" "{" members "}")? ";";

Where *name* is an identifier and *templates* a template declaration, *size* an expression evaluating to a numeric value `> 0`, denoting the bit size, and *members* a list of member functions. The default visibility value for rawtype members is `public`.

## Example

	rawtype Float [32] := {
		toHalf() : Half := ... ;
	};
	rawtype Half [16] := {
		toFloat() : Float := ... ; // instead of "...", imagine some assembly code.
	};