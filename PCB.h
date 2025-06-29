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
#define PCB_VERSION_MINOR 2
#endif //PCB_VERSION_MINOR

#ifndef PCB_VERSION_PATCH
#define PCB_VERSION_PATCH 0
#endif //PCB_VERSION_MAJOR

#ifndef PCB_VERSION
#define PCB_VERSION (PCB_VERSION_MAJOR * 1000000 + PCB_VERSION_MINOR * 1000 + PCB_VERSION_PATCH)
#endif //PCB_VERSION

#if !defined(__STDC_VERSION__) && !defined(__cplusplus)
#error "PCB Error: C89 is not supported"
#endif //C89/90?

#ifdef __cplusplus
extern "C" {
#endif //C++

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
#endif //platform
#endif //PCB_PLATFORM_POSIX

#if PCB_PLATFORM_POSIX
#ifdef __cplusplus
#if __cplusplus >= 201103L
#define _XOPEN_SOURCE 700
#else
#define _XOPEN_SOURCE 500
#endif //C++11
#else //C
#if __STDC_VERSION__ >= 201112L
#define _XOPEN_SOURCE 700
#elif __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif //C versions
#endif //C++?
#endif //POSIX sources required locally

//Section 1.2: Identify the compiler used to compile this code

#ifndef PCB_COMPILER
#if defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#define PCB_COMPILER_GCC 1
#define PCB_COMPILER_CLANG 0
#define PCB_COMPILER_MSVC 0
#if PCB_PLATFORM_WINDOWS
#define PCB_COMPILER "MinGW"
#ifdef __cplusplus
#define PCB_COMPILER_PATH "g++"
#else
#define PCB_COMPILER_PATH "gcc"
#endif //C++?
#else
#ifdef __MINGW32__
#define PCB_COMPILER "MinGW"
#ifdef __cplusplus
#define PCB_COMPILER_PATH "x86_64-w64-mingw32-g++"
#else
#define PCB_COMPILER_PATH "x86_64-w64-mingw32-gcc"
#endif //C++?
#else
#define PCB_COMPILER "GCC"
#ifdef __cplusplus
#define PCB_COMPILER_PATH "g++"
#else
#define PCB_COMPILER_PATH "gcc"
#endif //C++?
#endif //MinGW check
#endif //platform
#elif defined(__clang__)
#define PCB_COMPILER_GCC 0
#define PCB_COMPILER_CLANG 1
#define PCB_COMPILER_MSVC 0
#define PCB_COMPILER "Clang"
#ifdef __cplusplus
#define PCB_COMPILER_PATH "clang++"
#else
#define PCB_COMPILER_PATH "clang"
#endif //C++?
#elif defined(_MSC_VER) && !defined(__clang__)
#define PCB_COMPILER_GCC 0
#define PCB_COMPILER_CLANG 0
#define PCB_COMPILER_MSVC 1
#define PCB_COMPILER "MSVC"
#define PCB_COMPILER_PATH "cl"
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
#if __STDC_VERSION__ >= 202311L
#define PCB_NoReturn [[noreturn]]
#else
#if PCB_COMPILER_GCC
#define PCB_NoReturn __attribute__((noreturn))
#elif PCB_COMPILER_CLANG
#define PCB_NoReturn _Noreturn
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

#ifndef PCB_Printf_Format
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_Printf_Format(fmtIndex, rest) __attribute__((format(printf, fmtIndex, rest)))
#else
#define PCB_Printf_Format(fmtIndex, rest)
#endif //compilers
#endif //PCB_Printf_Format

#ifndef PCB_Cleanup
#ifdef PCB_WANT_CLEANUP
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_Cleanup(f) __attribute__((cleanup(f)))
#else
#error "Cleanup attribute is unavailable with the current compiler or the compiler is not supported."
#define PCB_Cleanup(f)
#endif //Compilers
#endif //PCB_WANT_CLEANUP
#endif //PCB_Cleanup




//Section 1.4: Import libc, unless this macro is defined as 0
#ifndef PCB_USE_CSTDLIB
#define PCB_USE_CSTDLIB 1
#endif //PCB_USE_CSTDLIB

#if defined(PCB_USE_CSTDLIB) && PCB_USE_CSTDLIB
//for "_s" functions
#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif //__STDC_WANT_LIB_EXT1__

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
#include <stdint.h>
#include <stddef.h>
//A useful command to list errno info: errno -l | sort -k2 -n
#include <errno.h>
#else
//fallback for no booleans
#if !defined(__cplusplus) && defined(__STDC_VERSION__) && __STDC_VERSION__ < 202311L
#ifndef PCB_BOOL_LOCALLY_DEFINED
#define PCB_BOOL_LOCALLY_DEFINED
#ifndef bool
#define bool _Bool
#endif //bool
#ifndef true
#define true 1
#endif //true
#ifndef false
#define false 0
#endif //false
#endif //PCB_BOOL_LOCALLY_DEFINED
#endif //bool

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
#define PCB_free(ptr) free(ptr)
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

#ifndef PCB_strcmp
#ifdef PCB_HAS_STRING_H
#define PCB_strcmp strcmp
#else
int PCB_strcmp(const char* s1, const char* s2) {
    const unsigned char* x1 = (const unsigned char*)s1;
    const unsigned char* x2 = (const unsigned char*)s2;
    while(*x1 && *x1 == *x2) { ++x1; ++x2; }
    return (*x1 > *x2) - (*x1 < *x2);
}
#define PCB_strcmp PCB_strcmp
#endif //PCB_HAS_STRING_H
#endif //PCB_strcmp

#ifndef PCB_strncmp
#ifdef PCB_HAS_STRING_H
#define PCB_strncmp strncmp
#else
int PCB_strncmp(const char* s1, const char* s2, size_t n) {
    const unsigned char* x1 = (const unsigned char*)s1;
    const unsigned char* x2 = (const unsigned char*)s2;
    while(n > 0 && *x1 && *x1 == *x2) { ++x1; ++x2; --n; }
    return n == 0 ? 0 : ((*x1 > *x2) - (*x1 < *x2));
}
#define PCB_strncmp PCB_strncmp
#endif //PCB_HAS_STRING_H
#endif //PCB_strncmp

#ifndef PCB_strlen
#ifdef PCB_HAS_STRING_H
#define PCB_strlen strlen
#else
size_t PCB_strlen(const char* s) {
    const char* cursor = s; while(*cursor++);
    return (size_t)(cursor - s);
}
#define PCB_strlen PCB_strlen
#endif ////PCB_HAS_STRING_H
#endif //PCB_strlen

#ifndef PCB_assert
#ifdef PCB_HAS_ASSERT_H
#define PCB_assert(expr) assert(expr)
#else
#pragma "PCB Warning: PCB_assert(expr) ignores expr at runtime"
#define PCB_assert(expr) ((void)(expr))
#endif //PCB_HAS_ASSERT_H
#endif //PCB_assert

//Section 1.6: Define other useful macros
#ifndef PCB_TODO
#define PCB_TODO(msg) PCB_assert(0 && msg " not yet implemented")
#endif //PCB_TODO

#ifndef PCB_ARRAY_LEN
#define PCB_ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif //PCB_ARRAY_LEN

#ifndef PCB_SHIFT
#define PCB_SHIFT(xs, xs_sz) (PCB_assert((xs) > 0), --(xs), *(xs_sz)++)
#endif //PCB_SHIFT



//Section 1.6.1: template<*> struct vector in C let's goooo

#ifndef PCB_VEC_INITIAL_CAPACITY
#define PCB_VEC_INITIAL_CAPACITY 64
#endif //PCB_VEC_INITIAL_CAPACITY

#ifndef PCB_Vec_reserve
/**
 * @brief Reserves `howMany` additional slots in `vec`.
 *
 * If reallocation fails, `vec`'s capacity remains unchanged.
 */
#define PCB_Vec_reserve(vec, howMany) do {                  \
    size_t new__capacity__ = (vec)->capacity;               \
    if(new__capacity__ == 0)                                \
        new__capacity__ = PCB_VEC_INITIAL_CAPACITY;         \
    while((vec)->capacity + (howMany) > new__capacity__) {  \
        new__capacity__ *= 2;                               \
    } void* new__data__ = PCB_realloc(                      \
        (vec)->data, new__capacity__ * sizeof(*(vec)->data) \
    ); if(new__data__ == NULL) break;                       \
    (vec)->data = new__data__;                              \
    (vec)->capacity = new__capacity__;                      \
} while(0)
#endif //PCB_Vec_reserve

#ifndef PCB_Vec_free
#define PCB_Vec_free(vec) PCB_free((vec)->data)
#endif //PCB_Vec_free

#ifndef PCB_Vec_destroy
/**
 * @brief Destroys `vec`, i.e. frees its buffer and resets fields
 * to their default values (0 and NULL).
 */
#define PCB_Vec_destroy(vec) do {           \
    PCB_Vec_free(vec); (vec)->data = NULL;  \
    (vec)->length = (vec)->capacity = 0;    \
} while(0)
#endif //PCB_Vec_destroy

#ifndef PCB_Vec_append
/**
 * @brief Appends `item` to `vec`.
 *
 * If reallocation fails, `vec`'s capacity remains unchanged.
 */
#define PCB_Vec_append(vec, item) do {       \
    const size_t cu__P__ = (vec)->capacity;  \
    if((vec)->length == (vec)->capacity) {   \
        PCB_Vec_reserve(vec, 1);             \
        if((vec)->capacity == cu__P__) break;\
    } (vec)->data[(vec)->length++] = (item); \
} while(0)
#endif //PCB_Vec_append

#ifndef PCB_Vec_append_multiple
/**
 * @brief Appends `howMany` `items` to `vec`.
 *
 * There is also `PCB_Vec_append_variadic`, which is more convenient in use,
 * but requires specifying the type of arguments provided since C doesn't have
 * type inference before C23 and `typeof` is a GNU extension, so it's not portable.
 */
#define PCB_Vec_append_multiple(vec, items, howMany) do {           \
    if((vec)->length + howMany > (vec)->capacity) {                 \
        PCB_Vec_reserve(vec, howMany);                              \
    } for(size_t IiindeX__ = 0; IiindeX__ < howMany; IiindeX__++) { \
        (vec)->data[IiindeX__ + (vec)->length] = (items)[IiindeX__];\
    } (vec)->length += (howMany);                                   \
} while(0)
#endif //PCB_Vec_append_multiple

#ifndef PCB_Vec_append_variadic
/**
 * @brief Appends a variadic number of elements to `vec`.
 *
 * The `type` of a variadic argument is needed since C doesn't have
 * type inference before C23 and `typeof` is a GNU
 * extension that is not portable.
 */
#define PCB_Vec_append_variadic(vec, type, ...) do {          \
    type itEMs__[] = { __VA_ARGS__ };                         \
    size_t hOw_mAnY__ = sizeof(itEMs__) / sizeof(itEMs__[0]); \
    PCB_Vec_append_multiple(vec, itEMs__, hOw_mAnY__);        \
} while(0)
#endif //PCB_Vec_append_variadic

#ifndef PCB_Vec_pop
/**
 * @brief Pops the last element from `vec`.
 *
 * If the length of `vec` is zero, the behavior is dependent on whether
 * `PCB_assert` is defined as an aborting assertion. 
 * If so, aborts. Otherwise the behavior is undefined.
 */
#define PCB_Vec_pop(vec) \
    (PCB_assert((vec)->length > 0), (vec)->data[--(vec)->length])
#endif //PCB_Vec_pop

#ifndef PCB_Vec_last
/**
 * @brief Returns a pointer to the last element of `vec`.
 */
#define PCB_Vec_last(vec) \
    (PCB_assert((vec)->length > 0), &((vec)->data[(vec)->length - 1]))
#endif //PCB_Vec_last

#ifndef PCB_Vec_clear
/**
 * @brief Clears `vec`...which literally only reset its length.
 * Do not use if elements hold data that needs to be destroyed.
 */
#define PCB_Vec_clear(vec) ((vec)->length = 0)
#endif //PCB_Vec_clear

#ifndef PCB_Vec_insert
/**
 * @brief Inserts `item` into `vec` at position `index`.
 *
 * If `index` >= current length of `vec`, nothing happens.
 */
#define PCB_Vec_insert(vec, item, index)                     \
while((index) < (vec)->length) {                             \
    if((vec)->length == (vec)->capacity)                     \
        PCB_Vec_reserve(vec, 1);                             \
    PCB_memmove(                                             \
        (vec)->data + (index) + 1, (vec)->data + (index),    \
        ((vec)->length - (index)) * sizeof(*(vec)->data)     \
    ); (vec)->data[(index)] = (item); ++(vec)->length; break;\
}
#endif //PCB_Vec_insert

#ifndef PCB_Vec_erase
/**
 * @brief Erases the element at index `index` from `vec`.
 *
 * If the element holds any important value, it needs to be copied beforehand.
 * If `index` >= current length of `vec`, nothing happens.
 */
#define PCB_Vec_erase(vec, index)                           \
while((index) < (vec)->length) {                            \
    PCB_memmove(                                            \
        (vec)->data + (index), (vec)->data + (index) + 1,   \
        ((vec)->length - (index) - 1) * sizeof(*(vec)->data)\
    ); --(vec)->length; break;                              \
}
#endif //PCB_Vec_erase

#ifndef PCB_Vec_forEach
/**
 * @brief Executes an expression on every element of `vec`.
 *
 * `expr` can be a function receiving a pointer to the element
 * or a macro for inline expressions.
 * An example usage with a macro is as follows:
 * ```c
 * //struct with `data`, `length` and `capacity` fields
 * Vec_int v = {0};
 * ...
 * #define EXPR(ptr) printf("%d ", *ptr)
 * PCB_Vec_forEach(&v, EXPR);
 * #undef EXPR
 * ...
 * ```
 */
#define PCB_Vec_forEach(vec, expr) \
    for(size_t iINDex__ = 0; iINDex__ < (vec)->length; iINDex__++) \
        { expr(&(vec)->data[iINDex__]); }
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


#ifdef PCB_BUILD_DYN
#ifndef PCB_DYN
#define PCB_DYN
#endif //PCB_DYN
#endif //PCB_BUILD_DYN implies PCB_DYN

#ifndef PCBAPI
#if PCB_PLATFORM_WINDOWS
#ifdef PCB_BUILD_DYN //we are building a DLL
#define PCBAPI __declspec(dllexport)
#elif defined(PCB_DYN) //we are *using* a DLL
#define PCBAPI __declspec(dllimport)
#endif //DLL-related options
#else
#ifdef PCB_BUILD_DYN
#define PCBAPI __attribute__((visibility("default")))
#endif //override visibility on non-Windows platforms
#endif //platform
#endif //PCBAPI

#ifndef PCBAPI
#define PCBAPI
#endif //PCBAPI

#ifndef PCBCALL
#if PCB_PLATFORM_WINDOWS
#define PCBCALL __cdecl
#else
#define PCBCALL
#endif //use C calling convention on Windows
#endif //PCBCALL, by default the C calling convention

//Section 2: Implementation of various functions


//Section 2.1: Logging, messages, error handling

typedef enum {
    PCB_LOGLEVEL_NONE,	PCB_LOGLEVEL_NONE_NL,  //without the prefix
    PCB_LOGLEVEL_TRACE,	PCB_LOGLEVEL_TRACE_NL,
    PCB_LOGLEVEL_DEBUG,	PCB_LOGLEVEL_DEBUG_NL,
    PCB_LOGLEVEL_INFO,	PCB_LOGLEVEL_INFO_NL,
    PCB_LOGLEVEL_WARN,	PCB_LOGLEVEL_WARN_NL,
    PCB_LOGLEVEL_ERROR,	PCB_LOGLEVEL_ERROR_NL,
    PCB_LOGLEVEL_FATAL,	PCB_LOGLEVEL_FATAL_NL
    //With '\n'         Without '\n'
} PCB_LogLevel;

void PCB_log(PCB_LogLevel level, const char* fmt, ...) {
    //https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
#if PCB_PLATFORM_WINDOWS
    //TODO: ANSI escape sequences are supported since Windows 10,
    //but have to be enabled with SetConsoleMode.
    enum { ANSI_OFF, ANSI_ON, ANSI_ERR };
    static char ansiEscapeSequenceAvailable = ANSI_OFF;
    if(ansiEscapeSequenceAvailable == ANSI_OFF) {} //enable it
    else if(ansiEscapeSequenceAvailable == ANSI_ERR) {} //fallback to SetConsoleTextAttribute
    //Implement those 2 ifs.

    HANDLE hStderr = GetStdHandle(STD_ERROR_HANDLE);
    if(hStderr == INVALID_HANDLE_VALUE) return;
#endif
    switch(level) {
        case PCB_LOGLEVEL_NONE:
        case PCB_LOGLEVEL_NONE_NL:
            break;
        case PCB_LOGLEVEL_TRACE:
        case PCB_LOGLEVEL_TRACE_NL:
#if PCB_PLATFORM_WINDOWS
            fprintf(stdout, "[");
            SetConsoleTextAttribute(hStderr, 8); fprintf(stdout, "Trace");
            SetConsoleTextAttribute(hStderr, 0xf); fprintf(stdout, "]\t");
#else
            fprintf(stdout, "[\033[38;5;238mTrace\033[0m]\t");
#endif
            break;
        case PCB_LOGLEVEL_DEBUG:
        case PCB_LOGLEVEL_DEBUG_NL:
#if PCB_PLATFORM_WINDOWS
        fprintf(stdout, "[");
        SetConsoleTextAttribute(hStderr, 0xb); fprintf(stdout, "Debug");
        SetConsoleTextAttribute(hStderr, 0xf); fprintf(stdout, "]\t");
#else
        fprintf(stdout, "[\033[38;5;51mDebug\033[0m]\t");
#endif
        break;
    case PCB_LOGLEVEL_INFO:
    case PCB_LOGLEVEL_INFO_NL:
        fprintf(stdout, "[Info]\t");
        break;
    case PCB_LOGLEVEL_WARN:
    case PCB_LOGLEVEL_WARN_NL:
#if PCB_PLATFORM_WINDOWS
        fprintf(stdout, "[");
        SetConsoleTextAttribute(hStderr, 6); fprintf(stdout, "Warn");
        SetConsoleTextAttribute(hStderr, 0xf); fprintf(stdout, "]\t");
#else
        fprintf(stdout, "[\033[38;5;214mWarn\033[0m]\t");
#endif
        break;
    case PCB_LOGLEVEL_ERROR:
    case PCB_LOGLEVEL_ERROR_NL:
#if PCB_PLATFORM_WINDOWS
        fprintf(stderr, "[");
        SetConsoleTextAttribute(hStderr, 0xc); fprintf(stderr, "Error");
        SetConsoleTextAttribute(hStderr, 0xf); fprintf(stderr, "]\t");
#else
        fprintf(stderr, "[\033[38;5;9mError\033[0m]\t");
#endif
        break;
    case PCB_LOGLEVEL_FATAL:
    case PCB_LOGLEVEL_FATAL_NL:
#if PCB_PLATFORM_WINDOWS
        fprintf(stderr, "[");
        SetConsoleTextAttribute(hStderr, 4); fprintf(stderr, "Fatal");
        SetConsoleTextAttribute(hStderr, 0xf); fprintf(stderr, "]\t");
#else
        fprintf(stderr, "[\033[1m\033[38;5;1mFatal\033[0m]\t");
#endif
        break;
    }
    va_list args;
    va_start(args, fmt);
    //Yes, dear reader. This switch-case shouldn't be written like this.
    //However, I hate excessive whitespace.
    switch(level) {
        case PCB_LOGLEVEL_NONE:  case PCB_LOGLEVEL_NONE_NL:
        case PCB_LOGLEVEL_TRACE: case PCB_LOGLEVEL_TRACE_NL:
        case PCB_LOGLEVEL_DEBUG: case PCB_LOGLEVEL_DEBUG_NL:
        case PCB_LOGLEVEL_INFO:  case PCB_LOGLEVEL_INFO_NL:
        case PCB_LOGLEVEL_WARN:  case PCB_LOGLEVEL_WARN_NL:
            vfprintf(stdout, fmt, args); break;
        case PCB_LOGLEVEL_ERROR: case PCB_LOGLEVEL_ERROR_NL:
        case PCB_LOGLEVEL_FATAL: case PCB_LOGLEVEL_FATAL_NL:
            vfprintf(stderr, fmt, args); break;
    }
    va_end(args);
    switch(level) {
        case PCB_LOGLEVEL_NONE:  case PCB_LOGLEVEL_TRACE:
        case PCB_LOGLEVEL_DEBUG: case PCB_LOGLEVEL_INFO:
        case PCB_LOGLEVEL_WARN:
            fprintf(stdout, "\n"); break;
        case PCB_LOGLEVEL_ERROR: case PCB_LOGLEVEL_FATAL:
            fprintf(stderr, "\n"); break;
        case PCB_LOGLEVEL_NONE_NL:  case PCB_LOGLEVEL_TRACE_NL:
        case PCB_LOGLEVEL_DEBUG_NL: case PCB_LOGLEVEL_INFO_NL:
        case PCB_LOGLEVEL_WARN_NL:  case PCB_LOGLEVEL_ERROR_NL:
        case PCB_LOGLEVEL_FATAL_NL:
            break;
    }
}

#ifndef PCB_logTrace
#ifdef PCB_DEBUG
#define PCB_logTrace(...) PCB_log(PCB_LOGLEVEL_TRACE, __VA_ARGS__)
#else
#define PCB_logTrace(...)
#endif //PCB_DEBUG
#endif //PCB_logTrace

#ifndef PCB_logDebug
#ifdef PCB_DEBUG
#define PCB_logDebug(...) PCB_log(PCB_LOGLEVEL_DEBUG, __VA_ARGS__)
#else
#define PCB_logDebug(...)
#endif //PCB_DEBUG
#endif //PCB_logDebug


int PCB_GetError(void) {
#if PCB_PLATFORM_WINDOWS
    return (int)GetLastError();
#elif PCB_PLATFORM_POSIX
    return errno;
#endif //platform
}

int PCB_GetErrorMessage(int errnum, char* buf, size_t bufSize) {
#if PCB_PLATFORM_WINDOWS
    DWORD l = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errnum, 0, buf, (DWORD)bufSize, NULL
    );
    if(l == 0) return (int)GetLastError();
    return 0;
#elif PCB_PLATFORM_POSIX
//this code right here is a very good example of xkcd 927
#ifdef _GNU_SOURCE
    char* errStr = strerror_r(errnum, buf, bufSize);
    if(buf != errStr) snprintf(buf, bufSize, "%s", errStr);
    return 0;
#elif defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L
    int code = strerror_r(errnum, buf, bufSize);
    if(code >= 0) return code; //glibc >= 2.13
    return errno; //glibc < 2.13
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && defined(__STDC_LIB_EXT1__)
#error test
    return strerror_s(buf, bufSize, errnum);
#else
    snprintf(buf, bufSize, "%s", strerror(errnum));
    return 0;
#endif //this is really annoying...
#endif //platform
}

//Log the latest error obtained from PCB_GetError() to stderr.
//Otherwise functions similarly to `printf`.
void PCB_logLatestError(const char* fmt, ...) {
    char buf[256] = {0};
    if(PCB_GetErrorMessage(
        PCB_GetError(), buf, sizeof(buf))
    ) return;
    PCB_log(PCB_LOGLEVEL_ERROR_NL, ""); //quick'n'dirty hack
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, ": %s\n", buf);
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
                PCB_logLatestError("Failed to create directory \"%s\"", path);
                return false;
        }
    }
    return true;
#elif PCB_PLATFORM_WINDOWS
    if(!CreateDirectory(path, NULL)) {
        DWORD err = GetLastError();
        if(err == ERROR_ALREADY_EXISTS) return true;
        PCB_logLatestError("Failed to create directory \"%s\"", path);
        return false;
    }
    return true;
#else
#error "Not implemented"
#endif //platform
}

typedef enum {
    //Unknown/unsupported file type.
    PCB_FILETYPE_UNKNOWN = 0x1,
    //Regular file.
    PCB_FILETYPE_REG = 0x2,
    //Directory.
    PCB_FILETYPE_DIR = 0x3,
    //Pipe, socket, etc. Further checks are platform-specific unfortunately.
    PCB_FILETYPE_STREAM = 0x4,
    //Character device, for example a console or some USB device.
    PCB_FILETYPE_CHAR = 0x5,
    //Block device, for example a hard drive.
    PCB_FILETYPE_BLK = 0x6,
    //Non-existent filesystem entry.
    PCB_FILETYPE_NONE = 0x10,
    //Symbolic link, always returned alongside another filetype.
    PCB_FILETYPE_SYMLINK = 0x20,

    //Unknown/unsupported file type that is pointed to via a symlink.
    PCB_FILETYPE_UNKNOWN_SYM = PCB_FILETYPE_UNKNOWN | PCB_FILETYPE_SYMLINK,
    //Regular file that is pointed to via a symlink.
    PCB_FILETYPE_REG_SYM = PCB_FILETYPE_REG | PCB_FILETYPE_SYMLINK,
    //Directory that is pointed to via a symlink.
    PCB_FILETYPE_DIR_SYM = PCB_FILETYPE_DIR | PCB_FILETYPE_SYMLINK,
    //Pipe, socket, etc. that is pointed to via a symlink.
    PCB_FILETYPE_STREAM_SYM = PCB_FILETYPE_STREAM | PCB_FILETYPE_SYMLINK,
    //Character device that is pointed to via a symlink.
    PCB_FILETYPE_CHAR_SYM = PCB_FILETYPE_CHAR | PCB_FILETYPE_SYMLINK,
    //Block device that is pointed to via a symlink.
    PCB_FILETYPE_BLK_SYM = PCB_FILETYPE_BLK | PCB_FILETYPE_SYMLINK,
    //Symlink that points to a non-existent filesystem entry.
    PCB_FILETYPE_NONE_SYM = PCB_FILETYPE_NONE | PCB_FILETYPE_SYMLINK,

    //An error occured while checking the type; to get the error code
    //call `PCB_GetError()`.
    PCB_FILETYPE_ERROR = 0,
    //A convenience value to strip away the symlink bit if one doesn't care.
    PCB_FILETYPE_SYMLINK_IGN = ~PCB_FILETYPE_SYMLINK
} PCB_FileType;

PCB_FileType PCB_FS_GetType(const char* path) {
    if(path == NULL) {
#if PCB_PLATFORM_WINDOWS
        SetLastError(0);
#endif
        errno = EFAULT; return PCB_FILETYPE_ERROR;
    }
#if PCB_PLATFORM_WINDOWS
    PCB_FileType type;
    HANDLE f = CreateFileA(
        path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );
    if(f == INVALID_HANDLE_VALUE) {
        errno = 0;
        type = GetLastError() == ERROR_FILE_NOT_FOUND ? PCB_FILETYPE_NONE : PCB_FILETYPE_ERROR;
        goto end;
    }
    switch(GetFileType(f)) {
        case FILE_TYPE_CHAR: type = PCB_FILETYPE_CHAR; break;
        case FILE_TYPE_PIPE: type = PCB_FILETYPE_STREAM; break;
        case FILE_TYPE_UNKNOWN:
            type = GetLastError() == NO_ERROR ? PCB_FILETYPE_UNKNOWN : PCB_FILETYPE_ERROR;
            break;
        case FILE_TYPE_DISK: {
            FILE_ATTRIBUTE_TAG_INFO tags;
            if(!GetFileInformationByHandleEx(
                f, FileAttributeTagInfo, &tags, sizeof(tags)
            )) {
                errno = 0; type = PCB_FILETYPE_ERROR; break;
            }
            if(tags.FileAttributes == INVALID_FILE_ATTRIBUTES) {
                errno = 0; type = PCB_FILETYPE_ERROR; break;
            }
            //TODO: Windows has something called "reparse points"
            //that are used for symlinks and "junctions". Implement logic for them.
            if(tags.FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                type = PCB_FILETYPE_DIR;
            else type = PCB_FILETYPE_REG;
        } break;
        default: type = PCB_FILETYPE_UNKNOWN; break;
    }
    end:
    CloseHandle(f);
    return type;
#elif PCB_PLATFORM_POSIX
    struct stat s;
    if(lstat(path, &s) == -1) {
        if(errno == ENOENT) {errno = 0; return PCB_FILETYPE_NONE; }
        else return PCB_FILETYPE_ERROR;
    }
    if(S_ISLNK(s.st_mode)) {
        if(stat(path, &s) == -1) {
            if(errno == ENOENT) {errno = 0; return PCB_FILETYPE_NONE_SYM; }
            else return PCB_FILETYPE_ERROR;
        }
        if(S_ISREG(s.st_mode)) return PCB_FILETYPE_REG_SYM;
        if(S_ISDIR(s.st_mode)) return PCB_FILETYPE_DIR_SYM;
        if(S_ISCHR(s.st_mode)) return PCB_FILETYPE_CHAR_SYM;
        if(S_ISBLK(s.st_mode)) return PCB_FILETYPE_BLK_SYM;
        if(S_ISFIFO(s.st_mode) || S_ISSOCK(s.st_mode)) return PCB_FILETYPE_STREAM_SYM;
        return PCB_FILETYPE_UNKNOWN_SYM;
    }
    if(S_ISREG(s.st_mode)) return PCB_FILETYPE_REG;
    if(S_ISDIR(s.st_mode)) return PCB_FILETYPE_DIR;
    if(S_ISCHR(s.st_mode)) return PCB_FILETYPE_CHAR;
    if(S_ISBLK(s.st_mode)) return PCB_FILETYPE_BLK;
    if(S_ISFIFO(s.st_mode) || S_ISSOCK(s.st_mode)) return PCB_FILETYPE_STREAM;
    return PCB_FILETYPE_UNKNOWN;
#endif //platform
}

uint64_t PCB_FS_GetModificationTime(const char* path) {
    errno = 0;
#if PCB_PLATFORM_WINDOWS
    HANDLE hFile = CreateFileA(
        path, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );
    if(hFile == INVALID_HANDLE_VALUE)
        return (uint64_t)(GetLastError() == ERROR_FILE_NOT_FOUND);

    BY_HANDLE_FILE_INFORMATION fileinfo = {0};
    BOOL b = GetFileInformationByHandle(hFile, &fileinfo);
    CloseHandle(hFile);
    if(!b) return 0;

    SetLastError(0);
    uint64_t modTime = fileinfo.ftLastWriteTime.dwLowDateTime;
    modTime += (uint64_t)(fileinfo.ftLastWriteTime.dwHighDateTime) << 32;
    return modTime;
#elif PCB_PLATFORM_POSIX
    struct stat fileinfo = {0};
    if(stat(path, &fileinfo) == -1) return (uint64_t)(errno == ENOENT);
    uint64_t modTime;
#ifdef __STRICT_ANSI__
    modTime = (uint64_t)fileinfo.st_ctime       * 1000000000; //forward compat
#else
    modTime = (uint64_t)fileinfo.st_ctim.tv_sec * 1000000000 + (uint64_t)fileinfo.st_ctim.tv_nsec;
#endif //pesky, but useful GNU extensions
    return modTime;
#endif //platform
}

int PCB_FS_Exists(const char* path) {
    PCB_FileType type = PCB_FS_GetType(path);
    if(type == PCB_FILETYPE_ERROR) return -1;
    return type != PCB_FILETYPE_NONE;
}

//Section 2.3: Strings, string views, vectors of strings...
//Section 2.3.1: A vector of const char*, a shell command
typedef struct {
    const char** data;
    size_t length;
    size_t capacity;
} PCB_CStrings;

typedef PCB_CStrings PCB_ShellCommand;

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
    size_t len = PCB_strlen(str);
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
    const size_t len = PCB_strlen(other);
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
    const size_t len = PCB_strlen(other);
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

PCB_String PCB_String_from_CStrings(const PCB_CStrings* cstr, const char* delimiter) {
    if(cstr->data == NULL || cstr->length == 0)
        return (PCB_String){0};
    size_t totalLength = 0;
    for(size_t i = 0; i < cstr->length; totalLength += PCB_strlen(cstr->data[i++]));
    //delimiter isn't placed at the end              v    '\0'
    totalLength += PCB_strlen(delimiter) * (cstr->length - 1) + 1;
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
    PCB_ShellCommand_append_arg(command, NULL);
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
#endif //platform-dependent return code
    }

    PCB_Process process = PCB_ShellCommand_runBg(command);
    if(!PCB_Process_isValid(process))
#if PCB_PLATFORM_WINDOWS
        return (ssize_t)0x8000000000000001;
#elif PCB_PLATFORM_POSIX
        return -2;
#endif //platform-dependent return code
    return PCB_Process_waitForExit(process);
}

//Section 3: PCB's build capability

#define PCB_BUILD_CAPABILITY //temporary for development

#ifdef PCB_BUILD_CAPABILITY
//Section 3.??? (not finalized)

typedef struct {
    //Path to the compiler executable to use.
    ////Defaults to the compiler's name used to build this file.
    const char* compilerPath;
    //Path to the build directory. Defaults to "build/".
    const char* buildPath;
    //Vector of paths to source directories.
    PCB_CStrings sources;
    //Vector of paths to include directories.
    PCB_CStrings includes;
    //Vector of names of libraries to link dynamically.
    PCB_CStrings libs;
    //Vector of names of libraries to link *statically*.    
    PCB_CStrings staticLibs;
    //Vector of additional paths to pass to the compiler to
    //search for specified libraries.
    PCB_CStrings librarySearchPaths;
    //Vector of compiler optimization flags. Not a singular const char*
    //since, for example in GCC, you can pass "-f" optimization flags
    //on top of "-O" flags.
    PCB_CStrings optimizationFlags;
    //Vector of debug flags. Put flags about sanitizers here.
    PCB_CStrings debugFlags;
    //Vector of warning flags, as well as warning-as-error flags.
    PCB_CStrings warningFlags;
    //Vector of other compiler flags not covered by the rest of this struct.
    PCB_CStrings otherFlags;
    //Internal buffer used for enumerating source paths.
    PCB_String currentSourcePath;
    //Internal buffer used for enumerating build paths w.r.t. the source path.
    PCB_String currentBuildPath;
    //Internal buffer used for commands when building. Do not use.
    PCB_ShellCommand commandBuffer;
    //The language standard used to compile source files.
    //Defaults to the standard used to build this file.
    long standard;
} PCB_BuildContext;





//Section 3.3: Functions used for building.


int PCB__build_file(PCB_BuildContext* context) {
    PCB_log(
        PCB_LOGLEVEL_INFO,
        "In: %s, out: %s",
        context->currentSourcePath.data, context->currentBuildPath.data
    );

    return 0;
}

//TODO: document return values
int PCB__build_directory(PCB_BuildContext* context) {
#if PCB_PLATFORM_WINDOWS
    (void)context;
    PCB_TODO("PCB__build_directory");
#elif PCB_PLATFORM_POSIX
#define PCB_err(err) { code = -err; goto error; }
    int code = 0;
    switch(PCB_FS_Exists(context->currentBuildPath.data)) {
        case true: break;
        case false: {
            if(!PCB_mkdir(context->currentBuildPath.data)) return -errno;
            break;
        }
        default:
            code = -errno;
            PCB_logLatestError(
                "Cannot check whether %s exists",
                context->currentBuildPath.data
            ); return code;
    }
    DIR* cwd = opendir(context->currentSourcePath.data);
    if(cwd == NULL) {
        code = -errno;
        PCB_logLatestError(
            "Could not open directory %s",
            context->currentSourcePath.data
        ); return code;
    }
//convenience macros
#define PCB_from (&context->currentSourcePath)
#define PCB_to (&context->currentBuildPath)
    struct stat st;
    for(struct dirent* entry = readdir(cwd); entry != NULL; entry = readdir(cwd)) {
        if(!PCB_strcmp(entry->d_name, ".") || !PCB_strcmp(entry->d_name, "..")) continue;
        size_t oldFromLength = PCB_from->length;
        size_t oldToLength = PCB_to->length;
        if(!PCB_String_append_cstr(PCB_from, entry->d_name)) PCB_err(ENOMEM)
        if(stat(PCB_from->data, &st) == -1) {
            switch(errno) {
                case ELOOP:  case ENAMETOOLONG:
                case ENOENT: case EOVERFLOW:
                    PCB_log(
                        PCB_LOGLEVEL_WARN,
                        "Skipping %s: %s",
                        PCB_from->data, strerror(errno)
                    ); continue;
                case ENOMEM:
                    PCB_log(PCB_LOGLEVEL_FATAL, "The system has ran out of memory.");
                    PCB_err(ENOMEM)
                default: PCB_Unreachable;
            }
        }
        if(S_ISDIR(st.st_mode)) {
            if(!PCB_String_append_cstr(PCB_to, entry->d_name)) PCB_err(ENOMEM)
            if(!PCB_String_append_chars(PCB_from, '/', 1)) PCB_err(ENOMEM)
            if(!PCB_String_append_chars(PCB_to, '/', 1)) PCB_err(ENOMEM)
            //recursively build the subdirectory
            if((code = PCB__build_directory(context)) != 0) goto error;
        }
        else if(S_ISREG(st.st_mode)) {
            if(PCB_String_endsWith_cstr(PCB_from, ".c")) {
                if(!PCB_String_append_cstr(PCB_to, entry->d_name)) PCB_err(ENOMEM)
                PCB_to->data[PCB_to->length - 1] = 'o';
                if((code = PCB__build_file(context)) != 0) goto error;
            }
        }
        //restore old length in a LIFO fashion
        PCB_from->data[PCB_from->length = oldFromLength] = '\0';
        PCB_to->data[PCB_to->length = oldToLength] = '\0';
        continue;
        error: {
            PCB_from->data[PCB_from->length = oldFromLength] = '\0';
            PCB_to->data[PCB_to->length = oldToLength] = '\0';
            closedir(cwd);
            return code;
        }
    }
#undef PCB_to
#undef PCB_from
#undef PCB_err
    closedir(cwd);

    return 0;
#endif //platform-dependent directory enumeration
}

typedef enum {
    PCB_BUILDOPTION_NONE = 0,
    //Emits debug symbols into object files.
    PCB_BUILDOPTION_DEBUG = 1,
    //Equivalent to -O3 or /O3. For more granularity modify
    //the build context manually.
    PCB_BUILDOPTION_OPTIMIZE = 1 << 1,
    //Turns ASan on if the target supports it.
    PCB_BUILDOPTION_ASAN = 1 << 2
    //TODO: other sanitizers + more options
} PCB_BuildOption;

/**
 * @brief Create a PCB_BuildContext struct.
 * 
 * @param flags PCB_BuildOptions OR'ed together
 * @return a zeroed out struct if `flags == 0` or on error. 
 * Otherwise returns a default-initialized build context based on flags passed.
 * 
 * See Appendix 1 for more details.
 */
PCB_BuildContext PCB_CreateBuildContext(int flags) {
    if(flags == 0) return (PCB_BuildContext) {0};
    PCB_BuildContext context = {
        .buildPath = "build/",
        .standard =
#ifdef __cplusplus
            __cplusplus,
#else
            __STDC_VERSION__,
#endif //C++
        .compilerPath =
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
    };
    PCB_Vec_append(&context.sources, "src/");
    PCB_Vec_append(&context.includes, "include/");
#if PCB_COMPILER_GCC //https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
    PCB_Vec_append_variadic(
        &context.warningFlags, const char*,
        "-Wall", "-Wextra", "-Wdouble-promotion",
        "-Werror=stringop-overflow", "-Wformat=2",
        "-Wnull-dereference", //-Winit-self
        "-Werror=use-after-free=2",
        "-Walloc-size", "-Walloc-zero",
        "-Werror=array-bounds=1",
        "-Wduplicated-branches", "-Wduplicated-cond",
        "-Wshadow", "-Wfree-nonheap-object",
        "-Wbad-function-cast", "-Wconversion"
    );
#elif PCB_COMPILER_CLANG //https://clang.llvm.org/docs/DiagnosticsReference.html
    PCB_Vec_append_variadic(
        &context.warningFlags, const char*,
        "-Wall", "-Wextra", "-Warray-bounds-pointer-arithmetic",
        "-Warray-parameter", "-Wassign-enum",
        "-Wbad-function-cast", "-Wbool-operation",
        "-Wcast-function-type", "-Wconversion",
        "-Wdouble-promotion", "-Wfloat-conversion",
        "-Widiomatic-parentheses", "-Winfinite-recursion",
        "-Wmismatched-tags", "-Wmissing-variable-declarations",
        "-Wshadow-all", "-Wsign-compare", "-Wshorten-64-to-32",
        "-Wsign-conversion", "-Wsometimes-uninitialized",
        "-Wthread-safety", "-Wunused",
        //-Wpessimizing-move -Wself-move <-- for C++
        //-Wsuggest-destructor-override -Wsuggest-override
        //-Wuninitialized-const-reference
    );
#elif PCB_COMPILER_MSVC
    PCB_Vec_append_variadic(
        &context.warningFlags, const char*,
        "/W4", "/w44062", "/w44388", "/w25219", "/w15247",
        "/w45263", "/w34191"
    );
#endif //compiler-specific warnings
    if(flags & PCB_BUILDOPTION_OPTIMIZE) {
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
        PCB_Vec_append(&context.optimizationFlags, "-O3");
#elif PCB_COMPILER_MSVC
        PCB_Vec_append(&context.optimizationFlags, "/O3");
#endif
    }
    if(flags & PCB_BUILDOPTION_OPTIMIZE) {
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
        PCB_Vec_append(&context.debugFlags, "-g");
#elif PCB_COMPILER_MSVC
        // PCB_Vec_append(&context.debugFlags, "/O3");
#error "Setting up debug flags for MSVC is a pain, will do it later"
#endif
    }

}

int PCB_buildFromContext(PCB_BuildContext* context) {
    PCB_assert(false && "Unfinished");
    
}

#endif //PCB_BUILD_CAPABILITY

#ifdef __cplusplus
}
#endif //C++

//Remove all locally defined, potentially conflicting macros

#ifdef PCB_BOOL_LOCALLY_DEFINED
#undef bool
#undef true
#undef false
#endif //PCB_BOOL_LOCALLY_DEFINED

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


//Appendix 2: Changelog
/**
 * Version 0.2.0:
 * - Removed PCB_* global variables and PCB_build function,
 * - Renamed PCB_build_* to PCB__build_*, PCB__ will mark functions not intended
 *   for public use
 * Version 0.1.13:
 * - Added PCBAPI macro for future use in dynamic linking of PCB, PCBCALL macro
 *   for declaring the calling convention, PCB_BUILD_DYN for setting appropiate
 *   things for symbol export,
 * - Added PCB_Printf_Format macro for printf-like functions (only works with GCC/Clang),
 * - Added PCB_Cleanup macro for use in C as an explicit C++ destructor; only
 *   available if PCB_WANT_CLEANUP is declared since it's not supported everywhere and
 *   its use fundamentally changes the code,
 * - Renamed PCB_CStringVec to PCB_CStrings,
 * - Removed PCB_ShellCommand_run_and_wait_old function,
 * - Added PCB_FileType enum & PCB_FS_GetType function,
 * - Added PCB_FS_GetModificationTime function,
 * - Added PCB_FS_Exists function,
 * Version 0.1.12:
 * - Removed PCB_roundUpToPowerOf2_32/64 macros, added PCB_TODO, PCB_ARRAY_LEN,
 *   PCB_SHIFT macros
 * Version 0.1.11:
 * - Added explicit notice that C89 is not supported.
 * Version 0.1.10:
 * - Added platform-agnostic way of error handling.
 * Version 0.1.9:
 * - Added `PCB_logDebug` and `PCB_logTrace` for debugging the library.
 * Version 0.1.8:
 * - Added `PCB_LOGLEVEL_*_NL` so that "\n" may be omitted in `PCB_log`,
 *   non-error log levels are directed to stdout instead of stderr.
 * Version 0.1.7:
 * - Fixed detection of MinGW, Clang, MSVC and C23 in `PCB_NoReturn`,
 *   added `PCB_NoReturn` for Clang.
 * Version 0.1.6:
 * - Added fallbacks for `bool`, `true`, `false`, `strcmp`, `strncmp` and `strlen`.
 * Version 0.1.5:
 * - Made `PCB_Vec_reserve` realloc-safe, i.e. it won't crash on insufficient memory.
 *   This is a double-edged sword, however; consider overriding it with feature macros.
 * Version 0.1.4:
 * - Changed `PCB_VERSION` to accomodate 1000 minor & patch versions instead of 10.
 * Version 0.1.3:
 * - Changed course on how to approach builds:
 *   Instead of a set of global variables and macros around them to add compiler flags,
 *   PCB will build from a "build context", which allows for multiple build types
 *   and more configurability.
 * Version 0.1.2:
 * - Fixed PCB_VERSION_* defines to set them to the actual version
 * - Fixed a missing #endif at "#ifndef PCB_PLATFORM_POSIX"
 * - Fixed the lack of NULL-termination in PCB_ShellCommand_runBg/POSIX
 * - Removed old PCB_Vec_append
 * - Added comments for some #endif's, added whitespace in some places for readability
 * Version 0.1.1:
 * - Added PCB_HAS_STRING_H macro for libc's string.h header detection
 * - Added PCB_memcpy, PCB_memmove, PCB_memset, PCB_memcmp macros
 *   which map to libc if available, otherwise they map to
 *   equivalent functions with the same name implemented within
 *   PCB itself; memcpy, memmove, memset, memcmp are therefore
 *   substituted with PCB_ versions
 * Version 0.1.0:
 * - Moved platform identification to section 1.1
 * - Added support for some Apple platforms
 * - Added identification of POSIX-compliant platforms
 * - Moved compiler identification to section 1.2
 * - Moved compiler-specific macros to section 1.3
 * - Changed "#error" statement start with "PCB Error"
 * - Added strings.h header for case-insensitive C-strings
 * - Changed Linux-specific implementations to POSIX-specific
 * - Added PCB_String_setSuffix_char function
 * - Added a missing #endif at "#ifdef PCB_BUILD_CAPABILITY"
 * - Reinforced PCB_build_directory for errors, now returns a status code
 * Version 0.0.1: Initial version
 */

#endif //PCB_H
