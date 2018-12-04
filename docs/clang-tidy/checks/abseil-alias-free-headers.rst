.. title:: clang-tidy - abseil-alias-free-headers

abseil-alias-free-headers
=========================

Flags using declarations in header files, and suggests that these aliases be removed.

A using declaration placed in a header file forces users of that header file
accept the specified alias. This is bad practice, which is why the check suggests
such declarations be removed. 
