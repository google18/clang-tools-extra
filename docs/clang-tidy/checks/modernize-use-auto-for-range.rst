.. title:: clang-tidy - modernize-use-auto-for-range

modernize-use-auto-for-range
============================

Check suggests using ``auto`` type specifier on variable declartion within
range based loops. Type specifier ``auto`` will automatically deduce the type
as being ``std::pair<const KeyValue, ValueType>`` as opposed to the often assumed
``std::pair<KeyType, ValueType>``.

.. code-block:: c++
 
  std::map<char,int> Obj;
  for(std::pair<char, int> i : Obj){
  }

  //transforms to

  for(auto i : Obj){
  }
