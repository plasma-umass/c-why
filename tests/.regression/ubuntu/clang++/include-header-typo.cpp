===================== Prompt =====================
This is my code:

File `/home/runner/work/cwhy/cwhy/tests/c++/include-header-typo.cpp`:
```
1 #include "blank-header.hp"
2 
3 int main() {
4   return 0;
```


This is my error:
```
/home/runner/work/cwhy/cwhy/tests/c++/include-header-typo.cpp:1:10: fatal error: 'blank-header.hp' file not found
    1 | #include "blank-header.hp"

[...]

      |          ^~~~~~~~~~~~~~~~~
1 error generated.
```


What's the problem? If you can, suggest code to fix the issue.
==================================================
