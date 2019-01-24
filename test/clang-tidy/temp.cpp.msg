14 warnings generated.
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:55:10: warning: use absl::make_unique instead [abseil-make-unique]
  return std::unique_ptr<int>(new int(0));
         ^~~~~~~~~~~~~~~      ~~~~~~~~ ~
         absl::make_unique
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:1:1: note: FIX-IT applied suggested code changes
// RUN: %check_clang_tidy %s abseil-make-unique %t -- -- -std=c++11 \
^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:55:10: note: FIX-IT applied suggested code changes
  return std::unique_ptr<int>(new int(0));
         ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:55:31: note: FIX-IT applied suggested code changes
  return std::unique_ptr<int>(new int(0));
                              ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:55:40: note: FIX-IT applied suggested code changes
  return std::unique_ptr<int>(new int(0));
                                       ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:61:29: warning: use absl::make_unique instead [abseil-make-unique]
  std::unique_ptr<int> P1 = std::unique_ptr<int>(new int(1));
                            ^~~~~~~~~~~~~~~      ~~~~~~~~ ~
                            absl::make_unique
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:61:29: note: FIX-IT applied suggested code changes
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:61:50: note: FIX-IT applied suggested code changes
  std::unique_ptr<int> P1 = std::unique_ptr<int>(new int(1));
                                                 ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:61:59: note: FIX-IT applied suggested code changes
  std::unique_ptr<int> P1 = std::unique_ptr<int>(new int(1));
                                                          ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:65:6: warning: use absl::make_unique instead [abseil-make-unique]
  P1.reset(new int(2));
    ~^~~~~ ~~~~~~~~ ~
     = absl::make_unique<int>
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:65:5: note: FIX-IT applied suggested code changes
  P1.reset(new int(2));
    ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:65:12: note: FIX-IT applied suggested code changes
  P1.reset(new int(2));
           ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:65:21: note: FIX-IT applied suggested code changes
  P1.reset(new int(2));
                    ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:70:27: warning: use absl::make_unique instead [abseil-make-unique]
  std::unique_ptr<A> P2 = std::unique_ptr<A>(new A(1, 2));
                          ^~~~~~~~~~~~~~~    ~~~~~~    ~
                          absl::make_unique
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:70:27: note: FIX-IT applied suggested code changes
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:70:46: note: FIX-IT applied suggested code changes
  std::unique_ptr<A> P2 = std::unique_ptr<A>(new A(1, 2));
                                             ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:70:56: note: FIX-IT applied suggested code changes
  std::unique_ptr<A> P2 = std::unique_ptr<A>(new A(1, 2));
                                                       ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:74:6: warning: use absl::make_unique instead [abseil-make-unique]
  P2.reset(new A(3, 4));
    ~^~~~~ ~~~~~~    ~
     = absl::make_unique<A>
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:74:5: note: FIX-IT applied suggested code changes
  P2.reset(new A(3, 4));
    ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:74:12: note: FIX-IT applied suggested code changes
  P2.reset(new A(3, 4));
           ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:74:22: note: FIX-IT applied suggested code changes
  P2.reset(new A(3, 4));
                     ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:79:29: warning: use absl::make_unique instead [abseil-make-unique]
  std::unique_ptr<int> P3 = std::unique_ptr<int>(new int);
                            ^~~~~~~~~~~~~~~      ~~~~~~~
                            absl::make_unique
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:79:29: note: FIX-IT applied suggested code changes
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:79:50: note: FIX-IT applied suggested code changes
  std::unique_ptr<int> P3 = std::unique_ptr<int>(new int);
                                                 ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:83:6: warning: use absl::make_unique instead [abseil-make-unique]
  P3.reset(new int);
    ~^~~~~ ~~~~~~~
     = absl::make_unique<int>
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:83:5: note: FIX-IT applied suggested code changes
  P3.reset(new int);
    ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:83:12: note: FIX-IT applied suggested code changes
  P3.reset(new int);
           ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:88:29: warning: use absl::make_unique instead [abseil-make-unique]
  std::unique_ptr<int> P4 = std::unique_ptr<int>((new int(3)));
                            ^~~~~~~~~~~~~~~      ~~~~~~~~~ ~~
                            absl::make_unique
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:88:29: note: FIX-IT applied suggested code changes
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:88:50: note: FIX-IT applied suggested code changes
  std::unique_ptr<int> P4 = std::unique_ptr<int>((new int(3)));
                                                 ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:88:60: note: FIX-IT applied suggested code changes
  std::unique_ptr<int> P4 = std::unique_ptr<int>((new int(3)));
                                                           ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:92:8: warning: use absl::make_unique instead [abseil-make-unique]
  P4 = std::unique_ptr<int>(((new int(4))));
       ^~~~~~~~~~~~~~~      ~~~~~~~~~~ ~~~
       absl::make_unique
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:92:8: note: FIX-IT applied suggested code changes
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:92:29: note: FIX-IT applied suggested code changes
  P4 = std::unique_ptr<int>(((new int(4))));
                            ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:92:40: note: FIX-IT applied suggested code changes
  P4 = std::unique_ptr<int>(((new int(4))));
                                       ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:96:6: warning: use absl::make_unique instead [abseil-make-unique]
  P4.reset((new int(5)));
    ~^~~~~ ~~~~~~~~~ ~~
     = absl::make_unique<int>
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:96:5: note: FIX-IT applied suggested code changes
  P4.reset((new int(5)));
    ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:96:12: note: FIX-IT applied suggested code changes
  P4.reset((new int(5)));
           ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:96:22: note: FIX-IT applied suggested code changes
  P4.reset((new int(5)));
                     ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:101:13: warning: use absl::make_unique instead [abseil-make-unique]
  auto P5 = std::unique_ptr<int>(new int());
            ^~~~~~~~~~~~~~~      ~~~~~~~~~
            absl::make_unique
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:101:13: note: FIX-IT applied suggested code changes
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:101:34: note: FIX-IT applied suggested code changes
  auto P5 = std::unique_ptr<int>(new int());
                                 ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:108:25: warning: use absl::make_unique instead [abseil-make-unique]
    unique_ptr<int> Q = unique_ptr<int>(new int());
                        ^~~~~~~~~~      ~~~~~~~~~
                        absl::make_unique
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:108:25: note: FIX-IT applied suggested code changes
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:108:41: note: FIX-IT applied suggested code changes
    unique_ptr<int> Q = unique_ptr<int>(new int());
                                        ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:112:9: warning: use absl::make_unique instead [abseil-make-unique]
    Q = unique_ptr<int>(new int());
        ^~~~~~~~~~      ~~~~~~~~~
        absl::make_unique
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:112:9: note: FIX-IT applied suggested code changes
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:112:25: note: FIX-IT applied suggested code changes
    Q = unique_ptr<int>(new int());
                        ^
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:118:17: warning: use absl::make_unique instead [abseil-make-unique]
  expectPointer(std::unique_ptr<int>(new int()));
                ^~~~~~~~~~~~~~~      ~~~~~~~~~
                absl::make_unique
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:118:17: note: FIX-IT applied suggested code changes
/Users/clinic18/Desktop/clinic/llvm/tools/clang/tools/extra/test/clang-tidy/temp.cpp:118:38: note: FIX-IT applied suggested code changes
  expectPointer(std::unique_ptr<int>(new int()));
                                     ^
clang-tidy applied 37 of 37 suggested fixes.
