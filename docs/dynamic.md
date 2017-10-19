# Dynamic

Dynamic values are delayed in computation until they are used. This means, that only upon their first read access, they will be evaluated. This is important for operators such as the logical or (`||`), because if the first operand is true, the second operand should not be evaluated at all. This can be done by marking the second operand as dynamic, via the `dynamic` keyword:

	inline operator||(lhs: Bool, rhs : bool@): Bool := {
		if(lhs)
			return true; // does not read rhs, so it is not evaluated.
		else
			return rhs; // evaluates rhs.
	};

Here, `rhs` is only calculated if `lhs' is false, since only the second return statement accesses its value.

	inline operator?{T: type}(cond: Bool, tval: T dynamic, fval: T dynamic): T := {
		if(cond)
			return tval; // only evaluates tval.
		else
			return fval; // only evaluates fval.
	};

Again, either `tval` or `fval` is evaluated, but never both. 

Dynamic arguments are to be realised in such a way, that they execute the expression that was used to create them upon the first reading access. For subsequent accesses, the evaluated value is remembered and must not be recomputed. The evaluation expression may either be inlined, if possible, or be put into a helper function that calculates it. The evaluation routine needs access to the original stack frame that was used to instantiate the dynamic expression, so that it can access the local variables that are referenced. The implementation of the dynamic type is up to the compiler vendor.