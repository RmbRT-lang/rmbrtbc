# Dynamic

Dynamic values are delayed in computation until they are used.
This means, that only upon their first read access, they will be evaluated.
This is important for operators such as the logical or (`||`), because if the first operand is true, the second operand should not be evaluated at all.
This can be done by marking the second operand as dynamic, via the `..?` type indirection:

	operator||(lhs: Bool, rhs : Bool..?) inline Bool
	{
		if(lhs)
			return true; // does not read rhs, so it is not evaluated.
		else
			return rhs; // evaluates rhs.
	}

Here, `rhs` is only calculated if `lhs' is false, since only the second return statement accesses its value.

	operator?(cond: Bool, tval: T..?, fval: T..?) inline T
	{
		if(cond)
			return tval; // only evaluates tval.
		else
			return fval; // only evaluates fval.
	}

Again, either `tval` or `fval` is evaluated, but never both. 

Any expression that is assigned to a dynamic variable becomes dynamic implicitly.
One can explicitly make an expression dynamic by prepending it with the `..?` operator:

	a || ..?b();

Dynamic arguments are to be realised in such a way, that they execute the expression that was used to create them upon the first reading access. For subsequent accesses, the evaluated value is remembered and must not be recomputed. The evaluation expression may either be inlined, if possible, or be put into a helper function that calculates it. The evaluation routine needs access to the original stack frame that was used to instantiate the dynamic expression, so that it can access the local variables that are referenced. The implementation of the dynamic type is up to the compiler vendor.

## Expected Dynamic

When a dynamic expression is needed, but it is guaranteed to be evaluated at some point, the `..!` type indirection can be used.
If such a value is never evaluated, it results in either a run-time or compile-time error, depending on whether it could be detected statically.
This type of dynamic expression may be faster, so it is recommended to use them when possible, as they allow for more compiler optimisations.
Just like with regular dynamic expressions, there exists the `..!` operator to make an expression into an expected dynamic expression.