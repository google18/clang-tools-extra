.. title:: clang-tidy - abseil-safely-scoped

abseil-safely-scoped
====================

Flags using declarations that are not contained in an innermost
namespace, and suggests these declarations be moved elsewhere.

Example:

.. code-block:: c++

  using something; // should be inside the innermost namespace bar below

  namespace foo {
  namespace bar {
	
  } // bar

  using something_else; // shoulw be inside the innermost namespace bar above

  } // foo

Placing convenience aliases in upper level namespaces can lead to ambiguity in 
which name the compiler should use. 

See https://abseil.io/tips/119 for more explanation. 

