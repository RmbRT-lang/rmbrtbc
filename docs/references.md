# References

In RL, there's two kinds of references: local references (`&`), and expiring references (`&&`).
Although they are similar to C++'s references, they are different.
References are similar to pointers, in that they do not hold an individual instance of a type.
Instead, they point to an already existing instance.
However, unlike pointers, they are accessed like instances (with `.` instead of `->`).

**Address restriction**&emsp;
It is forbidden to retrieve a reference variable's address.
This restriction is important for both optimisation and provability purposes.
For all other intents and purposes, a reference behaves exactly like a copy.

**Lifetime restriction**&emsp;
A reference may never live longer than the value it is referencing.
Unlike in C++, it is impossible to extend a value's lifetime using references.

## Local references

Only function arguments and local variables may be local references.
When passing a local instance to a function, it is implicitly passed as a read-only local reference unless otherwise specified (using the `&` reference operator).
A local reference's fields will also appear as local references.

**Mutable local references**&emsp;
Mutable local references are to be used as out parameters.
To pass a variable as mutable local reference, the variable itself has to be mutable.
The variable is converted into a reference using the `&` prefix operator.
Because it is impossible to take their address, they are safer than simple pointers.
Passing a variable as a mutable local reference may only result in immediate modifications, there must not be any delayed side effects that affect the passed variable.
Only local instance variables and mutable local references may be passed as mutable local references.
Mutable local references have to be explicitly converted into mutable local references again when passing them to a function, because otherwise, they default to read-only local references.

**Read-only local references**&emsp;
If a type can be trivially copied, and is not larger than a pointer, a read-only local reference can also be implemented using a true copy.
Read-only local references are used to pass read-only values to functions, and should be used instead of copies when possible, as they can optimise performance.
Passing a variable as a read-only local reference must not result in any immediate or delayed side effects that affect the variable (as its address is never published).
Only local instance variables and read-only references may be passed as read-only references.

## Expiring references
Only function arguments may be expiring references.
When passing a temporary value (such as the return value of a function) to a function, the value is implicitly passed as an expiring reference.
Expiring references are always mutable, even if the underlying type is not.
They can be used to implement move semantics.
An expiring reference's fields will also appear as expiring references.