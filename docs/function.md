# Function

In RL, functions are defined via the following syntax:

	name(arg1: type1, arg2: type2, type3, type4, arg5: type5): type := {
		statements;
	};

	name(): type := expression;

	name() ::= expression;

	name() { statements; }

	name() inline { }

Arguments are, like variables, of the form `name: type`.
Anonymous arguments are just type names.
To specify a return type, use the syntax of the first two examples.
The third type uses the auto-type assignment, which makes the function return the same type as the expression.
The last two examples have no return value, and do not need a trailing semicolon.
By writing `inline` before the body, a function can be inlined.
By writing `@` before the body (before potential `inline`), the function becomes asynchronous, and can be used as a coroutine:

	coroutine(int x) @ { /*...*/ }

This enables the use of the `yield` statement.

## Example

	max{T:type} (a: T, b: T) : T := (b<a) ? a : b;

	strlen(string: char \) : size_t :=
	{
		length : size_t(0);

		while(string[length] != '\0')
			++length;

		return length;
	};