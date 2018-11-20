.. title:: clang-tidy - abseil-make-unique

abseil-make-unique
==================

Replaces unique pointers that are constructed with raw pointers with ``absl::make_unique``, for leak-free dynamic allocation.

.. code-block:: c++
  std::unique_ptr<int> upi(new int);

will be replaced with 

.. code-block:: c++
  auto upi = absl::make_unique<int>();

See https://abseil.io/tips/126 for full explanation and justification.
