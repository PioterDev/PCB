//Copyright (c) 2025 Piotr Mikolajewski
//Permission is hereby granted, free of charge, to any person obtaining
//a copy of this software and associated documentation files (the "Software"),
//to deal in the Software without restriction, including without limitation
//the rights to use, copy, modify, merge, publish, distribute, sublicense,
//and/or sell copies of the Software, and to permit persons to whom
//the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
//DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
//OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef PCB_H
#define PCB_H

#ifndef PCB_VERSION_MAJOR
#define PCB_VERSION_MAJOR 0
#endif //PCB_VERSION_MAJOR

#ifndef PCB_VERSION_MINOR
#define PCB_VERSION_MINOR 0
#endif //PCB_VERSION_MINOR

#ifndef PCB_VERSION_PATCH
#define PCB_VERSION_PATCH 1
#endif //PCB_VERSION_MAJOR

#ifndef PCB_VERSION
#define PCB_VERSION (PCB_VERSION_MAJOR * 100 + PCB_VERSION_MINOR * 10 + PCB_VERSION_PATCH)
#endif //PCB_VERSION

//Section 0: The preamble.

//TODO:
//1. Allow compilation of C files with a C compiler
//even in a C++ project.
//2. PCB_BuildContext structure for PCB_build() instead of globals
//+ new function PCB_buildFromContext(PCB_BuildContext*)

//PCB uses the following naming convention:
//
//- Names starting with "data type" should have "_" as next character.
//- Names with the same prefix should have "_" as next character.
//- If a name block does not contain any "long" word
//(~8+ characters long), camelCase is used; snake_case otherwise
//- If names refer to something similar, their prefix should
//also be similar and the differing part should be connected
//with "_". Additionally, if there is an established
//default, the differing part can be omitted for the default one.
//- If the resulting name using camelCase would hurt
//readability, convert parts/all of it to snake_case.
//- Conversely, if the resulting name using snake_case would hurt
//readability, convert parts/all of it to camelCase.
//- If a name contains names of at least 2 data types,
//favor snake_case.
//- Compile-time constants should favor CAPITALIZED SNAKE_CASE.
//- Non-trivial data types' first letter should be CAPITALIZED.
//- Name parts that can be inferred by analyzing target's use
//case should be removed.
//- Prefer names with full words over partial words.
//- Prefer short and descriptive names for non-static variables
//and more descriptive names for static variables.
//- A combination of "type name", contents of "type name"
//(if struct/union) and "variable name" should be enough
//to infer the reason of the variable's existence.
//- Avoid single-letter variable names, unless used
//in a mathematical context.
//- If a name can be made shorter without sacrificing
//readability, it should...with restraint.
//- Lastly, this is a guideline, not a strict set of rules.
//Exceptions will happen.
//
//
//
//For example:
//
//typedef struct {
// char* data;
// size_t length;
// size_t capacity;
//} "dynamic string";
//"dynamic string" is non-trivial -> "Dynamic_string"?
//No, the part "Dynamic" can be inferred by its intended
//use case and functions manipulating it -> "_string"?
//No, the underscore hurts readability -> "string"?
//No, it's non-trivial -> "String"?
//Maybe, what about "Str"?
//No, "Str" is not a full english word -> back to "String".
//Settled.
//
//Let's say that we implement 2 following functions:
//
//"String starts with String" with arguments (String, String)
//and
//"String starts with C string" with arguments (String, C String)
//
//Both manipulate "String" -> both start with "String_".
//Both share "starts with" as a prefix:
// "starts" and "with" and not long enough, combine with camelCase.
// Now both start with "String_startsWith".
//Both do something similar, but on different arguments,
//therefore current name + the rest of the name should be
//connected with "_".
//
//We get:
//"String_startsWith_String"
//and
//"String_startsWith_CString".
//The first one will be the default -> "String_startsWith".
//The second one could be shortened to "String_startsWith_cstr"
//
//
//
//All in all, I think this is a reasonable naming convention.
//Certainly better than strictly following camelCase/snake_case/etc.
//Certainly better than a lack of any convention.
//And most certainly better than whatever the fuck
//libstdc++ uses internally...trust me, if styling code
//was a marathon, libstdc++ would sprint in circles
//until it lit the asphalt on fire and opened
//a nether portal.



//Section 1: The preprocessor shenanigans
//Section 1.1: Identify the target operating system

//https://sourceforge.net/p/predef/wiki/OperatingSystems/
#ifndef PCB_PLATFORM
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__NT__)
#define PCB_PLATFORM_WINDOWS 1
#define PCB_PLATFORM_LINUX 0
#define PCB_PLATFORM_BSD 0
#define PCB_PLATFORM_MACOS 0
#define PCB_PLATFORM_IOS 0
#define PCB_PLATFORM "Windows"
#elif defined(__linux__)
#define PCB_PLATFORM_WINDOWS 0
#define PCB_PLATFORM_LINUX 1
#define PCB_PLATFORM_BSD 0
#define PCB_PLATFORM_MACOS 0
#define PCB_PLATFORM_IOS 0
#define PCB_PLATFORM "Linux"
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IOS
#define PCB_PLATFORM_WINDOWS 0
#define PCB_PLATFORM_LINUX 0
#define PCB_PLATFORM_BSD 0
#define PCB_PLATFORM_MACOS 0
#define PCB_PLATFORM_IOS 1
#define PCB_PLATFORM "iOS"
#elif TARGET_OS_MAC
#define PCB_PLATFORM_WINDOWS 0
#define PCB_PLATFORM_LINUX 0
#define PCB_PLATFORM_BSD 0
#define PCB_PLATFORM_MACOS 1
#define PCB_PLATFORM_IOS 0
#define PCB_PLATFORM "Mac OS"
#else
#error PCB Error: Unsupported Apple platform
#endif //Apple platforms
#else
#error PCB Error: Unsupported platform
#endif //platform
#endif //PCB_PLATFORM

//This macro is used for certain #include's of system headers
//and some function implementations.
//POSIX-compliant platforms can safely share implementations.
//Other platforms require dedicated implementations (*ekhem* Windows...).
#ifndef PCB_PLATFORM_POSIX
#if PCB_PLATFORM_WINDOWS
#define PCB_PLATFORM_POSIX 0
#elif PCB_PLATFORM_LINUX || PCB_PLATFORM_BSD || PCB_PLATFORM_MACOS || PCB_PLATFORM_IOS
#define PCB_PLATFORM_POSIX 1
#else
#define PCB_PLATFORM_POSIX 0
#endif //PCB_PLATFORM_POSIX


//Section 1.2: Identify the compiler used to compile this code

#ifndef PCB_COMPILER
#if defined(__GNUC__)
#define PCB_COMPILER_GCC 1
#define PCB_COMPILER_CLANG 0
#define PCB_COMPILER_MSVC 0
#if PCB_PLATFORM_WINDOWS
#define PCB_COMPILER "MinGW"
#else
#define PCB_COMPILER "GCC"
#endif //MinGW check
#elif defined(__clang__)
#define PCB_COMPILER_GCC 0
#define PCB_COMPILER_CLANG 1
#define PCB_COMPILER_MSVC 0
#define PCB_COMPILER "Clang"
#elif defined(_MSC_VER)
#define PCB_COMPILER_GCC 0
#define PCB_COMPILER_CLANG 0
#define PCB_COMPILER_MSVC 1
#define PCB_COMPILER "MSVC"
#else
#error PCB Error: Unsupported compiler
#endif //compiler
#endif //PCB_COMPILER



//Section 1.3: Define useful, but often compiler-specific macros
#ifndef PCB_NoDiscard
#ifdef __cplusplus
#if __cplusplus >= 201703L
#define PCB_NoDiscard [[nodiscard]]
#else
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_NoDiscard __attribute__((warn_unused_result))
#elif PCB_COMPILER_MSVC
#define PCB_NoDiscard
#else
#define PCB_NoDiscard
#endif //Compilers
#endif //C++17
#else //C
#if __STDC_VERSION__ >= 202311L
#define PCB_NoDiscard [[nodiscard]]
#else
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_NoDiscard __attribute__((warn_unused_result))
#elif PCB_COMPILER_MSVC
#define PCB_NoDiscard
#else
#define PCB_NoDiscard
#endif //Compilers
#endif //C23
#endif //C++?
#endif //PCB_NoDiscard

#ifndef PCB_NoDiscardReason
#ifdef __cplusplus
#if __cplusplus >= 202002L
#define PCB_NoDiscardReason(reason) [[nodiscard(reason)]]
#else
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_NoDiscardReason(reason) __attribute__((warn_unused_result))
#elif PCB_COMPILER_MSVC
#define PCB_NoDiscardReason(reason)
#else
#define PCB_NoDiscardReason(reason)
#endif //Compilers
#endif //C++17
#else //C
#if __STDC_VERSION__ >= 202311L
#define PCB_NoDiscardReason(reason) [[nodiscard(reason)]]
#else
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_NoDiscardReason(reason) __attribute__((warn_unused_result))
#elif PCB_COMPILER_MSVC
#define PCB_NoDiscardReason(reason)
#else
#define PCB_NoDiscardReason(reason)
#endif //Compilers
#endif //C23
#endif //C++?
#endif //PCB_NoDiscardReason

#ifndef PCB_Deprecated
#ifdef __cplusplus
#if __cplusplus >= 201402L
#define PCB_Deprecated [[deprecated]]
#else
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_Deprecated __attribute__((deprecated))
#elif PCB_COMPILER_MSVC
#define PCB_Deprecated __declspec(deprecated)
#else
#define PCB_Deprecated
#endif //Compilers
#endif //C++14
#else //C
#if __STDC_VERSION__ >= 202311L
#define PCB_Deprecated [[deprecated]]
#else
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_Deprecated __attribute__((deprecated))
#elif PCB_COMPILER_MSVC
#define PCB_Deprecated __declspec(deprecated)
#else
#define PCB_Deprecated
#endif //Compilers
#endif //C23
#endif //C++?
#endif //PCB_Deprecated

#ifndef PCB_DeprecatedReason
#ifdef __cplusplus
#if __cplusplus >= 201402L
#define PCB_DeprecatedReason(reason) [[deprecated(reason)]]
#else
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_DeprecatedReason(reason) __attribute__((deprecated(reason)))
#elif PCB_COMPILER_MSVC
#define PCB_Deprecated(reason) __declspec(deprecated(reason))
#else
#define PCB_Deprecated(reason)
#endif //Compilers
#endif //C++14
#else //C
#if __STDC_VERSION__ >= 202311L
#define PCB_DeprecatedReason(reason) [[deprecated(reason)]]
#else
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_DeprecatedReason(reason) __attribute__((deprecated(reason)))
#elif PCB_COMPILER_MSVC
#define PCB_DeprecatedReason(reason) __declspec(deprecated(reason))
#else
#define PCB_DeprecatedReason(reason)
#endif //Compilers
#endif //C23
#endif //C++?
#endif //PCB_Deprecated

#ifndef PCB_NoReturn
#ifdef __cplusplus
#if __cplusplus >= 201103L
#define PCB_NoReturn [[noreturn]]
#else
#if PCB_COMPILER_GCC
#define PCB_NoReturn __attribute__((noreturn))
#elif PCB_COMPILER_MSVC
#define PCB_NoReturn __declspec(noreturn)
#else
#define PCB_NoReturn
#endif //Compilers
#endif //C++11
#else //C
#if __STDC_VERSION >= 202311L
#define PCB_NoReturn [[noreturn]]
#else
#if PCB_COMPILER_GCC
#define PCB_NoReturn __attribute__((noreturn))
#elif PCB_COMPILER_MSVC
#define PCB_NoReturn __declspec(noreturn)
#else
#pragma "PCB Warning: PCB_NoReturn does not mark function as one that doesn't return"
#define PCB_NoReturn
#endif //Compilers
#endif //C23
#endif //C++?
#endif //PCB_NoReturn

#ifndef PCB_ForceInline
#if (defined(__GNUC__) && __GNUC__ >= 4) || PCB_COMPILER_CLANG
#define ForceInline inline __attribute__((always_inline))
#elif PCB_COMPILER_MSVC
#define ForceInline __forceinline
#else
#define ForceInline inline
#endif //Compilers
#endif //PCB_ForceInline

#ifndef PCB_BeforeMain
#ifdef __cplusplus
//Using C++'s constructor trickery we can construct
//an empty object with a static lifetime, which means
//running a function at startup.
#define PCB_BeforeMain(f) static void f(); \
struct __##f##__ { __##f##__() { f(); } }; static __##f##__ __##f##_; \
static void f()
#else //C
#if PCB_COMPILER_MSVC
//https://stackoverflow.com/questions/1113409/attribute-constructor-equivalent-in-vc
#pragma section(".CRT$XCU",read)
#define PCB_INITIALIZER_(f,p) static void f(); \
__declspec(allocate(".CRT$XCU")) void (*f##_)() = f; \
__pragma(comment(linker,"/include:" p #f "_")) \
static void f()
#ifdef _WIN64
#define PCB_BeforeMain(f) PCB_INITIALIZER_(f,"")
#else
#define PCB_BeforeMain(f) PCB_INITIALIZER_(f,"_")
#endif
#elif PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_BeforeMain(f) static void f() __attribute__((constructor))
#else
#define PCB_BeforeMain(f) \
_Pragma("PCB Warning: function '" #f "' will not run before main because the compiler used does not support it") \
static void f()
#endif //Compilers
#endif //C++?
#endif //PCB_BeforeMain

#ifndef PCB_Unreachable
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_Unreachable __builtin_unreachable()
#elif PCB_COMPILER_MSVC
#define PCB_Unreachable (__assume(false))
#else
#pragma "PCB Warning: PCB_Unreachable does not mark unreachability"
#define PCB_Unreachable
#endif //Compilers
#endif //PCB_Unreachable



//Section 1.4: Import the C standard library,
//unless defined otherwise
#ifndef PCB_USE_CSTDLIB
#define PCB_USE_CSTDLIB 1
#endif //PCB_USE_CSTDLIB

#if defined(PCB_USE_CSTDLIB) && PCB_USE_CSTDLIB
#ifndef PCB_HAS_STDIO_H
#include <stdio.h>
#define PCB_HAS_STDIO_H
#endif //PCB_HAS_STDIO_H
#ifndef PCB_HAS_STDLIB_H
#include <stdlib.h>
#define PCB_HAS_STDLIB_H
#endif //PCB_HAS_STDLIB_H
#ifndef PCB_HAS_ASSERT_H
#include <assert.h>
#define PCB_HAS_ASSERT_H
#endif //PCB_HAS_ASSERT_H
#ifndef PCB_HAS_STRING_H
#include <string.h>
#include <strings.h>
#define PCB_HAS_STRING_H
#endif //PCB_HAS_STRING_H
#include <stdarg.h>
#include <stdbool.h>
#endif //PCB_USE_CSTDLIB?

//Section 1.5: Define functions/macros that PCB uses
//from the standard library
#ifndef PCB_realloc
#ifdef PCB_HAS_STDLIB_H
#define PCB_realloc realloc
#else
#error "PCB Error: PCB requires PCB_realloc defined, but none is available. Perhaps you can't use libc, in which case you need to #define it manually."
#define PCB_realloc(oldPtr, newSize) NULL
#endif //PCB_HAS_STDLIB_H
#endif //PCB_realloc

#ifndef PCB_free
#ifdef PCB_HAS_STDLIB_H
#define PCB_free free
#else
#error "PCB Error: PCB requires PCB_free defined, but none is available. Perhaps you can't use libc, in which case you need to #define it manually."
#define PCB_free(ptr)
#endif //PCB_HAS_STDLIB_H
#endif //PCB_free

#ifndef PCB_memcpy
#ifdef PCB_HAS_STRING_H
#define PCB_memcpy memcpy
#else
void* PCB_memcpy(void* restrict dest, const void* src, size_t n) {
    char* restrict d = (char*)dest;
    const char* restrict s = (const char*)src;
    while(n > 0) *d++ = *s++, --n;
    return dest;
}
#define PCB_memcpy PCB_memcpy
#endif //PCB_HAS_STRING_H
#endif //PCB_memcpy

#ifndef PCB_memmove
#ifdef PCB_HAS_STRING_H
#define PCB_memmove memmove
#else
void* PCB_memmove(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    if(s < d && d < s + n) { //overlap check?
        s += n; d += n;
        while(n-- > 0) *--d = *--s;
    }
    else while(n-- > 0) *d++ = *s++;
    return dest;
}
#define PCB_memmove PCB_memmove
#endif //PCB_HAS_STRING_H
#endif //PCB_memmove

#ifndef PCB_memset
#ifdef PCB_HAS_STRING_H
#define PCB_memset memset
#else
void* PCB_memset(void* s, int v, size_t n) {
    char* p = (char*)s;
    while(n-- > 0) *p++ = (char)v;
    return s;
}
#define PCB_memset PCB_memset
#endif //PCB_HAS_STRING_H
#endif //PCB_memset

#ifndef PCB_memcmp
#ifdef PCB_HAS_STRING_H
#define PCB_memcmp memcmp
#else
int PCB_memcmp(const void* p1, const void* p2, size_t n) {
    const unsigned char* x1 = (const unsigned char*)p1;
    const unsigned char* x2 = (const unsigned char*)p2;
    while(*x1 == *x2 && n > 0) { ++x1; ++x2; --n; }
    return (*x1 > *x2) - (*x1 < *x2);
}
#define PCB_memcmp PCB_memcmp
#endif //PCB_HAS_STRING_H
#endif //PCB_memcmp

#ifndef PCB_assert
#ifdef PCB_HAS_ASSERT_H
#define PCB_assert(expr) assert(expr)
#else
#pragma "PCB Warning: PCB_assert(expr) ignores expr at runtime"
#define PCB_assert(expr) ((void)(expr))
#endif //PCB_HAS_ASSERT_H
#endif //PCB_assert

//Section 1.6: Define other useful macros

//Rounds `in` up to the nearest power of 2 - the 64-bit version.
#ifndef PCB_roundUpToPowerOf2_64
#define PCB_roundUpToPowerOf2_64(in) do { \
    (in)--;                                 \
    (in) |= (in) >> 1; (in) |= (in) >> 2;   \
    (in) |= (in) >> 4; (in) |= (in) >> 8;   \
    (in) |= (in) >> 16;(in) |= (in) >> 32;  \
    (in)++;                                 \
} while(0)
#endif //PCB_roundUpToPowerOf2_64

//Rounds `in` up to the nearest power of 2 - the 32-bit version.
#ifndef PCB_roundUpToPowerOf2_32
#define PCB_roundUpToPowerOf2_32(in) do {   \
    (in)--;                                 \
    (in) |= (in) >> 1; (in) |= (in) >> 2;   \
    (in) |= (in) >> 4; (in) |= (in) >> 8;   \
    (in) |= (in) >> 16; (in)++;             \
} while(0)
#endif //PCB_roundUpToPowerOf2_32

//Rounds `in` up to the nearest power of 2. Assumes that `in` is 64-bit wide.
#ifndef PCB_roundUpToPowerOf2
#define PCB_roundUpToPowerOf2(in) PCB_roundUpToPowerOf2_64(in)
#endif //PCB_roundUpToPowerOf2

//Section 1.6.1: template<*> struct vector in C let's goooo

//Declares a struct which can be used in `PCB_Vec_*`
//macros for dynamic array functionalities.
#ifndef PCB_DECLARE_Vec
#define PCB_DECLARE_Vec(T) typedef struct { \
    T* data; \
    size_t length; \
    size_t capacity; \
} Vec_##T
#endif //PCB_DECLARE_Vec

#ifndef PCB_VEC_INITIAL_CAPACITY
#define PCB_VEC_INITIAL_CAPACITY 64
#endif //PCB_VEC_INITIAL_CAPACITY

#ifndef PCB_Vec_reserve
#define PCB_Vec_reserve(vec, howMany) do {              \
    size_t newCapacity = (vec)->capacity;               \
    if(newCapacity == 0)                                \
        newCapacity = PCB_VEC_INITIAL_CAPACITY;         \
    while((vec)->capacity + (howMany) > newCapacity)    \
        newCapacity *= 2;                               \
    (vec)->data = PCB_realloc(                          \
        (vec)->data, newCapacity * sizeof(*(vec)->data) \
    ); PCB_assert((vec)->data && "Yikes.");             \
    (vec)->capacity = newCapacity;                      \
} while(0)
#endif //PCB_Vec_reserve

#ifndef PCB_Vec_free
#define PCB_Vec_free(vec) (PCB_free((vec)->data))
#endif //PCB_Vec_free

//Appends `item` to `vec`. It does not handle
//an erroneous reallocation and aborts instead.
#ifndef PCB_Vec_append
//The old version of PCB_Vec_append
/* #define PCB_Vec_append(vec, item) do {                      \
 if((vec)->length == (vec)->capacity) {                     \
    size_t newCapacity = (vec)->capacity == 0               \
        ? PCB_VEC_INITIAL_CAPACITY : (vec)->capacity * 2;   \
    (vec)->data = PCB_realloc(                              \
        (vec)->data, sizeof(*(vec)->data) * newCapacity     \
    ); PCB_assert((vec)->data && "Yikes.");                 \
    (vec)->capacity = newCapacity;                          \
 } (vec)->data[(vec)->length++] = (item);                   \
} while(0) */
#define PCB_Vec_append(vec, item) do {      \
    if((vec)->length == (vec)->capacity) {  \
       PCB_Vec_reserve(vec, 1);             \
    } (vec)->data[(vec)->length++] = (item);\
} while(0)
#endif //PCB_Vec_append

//Appends `howMany` `items` to `vec`. The `items` argument
//must be an array which size is known at compile time with
//sizeof, while `howMany` is the number of elements in `items`
//or less. There is also `PCB_Vec_append_variadic`,
//which is more convenient in use, but requires specifying
//the type of arguments provided since C doesn't have type inference
//before C23 and `typeof` is a GNU extension that is not portable.
#ifndef PCB_Vec_append_multiple
#define PCB_Vec_append_multiple(vec, items, howMany) do {   \
    if((vec)->length + howMany > (vec)->capacity) {         \
        PCB_Vec_reserve(vec, howMany);                      \
    } for(size_t i = 0; i < howMany; i++) {                 \
        (vec)->data[i + (vec)->length] = (items)[i];        \
    } (vec)->length += (howMany);                           \
} while(0)
#endif //PCB_Vec_append_multiple

//Appends a variadic number of elements to `vec`.
//The `type` of a variadic argument is needed since
//C doesn't have type inference before C23 and `typeof`
//is a GNU extension that is not portable.
#ifndef PCB_Vec_append_variadic
#define PCB_Vec_append_variadic(vec, type, ...) do {        \
    type _items[] = { __VA_ARGS__ };                        \
    size_t _howMany = sizeof(_items) / sizeof(_items[0]);   \
    PCB_Vec_append_multiple(vec, _items, _howMany);         \
} while(0)
#endif //PCB_Vec_append_variadic

//Pops the last element from `vec`.
//If the length of `vec` is zero, the behavior
//is dependent on whether PCB_assert is defined as an aborting assert.
//If so, aborts. Otherwise the behavior is undefined.
#ifndef PCB_Vec_pop
#define PCB_Vec_pop(vec) \
    (PCB_assert((vec)->length > 0), (vec)->data[--(vec)->length])
#endif //PCB_Vec_pop

//Clears `vec`...which literally only reset its length.
//Do not use if elements hold data that needs to be destroyed.
#ifndef PCB_Vec_clear
#define PCB_Vec_clear(vec) ((vec)->length = 0)
#endif //PCB_Vec_clear

//Inserts `item` into `vec` at position `index`.
//If `index` >= current length of `vec`, nothing happens.
#ifndef PCB_Vec_insert
#define PCB_Vec_insert(vec, item, index)                 \
while((index) < (vec)->length) {                         \
    if((vec)->length == (vec)->capacity)                 \
        PCB_Vec_reserve(vec, 1);                         \
    PCB_memmove(                                             \
        (vec)->data + (index) + 1, (vec)->data + (index),\
        ((vec)->length - (index)) * sizeof(*(vec)->data) \
    ); (vec)->data[(index)] = (item);                    \
    ++(vec)->length; break;                              \
}
#endif //PCB_Vec_insert

//Erases the element at index `index` from `vec`.
//If the element holds any important value, it needs to be
//copied beforehand.
//If `index` >= current length of `vec`, nothing happens.
#ifndef PCB_Vec_erase
#define PCB_Vec_erase(vec, index)                            \
while((index) < (vec)->length) {                             \
    PCB_memmove(                                                 \
        (vec)->data + (index), (vec)->data + (index) + 1,    \
        ((vec)->length - (index) - 1) * sizeof(*(vec)->data) \
    ); --(vec)->length; break;                               \
}
#endif //PCB_Vec_erase

//Executes an expression on every element of `vec`.
//`_expr` can be a function receiving a pointer to the element
//or a macro for inline expressions.
//An example usage with a macro is as follows:
//PCB_DECLARE_Vec(int) v;
//...
//#define EXPR(x) printf("%d ", *x)
//PCB_Vec_forEach(&v, EXPR);
//#undef EXPR
//...
#ifndef PCB_Vec_forEach
#define PCB_Vec_forEach(vec, _expr) \
    for(size_t i = 0; i < (vec)->length; i++) { _expr(&(vec)->data[i]); }
#endif //PCB_Vec_forEach

//Section 1.6.2: Other macros


//Section 1.7: Import platform-specific header files
#if PCB_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
//TODO: replace this include with modular includes
//to limit the amount of stuff imported
//for faster compilation
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#elif PCB_PLATFORM_POSIX
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#endif //platform-specific APIs


//Section 2: Implementation of various functions
#ifdef __cplusplus
extern "C" {
#endif //C++

//Section 2.1: Logging
typedef enum {
    PCB_LOGLEVEL_TRACE,
    PCB_LOGLEVEL_DEBUG,
    PCB_LOGLEVEL_INFO,
    PCB_LOGLEVEL_WARN,
    PCB_LOGLEVEL_ERROR,
    PCB_LOGLEVEL_FATAL
} PCB_LogLevel;

void PCB_log(PCB_LogLevel level, const char* fmt, ...) {
    //https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
#if PCB_PLATFORM_WINDOWS
    //TODO: ANSI escape sequences are supported since Windows 10,
    //but have to be enabled with SetConsoleMode.
    typedef enum { ANSI_OFF, ANSI_ON, ANSI_ERR } ansi_esc_seq_avail;
    static char ansiEscapeSequenceAvailable = ANSI_OFF;
    if(ansiEscapeSequenceAvailable == ANSI_OFF) {} //enable it
    else if(ansiEscapeSequenceAvailable == ANSI_ERR) {} //fallback to SetConsoleTextAttribute
    //Implement those 2 ifs.

    HANDLE hStderr = GetStdHandle(STD_ERROR_HANDLE);
    if(hStderr == INVALID_HANDLE_VALUE) return;
#endif
    switch(level) {
        case PCB_LOGLEVEL_TRACE:
#if PCB_PLATFORM_WINDOWS
            fprintf(stderr, "[");
            SetConsoleTextAttribute(hStderr, 8); fprintf(stderr, "Trace");
            SetConsoleTextAttribute(hStderr, 0xf); fprintf(stderr, "]\t");
#else
            fprintf(stderr, "[\033[38;5;238mTrace\033[0m]\t");
#endif
            break;
        case PCB_LOGLEVEL_DEBUG:
#if PCB_PLATFORM_WINDOWS
        fprintf(stderr, "[");
        SetConsoleTextAttribute(hStderr, 0xb); fprintf(stderr, "Debug");
        SetConsoleTextAttribute(hStderr, 0xf); fprintf(stderr, "]\t");
#else
        fprintf(stderr, "[\033[38;5;51mDebug\033[0m]\t");
#endif
        break;
    case PCB_LOGLEVEL_INFO:
        fprintf(stderr, "[Info]\t");
        break;
    case PCB_LOGLEVEL_WARN:
#if PCB_PLATFORM_WINDOWS
        fprintf(stderr, "[");
        SetConsoleTextAttribute(hStderr, 6); fprintf(stderr, "Warn");
        SetConsoleTextAttribute(hStderr, 0xf); fprintf(stderr, "]\t");
#else
        fprintf(stderr, "[\033[38;5;214mWarn\033[0m]\t");
#endif
        break;
    case PCB_LOGLEVEL_ERROR:
#if PCB_PLATFORM_WINDOWS
        fprintf(stderr, "[");
        SetConsoleTextAttribute(hStderr, 0xc); fprintf(stderr, "Error");
        SetConsoleTextAttribute(hStderr, 0xf); fprintf(stderr, "]\t");
#else
        fprintf(stderr, "[\033[38;5;9mError\033[0m]\t");
#endif
        break;
    case PCB_LOGLEVEL_FATAL:
#if PCB_PLATFORM_WINDOWS
        fprintf(stderr, "[");
        SetConsoleTextAttribute(hStderr, 4); fprintf(stderr, "Fatal");
        SetConsoleTextAttribute(hStderr, 0xf); fprintf(stderr, "]\t");
#else
        fprintf(stderr, "[\033[1m\033[38;5;1mFatal\033[0m]\t");
#endif
        break;
    default:
        fprintf(stderr, "[Unknown] ");
        break;
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

//Section 2.2: Platform-independent (sort of) filesystem functions

//Creates a directory in the given `path`.
//Returns whether the operation succeeded.
//Failure by "it already exists" is treated as success.
//On Linux, permission field of the created directory is rw-r--r--.
bool PCB_mkdir(const char* path) {
#if PCB_PLATFORM_POSIX
    if(mkdir(path, 0644) == -1) {
        switch(errno) {
            case EEXIST: //not an error if already exists
                return true;
            default:
                PCB_log(
                    PCB_LOGLEVEL_ERROR,
                    "Failed to create directory: %s", strerror(errno)
                );
                return false;
        }
    }
    return true;
#elif PCB_PLATFORM_WINDOWS
    if(!CreateDirectory(path, NULL)) {
        DWORD err = GetLastError();
        if(err == ERROR_ALREADY_EXISTS) return true;
        PCB_log(
            PCB_LOGLEVEL_ERROR,
            "Failed to create directory with error code %d", err
        );
        return false;
    }
    return true;
#else
#error "Not implemented"
#endif //platform
}

//Section 2.3: Strings, string views, vectors of strings...
//Section 2.3.1: A vector of const char*, a shell command
typedef struct {
    const char** data;
    size_t length;
    size_t capacity;
} PCB_CStringVec;

typedef PCB_CStringVec PCB_ShellCommand;

#ifndef PCB_ShellCommand_append_arg
#define PCB_ShellCommand_append_arg(cmd, str) PCB_Vec_append(cmd, str)
#endif //PCB_ShellCommand_append_arg

#ifndef PCB_ShellCommand_append_args
#define PCB_ShellCommand_append_args(cmd, ...) \
    PCB_Vec_append_multiple( \
        cmd, \
        ((const char*[]) {__VA_ARGS__}), \
        (sizeof((const char*[]){ __VA_ARGS__ }) / sizeof(const char*)) \
    )
#endif //PCB_ShellCommand_append_args

//Section 2.3.2: Dynamic string implementation

//A dynamic array of characters with a
//trailing zero at the end - a string.
//Unlike other dynamic arrays has a concrete implementation.
//The trailing zero is not included in its length.
typedef struct {
    char* data;
    size_t length;
    size_t capacity;
} PCB_String;

//A non-owning view at a portion of some string.
//Likely does not end with a zero, keep that in mind
//when passing `data` to a function expecting a C string.
typedef struct {
    const char* data;
    size_t length;
} PCB_StringView;

//a temporary convenience macro
#define PCB_String_realloc(this, targetSize) do {           \
    size_t newCapacity = this->capacity;                    \
    if(this->capacity == 0)                                 \
        newCapacity = PCB_VEC_INITIAL_CAPACITY;             \
    while(targetSize > newCapacity) newCapacity *= 2;       \
    char* newData = PCB_realloc(this->data, newCapacity);   \
    if(newData == NULL) return false;                       \
    this->data = newData;                                   \
    this->capacity = newCapacity;                           \
} while(0)

bool PCB_String_reserve(PCB_String* this, const size_t howMany) {
    const size_t newSize = this->length + howMany;
    PCB_String_realloc(this, newSize);
    return true;
}

//Resizes `this` to fit a string of `targetLength` length.
//Truncates the string to `targetLength` if `targetLength < this->length`.
//Does nothing if `targetLength == this->length`.
//Behaves identically to `PCB_String_reserve` otherwise.
bool PCB_String_resize(PCB_String* this, const size_t targetLength) {
    if(targetLength == this->length) return true;
    else if(targetLength < this->length) {
        this->data[targetLength] = '\0';
        this->length = targetLength;
        return true;
    }
    return PCB_String_reserve(this, targetLength - this->length);
}

bool PCB_String_append(PCB_String* this, const PCB_String* other) {
    if(other->data == NULL) return true;
    if(this->length + other->length >= this->capacity) {
        const size_t targetSize = this->length + other->length + 1;
        PCB_String_realloc(this, targetSize);
    }
    PCB_memcpy(this->data + this->length, other->data, other->length + 1);
    this->length += other->length;
    return true;
}

bool PCB_String_append_cstr(PCB_String* this, const char* str) {
    size_t len = strlen(str);
    if(this->length + len >= this->capacity) {
        const size_t targetSize = this->length + len + 1;
        PCB_String_realloc(this, targetSize);
    }
    PCB_memcpy(this->data + this->length, str, len + 1);
    this->length += len;
    return true;
}

//Appends `c` to `this` `howManyTimes` times.
bool PCB_String_append_chars(PCB_String* this, const char c, const size_t howManyTimes) {
    if(this->length + howManyTimes >= this->capacity) {
        const size_t targetSize = this->length + howManyTimes + 1;
        PCB_String_realloc(this, targetSize);
    }
    PCB_memset(this->data + this->length, c, howManyTimes);
    this->length += howManyTimes;
    this->data[this->length] = '\0';
    return true;
}

//Makes `c` the last character in `this`.
//If `c` is not the last character, it appends it.
//Otherwise does nothing.
bool PCB_String_setSuffix_char(PCB_String* this, const char c) {
    if(this->data == NULL) PCB_String_realloc(this, 1);
    if(this->length == 0) {
        this->data[0] = c; this->data[++this->length] = '\0';
        return true;
    }
    if(this->data[this->length - 1] != 'c') {
        this->data[this->length] = c;
        this->data[++this->length] = '\0';
    }
    return true;
}

#undef PCB_String_realloc

PCB_String PCB_String_clone(const PCB_String* this) {
    if(this->data == NULL || this->length == 0)
        return (PCB_String){0};
    PCB_String s = {
        .data = PCB_realloc(NULL, this->length + 1),
        .length = this->length,
        .capacity = this->capacity
    };
    if(s.data == NULL) {
        s.length = s.capacity = 0;
        return s;
    }
    PCB_memcpy(s.data, this->data, s.length + 1);
    return s;
}
//Compares `a` and `b` lexicographically.
int PCB_String_compare(const PCB_String* a, const PCB_String* b) {
    if(a->data == NULL && b->data == NULL) return 0;
    else if(a->data == NULL) return 1;
    else if(b->data == NULL) return -1;
    return a->length == b->length
        ? PCB_memcmp(a->data, b->data, a->length)
        : (a->length > b->length) - (a->length < b->length);
}

//Compares `a` and `b`, case insensitive version.
int PCB_String_compare_ci(const PCB_String* a, const PCB_String* b) {
    if(a->data == NULL && b->data == NULL) return 0;
    else if(a->data == NULL) return 1;
    else if(b->data == NULL) return -1;
    return strncasecmp(a->data, b->data, a->length);
}

//Checks if `this` starts with `other`.
//If any of them is empty (i.e. `data == NULL`), returns false.
bool PCB_String_startsWith(const PCB_String* this, const PCB_String* other) {
    if(this->data == NULL || other->data == NULL) return false;
    if(other->length > this->length) return false;
    return !PCB_memcmp(this->data, other->data, other->length);
}

//Checks if `this` starts with `other`.
//If `this` is empty (i.e. `data == NULL`), returns false.
bool PCB_String_startsWith_cstr(const PCB_String* this, const char* other) {
    if(this->data == NULL) return false;
    const size_t len = strlen(other);
    if(len > this->length) return false;
    return !PCB_memcmp(this->data, other, len);
}

//Checks if `this` ends with `other`.
//If any of them is empty (i.e. `data == NULL`), returns false.
bool PCB_String_endsWith(const PCB_String* this, const PCB_String* other) {
    if(this->data == NULL || other->data == NULL) return false;
    if(other->length > this->length) return false;
    return !PCB_memcmp(
        this->data + this->length - other->length,
        other->data, other->length
    );
}

//Checks if `this` ends with `other`.
//If `this` is empty (i.e. `data == NULL`), returns false.
bool PCB_String_endsWith_cstr(const PCB_String* this, const char* other) {
    if(this->data == NULL) return false;
    const size_t len = strlen(other);
    if(len > this->length) return false;
    return !PCB_memcmp(this->data + this->length - len, other, len);
}

void PCB_String_toUpperCase(PCB_String* this) {
    if(this->data == NULL) return;
    for(size_t i = 0; i < this->length; i++) {
        if(this->data[i] >= 'a' && this->data[i] <= 'z') {
            this->data[i] -= 'a' - 'A';
        }
    }
}

PCB_String PCB_String_toUpperCase_copy(const PCB_String* this) {
    PCB_String copy = PCB_String_clone(this);
    PCB_String_toUpperCase(&copy);
    return copy;
}

void PCB_String_toLowerCase(PCB_String* this) {
    if(this->data == NULL) return;
    for(size_t i = 0; i < this->length; i++) {
        if(this->data[i] >= 'A' && this->data[i] <= 'Z') {
            this->data[i] += 'a' - 'A';
        }
    }
}

PCB_String PCB_String_toLowerCase_copy(const PCB_String* this) {
    PCB_String copy = PCB_String_clone(this);
    PCB_String_toLowerCase(&copy);
    return copy;
}

//Pops the `other->length` characters from `this` if they match.
//Returns the new length.
size_t PCB_String_pop(PCB_String* this, const PCB_String* other) {
    if(PCB_String_endsWith(this, other)) {
        this->data[this->length - other->length] = '\0';
        return this->length -= other->length;
    }
    return this->length;
}

PCB_String PCB_String_from_CStringVec(const PCB_CStringVec* cstr, const char* delimiter) {
    if(cstr->data == NULL || cstr->length == 0)
        return (PCB_String){0};
    size_t totalLength = 0;
    for(size_t i = 0; i < cstr->length; totalLength += strlen(cstr->data[i++]));
    //delimiter isn't placed at the end              v    '\0'
    totalLength += strlen(delimiter) * (cstr->length - 1) + 1;
    PCB_String str = {
        .data = PCB_realloc(NULL, totalLength),
        .length = totalLength - 1, //we don't count the '\0'
        .capacity = totalLength,
    };
    if(str.data == NULL) return (PCB_String) {0};
    char* cursor = str.data;
    for(size_t i = 0; i < cstr->length - 1; i++) {
        const char* current = cstr->data[i];
        //this is cursed...
        for(; *current; *cursor++ = *current++);
        for(current = delimiter; *current; *cursor++ = *current++);
    }
    for(const char* current = cstr->data[cstr->length - 1]; *current; *cursor++ = *current++);
    *cursor = '\0';
    return str;
}

//Section 2.4: Platform-independent (sort of) process functions.

typedef struct {
#if PCB_PLATFORM_WINDOWS
    HANDLE handle;
#elif PCB_PLATFORM_POSIX
    pid_t handle;
#endif //platform-dependent handles to processes
} PCB_Process;

PCB_Process PCB_Process_self() {
    PCB_assert(false && "Not yet implemented");
#if PCB_PLATFORM_WINDOWS

#elif PCB_PLATFORM_POSIX

#endif
}

bool PCB_Process_isValid(PCB_Process process) {
#if PCB_PLATFORM_WINDOWS
    return process.handle != INVALID_HANDLE_VALUE;
#elif PCB_PLATFORM_POSIX
    return process.handle > (pid_t)0;
#endif
}

int PCB_Process_waitForExit(PCB_Process process) {
#if PCB_PLATFORM_WINDOWS
    PCB_assert(false && "Not yet implemented");
#elif PCB_PLATFORM_POSIX
    int exitCode;
    waitpid(process.handle, &exitCode, 0);
    return exitCode;
#endif
}

PCB_Process PCB_ShellCommand_runBg(PCB_ShellCommand* command) {
#if PCB_PLATFORM_WINDOWS
    PCB_assert(false && "Not yet implemented");
#elif PCB_PLATFORM_POSIX
    if(command->length < 1) {
        PCB_log(
            PCB_LOGLEVEL_ERROR,
            "Cannot run an empty command"
        );
        return (PCB_Process){ .handle = -1 };
    }
    PCB_Process child = { .handle = fork() };
    if(child.handle == -1) {
        PCB_log(
            PCB_LOGLEVEL_ERROR,
            "Failed to create a child process: %s",
            strerror(errno)
        );
        return (PCB_Process) { .handle = -2 };
    }
    else if(child.handle == 0) {
        execvp(command->data[0], (char* const*)command->data);
        PCB_log(PCB_LOGLEVEL_ERROR, "Couldn't execute command");
        exit(1);
    }
    return child;

#endif //platform-dependent way of running a shell command
}

/**
 * @brief Runs a shell command and waits for it to exit.
 * 
 * @param command command built with `PCB_ShellCommand_*` utilities.
 * @return exit code of the executed command or a negative value
 * outside the range of possible exit codes (a negative value on
 * POSIX-compliant systems, a value < -2³² on Windows).
 * For a full description, see Appendix 1 at the bottom.
 * 
 * If you just want to check for any error simply check for a negative value.
 */
ssize_t PCB_ShellCommand_runAndWait(PCB_ShellCommand* command) {
    if(command->length < 1) {
        PCB_log(
            PCB_LOGLEVEL_ERROR,
            "Cannot run an empty command"
        );
#if PCB_PLATFORM_WINDOWS
        return (ssize_t)0x8000000000000000;
#elif PCB_PLATFORM_POSIX
        return -1;
#endif
    }

    PCB_Process process = PCB_ShellCommand_runBg(command);
    if(!PCB_Process_isValid(process))
#if PCB_PLATFORM_WINDOWS
        return (ssize_t)0x8000000000000001;
#elif PCB_PLATFORM_POSIX
        return -2;
#endif
    return PCB_Process_waitForExit(process);
}

//Section 3: PCB's build capability

#define PCB_BUILD_CAPABILITY //temporary for development
//Section 3.1: Default compiler-dependent flags

#ifdef PCB_BUILD_CAPABILITY
#ifndef PCB_DEFAULT_COMPILER_FLAGS
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_DEFAULT_COMPILER_FLAGS "-Wall -Wextra"
#elif PCB_COMPILER_MSVC
#define PCB_DEFAULT_COMPILER_FLAGS "/Wall"
#endif //compilers
#endif //PCB_DEFAULT_COMPILER_FLAGS
#endif //PCB_BUILD_CAPABILITY


//Section 3.2: Variables used for building.


//Path to the compiler used.
//Defaults to the compiler's name used to build this file.
static const char* PCB_compiler_path =
//this is ugly, but it has to be like that
#if PCB_COMPILER_GCC
#ifdef __cplusplus
"g++"
#else
"gcc"
#endif //C++
#elif PCB_COMPILER_CLANG
#ifdef __cplusplus
"clang++"
#else
"clang"
#endif //C++
#elif PCB_COMPILER_MSVC
"cl"
#endif //default compiler paths
;

//The language version used to compile source files.
//Defaults to the version used to build this file.
static long PCB_lang_version =
//same as above
#ifdef __cplusplus
__cplusplus
#else
__STDC_VERSION__
#endif //C++
;
//Path to the build directory. Defaults to "build/".
static const char* PCB_build_dir = "build/";
//Vector of paths to source directories. Initially empty.
static PCB_CStringVec PCB_src_dirs = {0};
//Vector of paths to include directories. Initially empty.
static PCB_CStringVec PCB_include_dirs = {0};
//Vector of names of libraries to link dynamically. Initially empty.
static PCB_CStringVec PCB_libs = {0};
//Vector of names of libraries to link *statically*. Initially empty.
static PCB_CStringVec PCB_static_libs = {0};

//Internal buffer used for commands when building. Do not use.
static PCB_ShellCommand PCB_commandBuffer = {0};

//It doesn't really make sense to have multiple *main*
//build directories, hence it's not a vector, unlike others.
#ifndef PCB_Set_Build_Directory
#define PCB_Set_Build_Directory(path) PCB_build_dir = path
#endif //PCB_Set_Build_Directory


#ifndef PCB_Add_Source_Directory
#define PCB_Add_Source_Directory(path) PCB_Vec_append(&PCB_src_dirs, path)
#endif //PCB_Add_Source_Directory

#ifndef PCB_Add_Include_Directory
#define PCB_Add_Include_Directory(path) PCB_Vec_append(&PCB_include_dirs, path)
#endif //PCB_Add_Include_Directory

#ifndef PCB_Add_Library
#define PCB_Add_Library(name) PCB_Vec_append(&PCB_libs, name)
#endif //PCB_Add_Library

#ifndef PCB_Add_Static_Library
#define PCB_Add_Static_Library(name) PCB_Vec_append(&PCB_static_libs, name)
#endif //PCB_Add_Static_Library



//Section 3.3: Functions used for building.


int PCB_build_file(PCB_String* in, PCB_String* out) {
    // PCB_assert(false && "Not yet implemented");
    (void)in; (void)out;
    PCB_log(PCB_LOGLEVEL_INFO, "In: %s, out: %s", in->data, out->data);

    // PCB_commandBuffer.length = 0;
    // PCB_ShellCommand_append_arg(&PCB_commandBuffer, PCB_compiler_path);
    return 0;
}

//TODO: document return values
//this function expects that `from` and `to` have *any* data allocated
int PCB_build_directory(PCB_String* from, PCB_String* to) {
#if PCB_PLATFORM_WINDOWS
    PCB_assert(false && "Not yet implemented");
#elif PCB_PLATFORM_POSIX
#define PCB_err(err) { code = err; goto error; }
    DIR* cwd = opendir(from->data);
    if(cwd == NULL) {
        PCB_log(
            PCB_LOGLEVEL_ERROR,
            "Could not open directory %s: %s",
            from->data, strerror(errno)
        );
        return -1;
    }
    int code = 0;
#ifdef __STRICT_ANSI__
    struct stat st;
#endif
    for(struct dirent* entry = readdir(cwd); entry != NULL; entry = readdir(cwd)) {
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
        size_t oldFromLength = from->length;
        size_t oldToLength = to->length;
#ifdef __STRICT_ANSI__
        if(!PCB_String_append_cstr(from, entry->d_name)) PCB_err(ENOMEM)
        if(stat(from->data, &st) == -1) {
            switch(errno) {
                case ELOOP:
                case ENAMETOOLONG:
                case ENOENT:
                case EOVERFLOW:
                    PCB_log(
                        PCB_LOGLEVEL_WARN,
                        "Skipping %s: %s",
                        from->data, strerror(errno)
                    );
                    continue;
                case ENOMEM:
                    PCB_log(
                        PCB_LOGLEVEL_FATAL,
                        "The system has ran out of memory."
                    );
                    PCB_err(ENOMEM)
                default: PCB_Unreachable;
            }
        }
        if(S_ISDIR(st.st_mode)) {
#else
        if(entry->d_type == DT_DIR) {
            if(!PCB_String_append_cstr(from, entry->d_name)) PCB_err(ENOMEM)
#endif //pesky, but useful GNU extensions
            if(!PCB_String_append_cstr(to, entry->d_name)) PCB_err(ENOMEM)
            if(!PCB_String_append_chars(from, '/', 1)) PCB_err(ENOMEM)
            if(!PCB_String_append_chars(to, '/', 1)) PCB_err(ENOMEM)
            //recursively build the subdirectory
            if((code = PCB_build_directory(from, to)) != 0) goto error;
        }
#ifdef __STRICT_ANSI__
        else if(S_ISREG(st.st_mode)) {
#else
        else if(entry->d_type == DT_REG) {
            if(!PCB_String_append_cstr(from, entry->d_name)) PCB_err(ENOMEM)
#endif //pesky, but useful GNU extensions
            if(PCB_String_endsWith_cstr(from, ".c")) {
                if(!PCB_String_append_cstr(to, entry->d_name)) PCB_err(ENOMEM)
                to->data[to->length - 1] = 'o';
                if((code = PCB_build_file(from, to)) != 0) goto error;
            }
        }
        //restore old length in a LIFO fashion
        from->data[from->length = oldFromLength] = '\0';
        to->data[to->length = oldToLength] = '\0';
        continue;
        error: {
            from->data[from->length = oldFromLength] = '\0';
            to->data[to->length = oldToLength] = '\0';
            closedir(cwd);
            return code;
        }
    }
    closedir(cwd);

    return 0;
#undef PCB_err
#endif //platform-dependent implementation
}

int PCB_build(int argc, char** argv) {
#define PCB_defer(v) { code = v; goto defer; }
    int code = 0;
    PCB_assert(false && "Unfinished");
    (void)argc; (void)argv;
    (void)PCB_static_libs;
    (void)PCB_libs;
    (void)PCB_include_dirs;
    (void)PCB_compiler_path;
    (void)PCB_lang_version;
    (void)PCB_commandBuffer;

    PCB_String srcPath = {0};
    PCB_String buildPath = {0};
    if(!PCB_String_append_cstr(&buildPath, PCB_build_dir)) PCB_defer(ENOMEM)
    if(!PCB_String_setSuffix_char(&buildPath, '/')) PCB_defer(ENOMEM)
    //for now we won't distinguish debug and release
    //builds and keep both in the same build directory
    if(PCB_src_dirs.length > 1) {
        for(size_t i = 0; i < PCB_src_dirs.length; i++) {

        }       
    }
    for(size_t i = 0; i < PCB_src_dirs.length; i++) {

    }
    defer:
        PCB_free(srcPath.data);
        PCB_free(buildPath.data);
        return code;
#undef PCB_defer
}
#endif //PCB_BUILD_CAPABILITY

#ifdef __cplusplus
}
#endif //C++

//Appendix 1: Extended documentation of certain functions



//PCB_ShellCommand_runAndWait:
//On Linux:
//The function creates a pipe to check if an error
//happened when attempting to run the command. This does not
//mean however that the command itself failed - only that
//it couldn't be run.
//A return value of -1 indicates that an empty command
//was passed.
//A return value of -2 indicates that the pipe couldn't be
//created.
//A return value of -3 indicates that creating a child process
//to run the command failed.
//A return value of -4 indicates that the command couldn't be run.
//Any other return value can be interpreted as the exit code
//of the executed shell command.
//On Windows:
//TODO: not finished

//Functions removed/unused/otherwise
ssize_t PCB_ShellCommand_run_and_wait_old(PCB_ShellCommand* command) {
#if PCB_PLATFORM_WINDOWS
    if(command->length == 0) {
        PCB_log(PCB_LOGLEVEL_ERROR, "Cannot run an empty command");
        return (ssize_t)0x8000000000000000;
    }
    STARTUPINFO startupinfo = { .cb = sizeof(startupinfo) };
    PROCESS_INFORMATION pInfo;

    PCB_String s = PCB_String_from_CStringVec(command, " ");
    PCB_log(PCB_LOGLEVEL_DEBUG, "\"%s\" %llu", s.data, s.length);

    if(!CreateProcess(
        NULL, s.data,
        NULL, NULL, true, 0,
        NULL, NULL, &startupinfo,
        &pInfo
    )) {
        PCB_log(
            PCB_LOGLEVEL_ERROR,
            "Failed to create a child process with error code %d",
            GetLastError()
        );
        return (ssize_t)0x8000000000000001;
    }
    if(WaitForSingleObject(pInfo.hProcess, INFINITE) != WAIT_OBJECT_0)
        PCB_Unreachable; //surely...
    DWORD exitCode;
    if(GetExitCodeProcess(pInfo.hProcess, &exitCode))
        PCB_Unreachable; //...right?
    PCB_free(s.data);
    return exitCode;
#elif PCB_PLATFORM_LINUX
    if(command->length < 1) {
        PCB_log(PCB_LOGLEVEL_ERROR, "Cannot run an empty command");
        return -1;
    }
    PCB_Vec_append(command, NULL);

    //For better debugging, we'll use a pipe to distinguish between
    //an error in the shell command and an error in attempting
    //to run the shell command.
    int p[2];
    if(pipe(p) == -1) {
        PCB_log(PCB_LOGLEVEL_ERROR, "Failed to create a pipe: %s", strerror(errno));
        return -2;
    }
    pid_t child = fork();
    if(child == -1) {
        PCB_log(
            PCB_LOGLEVEL_ERROR,
            "Failed to create a child process: %s",
            strerror(errno)
        );
        return -3;
    }
    else if(child == 0) {
        close(p[0]);
        execvp(command->data[0], (char* const*)command->data);
        //The following code will not be executed if execvp succeeds.
        //If it fails, we need to send a message to the parent
        //via a pipe - anything will suffice
        write(p[1], "", 1);
        close(p[1]);
        exit(1);
    }
    close(p[1]);
    int status;
    waitpid(child, &status, 0);
    char c;
    switch(read(p[0], &c, 1)) {
        case -1: //can't actually fail...or can it?
            PCB_Unreachable;
        case 0: //nothing was written to the pipe = no error
            close(p[0]);
            return WEXITSTATUS(status);
        default: //something was written to the pipe = execvp failed
            close(p[0]);
            return -4;
    }
#endif
}

//Appendix 2: Changelog
//Version 0.1.1:
//- Added PCB_HAS_STRING_H macro for libc's string.h header detection
//- Added PCB_memcpy, PCB_memmove, PCB_memset, PCB_memcmp macros
//  which map to libc if available, otherwise they map to
//  equivalent functions with the same name implemented within
//  PCB itself; memcpy, memmove, memset, memcmp are therefore
//  substituted with PCB_ versions
//Version 0.1.0:
//- Moved platform identification to section 1.1
//- Added support for some Apple platforms
//- Added identification of POSIX-compliant platforms
//- Moved compiler identification to section 1.2
//- Moved compiler-specific macros to section 1.3
//- Changed "#error" statement start with "PCB Error"
//- Added strings.h header for case-insensitive C-strings
//- Changed Linux-specific implementations to POSIX-specific
//- Added PCB_String_setSuffix_char function
//- Added a missing #endif at "#ifdef PCB_BUILD_CAPABILITY"
//- Reinforced PCB_build_directory for errors, now returns a status code
//Version 0.0.1: Initial version

#endif //PCB_H