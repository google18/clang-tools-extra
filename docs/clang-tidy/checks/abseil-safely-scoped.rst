.. title:: clang-tidy - abseil-safely-scoped

abseil-safely-scoped
====================

Flag situations where a using declaration is not inside a namespace, or
not inside the innermost namespace when there is a nested series of 
namespaces.

Example:

.. code-block:: c++

using something; // should be inside the innermost namespace bar below

namespace foo {

namespace bar {
	
} // bar

using somethingelse; // shoulw be inside the innermost namespace bar above

} // foo
