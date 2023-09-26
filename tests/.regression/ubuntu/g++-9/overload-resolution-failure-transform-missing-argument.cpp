===================== Prompt =====================
This is my code:

File `/home/runner/work/cwhy/cwhy/tests/c++/overload-resolution-failure-transform-missing-argument.cpp`:
```
34 iterator that appends new elements to the `v_out` vector.
35 */
36 #include <algorithm>
37 #include <vector>
38 
39 int main() {
40     std::vector<int> v;
41     std::transform(v.begin(), v.end(), [](int i) { return i * i; });
42 }
```

File `/usr/include/c++/9/bits/stl_algo.h`:
```
4325    *  range @p [0,__last-__first).
4326    *
4327    *  @p unary_op must not alter its argument.
4328   */
4329   template<typename _InputIterator, typename _OutputIterator,
4330 	   typename _UnaryOperation>
4331     _OutputIterator
4332     transform(_InputIterator __first, _InputIterator __last,
4333 	      _OutputIterator __result, _UnaryOperation __unary_op)
4334     {
4335       // concept requirements
```

File `/usr/include/c++/9/bits/stl_algo.h`:
```
4363    *
4364    *  @p binary_op must not alter either of its arguments.
4365   */
4366   template<typename _InputIterator1, typename _InputIterator2,
4367 	   typename _OutputIterator, typename _BinaryOperation>
4368     _OutputIterator
4369     transform(_InputIterator1 __first1, _InputIterator1 __last1,
4370 	      _InputIterator2 __first2, _OutputIterator __result,
4371 	      _BinaryOperation __binary_op)
4372     {
```


This is my error:
```
/home/runner/work/cwhy/cwhy/tests/c++/overload-resolution-failure-transform-missing-argument.cpp: In function ‘int main()’:
/home/runner/work/cwhy/cwhy/tests/c++/overload-resolution-failure-transform-missing-argument.cpp:41:67: error: no matching function for call to ‘transform(std::vector<int>::iterator, std::vector<int>::iterator, main()::<lambda(int)>)’
   41 |     std::transform(v.begin(), v.end(), [](int i) { return i * i; });
      |                                                                   ^
In file included from /usr/include/c++/9/algorithm:62,
                 from /home/runner/work/cwhy/cwhy/tests/c++/overload-resolution-failure-transform-missing-argument.cpp:36:
/usr/include/c++/9/bits/stl_algo.h:4332:5: note: candidate: ‘template<class _IIter, class _OIter, class _UnaryOperation> _OIter std::transform(_IIter, _IIter, _OIter, _UnaryOperation)’
 4332 |     transform(_InputIterator __first, _InputIterator __last,
      |     ^~~~~~~~~
/usr/include/c++/9/bits/stl_algo.h:4332:5: note:   template argument deduction/substitution failed:
/home/runner/work/cwhy/cwhy/tests/c++/overload-resolution-failure-transform-missing-argument.cpp:41:67: note:   candidate expects 4 arguments, 3 provided

[...]

   41 |     std::transform(v.begin(), v.end(), [](int i) { return i * i; });
      |                                                                   ^
In file included from /usr/include/c++/9/algorithm:62,
                 from /home/runner/work/cwhy/cwhy/tests/c++/overload-resolution-failure-transform-missing-argument.cpp:36:
/usr/include/c++/9/bits/stl_algo.h:4369:5: note: candidate: ‘template<class _IIter1, class _IIter2, class _OIter, class _BinaryOperation> _OIter std::transform(_IIter1, _IIter1, _IIter2, _OIter, _BinaryOperation)’
 4369 |     transform(_InputIterator1 __first1, _InputIterator1 __last1,
      |     ^~~~~~~~~
/usr/include/c++/9/bits/stl_algo.h:4369:5: note:   template argument deduction/substitution failed:
/home/runner/work/cwhy/cwhy/tests/c++/overload-resolution-failure-transform-missing-argument.cpp:41:67: note:   candidate expects 5 arguments, 3 provided
   41 |     std::transform(v.begin(), v.end(), [](int i) { return i * i; });
      |                                                                   ^
```


What's the problem?
==================================================
