.. title:: clang-tidy - abseil-qualified-aliases

abseil-qualified-aliases
========================

Detects using declarations that are not fully qualified, and suggests
that the developer fully qualify those declarations.

Example:
.. code-block:: c++

  namespace foo {
    void f();
    void correct();
  }

  namespace bar {
    using foo::f; // The check produces a warning here. 
    using ::foo::correct; // The check sees no issue here.
  }

See https://abseil.io/tips/119 for a more in depth justification of this
check.
