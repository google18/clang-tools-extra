.. title:: clang-tidy - abseil-make-unique

abseil-make-unique
==================

Checks for instances of initializing a `unique_ptr` with a direct call to
`new` and suggests using `absl::make_unique` instead.

Replaces initialization of smart pointers:
\code
  std::unique_ptr<int> ptr = std::unique_ptr<int>(new int(1));
\endcode

with the preferred usage:
\code
  std::unique_ptr<int> ptr = absl::make_unique<int>(1);
\endcode

per the Abseil tips and guidelines at https://abseil.io/tips/126.
