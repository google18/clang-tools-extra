// RUN: %check_clang_tidy %s modernize-use-auto-for-range %t

#include<map>
#include<unordered_map>
// FIXME: Add something that triggers the check here.
void positives(){

  std::map<char, int> a;
  for(std::pair<char, int> i : a);

  for(std::pair<const char, int> i : a);

  std::unordered_map<char,int> b;
  for(std::pair<char,int> i : b);

  for (std::pair>const char, int> i : b);
}

void negatives(){
  
  std::map<double, char> b;
  for(auto i : b);
}



// CHECK-MESSAGES: :[[@LINE-1]]:6: warning: function 'f' is insufficiently awesome [modernize-use-auto-for-range]


// FIXME: Verify the applied fix.
//   * Make the CHECK patterns specific enough and try to make verified lines
//     unique to avoid incorrect matches.
//   * Use {{}} for regular expressions.
// CHECK-FIXES: {{^}}void awesome_f();{{$}}

// FIXME: Add something that doesn't trigger the check here.
void awesome_f2();
