# Function

In RL, functions are defined via the following syntax:

	Function-Declaration ::= Identifier Template-Declaration? "(" Argument ("," Argument)* ")" ":" Type-Expression ":=" (Block-Statement | Expression) ";";

If the function body is an expression instead of a block element, it is equal to a block statement containing solely a return statement, returning the given expression. A function is then invoked via the following syntax:

	Expression "(" Expression ("," Expression)* ")"

where the first expression is the function name or evaluates into one, and the following expressions are the function's arguments.

## Example

	max{T:type} (a: T, b: T) : T := (b<a) ? a : b;

	strlen(string: char \) : size_t :=
	{
		length : size_t(0);

		while(string[length] != '\0')
			++length;

		return length;
	};