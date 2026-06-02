## 介绍
这个是一个 cpp 的大整数库  

## 使用方法
和 `boost::multiprecision::cpp_int` 差不多
```
cpp_int a("123");
cpp_int b;
std::cin >> b;
std::cout << b;
cpp_int c = a + b;
c = a * b;
c = a / b;
c = a % b;
a = cpp_int(123);
```