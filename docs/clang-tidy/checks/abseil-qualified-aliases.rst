.. title:: clang-tidy - abseil-qualified-aliases

abseil-qualified-aliases
========================

Detects using declarations that are not fully qualified, and suggests 
those declarations be fully qualified. 

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

See https://abseil.io/tips/119 for an in depth justification for this
check. 
 
