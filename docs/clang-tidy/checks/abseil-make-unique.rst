.. title:: clang-tidy - abseil-make-unique
.. meta::
   :http-equiv=refresh: 5;URL=abseil-make-unique.html

abseil-make-unique
==================

This check finds the creation of ``std::unique_ptr`` objects by explicitly
calling the constructor and a ``new`` expression, and replaces it with a call
to ``absl::make_unique``, the Abseil implementation of ``std::make_unique`` 
in C++11.

.. code-block:: c++

  auto ptr = std::unique_ptr<int>(new int(1));

  // becomes

  auto ptr = absl::make_unique<int>(1);

The Abseil Style Guide <https://abseil.io/tips/126>_ discusses this issue in
more detail.
