.. title:: clang-tidy - abseil-alias-free-headers

abseil-alias-free-headers
=========================

Flags using declarations in header files, and suggests that these aliases be removed.

A using declaration placed in a header file forces users of that header file
accept the specified alias. Because of this, using declarations should almost never
be used in a header file

The style guide  http://google.github.io/styleguide/cppguide.html#Aliases discusses this
issue in more detail
