.. title:: clang-tidy - abseil-anonymous-enclosed-aliases

abseil-anonymous-enclosed-aliases
=================================

Flag uses of using declarations outside an anonymous namespace when there exists one.

Example:
.. code-block:: c++

  namespace foo {
  
  using something; // should be inside the anonymous namespace below

  namespace {

  } // anonymous namespace

  } // foo