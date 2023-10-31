// Copyright (c) 2020 by Chrono

#ifndef _CPP_LANG_HPP
#define _CPP_LANG_HPP

#include <cassert>

#include <iostream>

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <regex>

#include <atomic>
#include <future>
#include <thread>

// never 'using namespace std;' in c++ header

// must be C++11 or later
#if __cplusplus < 201103
#error "C++ is too old"
#endif // __cplusplus < 201103

/*
这种设计主要是为了提高代码的可移植性和兼容性。
C++ 14 中引入的 `[[deprecated]]` 特性可以更加直观地标记被弃用的代码，并且可以在编译时给出警告或错误提示；而使用 GNU 扩展则可以兼容旧版本的编译器。
*/
// [[deprecated]]
#if __cplusplus >= 201402
#define CPP_DEPRECATED [[deprecated]]
#else
#define CPP_DEPRECATED [[gnu::deprecated]]
#endif // __cplusplus >= 201402

/*
C++ 14 中引入的 `static_assert` 可以更加直观地进行静态断言，并且可以给出自定义的错误提示信息；
而使用带有错误信息的 `static_assert` 则可以兼容旧版本的编译器，确保在编译时能够及时发现代码中的问题。
*/
// static_assert
#if __cpp_static_assert >= 201411
#define STATIC_ASSERT(x) static_assert(x)
#else
#define STATIC_ASSERT(x) static_assert(x, #x)
#endif

/*
定义、结束和使用命名空间的过程可以通过宏来简化、统一和规范化，使代码更加清晰易懂。
*/
// macro for convienient namespace
#define BEGIN_NAMESPACE(x) \
    namespace x            \
    {
#define END_NAMESPACE(x) }
#define USING_NAMESPACE(x) using namespace x

// static_assert(
//     __GNUC__ >= 4, "GCC is too old");

#endif //_CPP_LANG_HPP

/*
总结：利用预处理编程，使用宏定义、条件编译来屏蔽操作系统、语言版本的差异，增强程序的兼容性。
*/