.. title:: clang-tidy - abseil-anonymous-enclosed-aliases

abseil-anonymous-enclosed-aliases
=================================

Finds using declarations outside of anonymous namespaces, and
suggests those declarations be moved to that namespace.

Example:
.. code-block:: c++

  namespace foo {
  
  using something; // should be inside the anonymous namespace below

  namespace {

  } // anonymous namespace

  } // foo
