/**
 * Copyright (c) 2025 Piotr Mikolajewski
 *
 * This is PCB, a header-only general purpose C library
 * with build capabilities (well, not yet :>).
 *
 * PCB is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * PCB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with PCB. If not, see
 * <https://www.gnu.org/licenses/>.
 */
#ifndef PCB_H
#define PCB_H

#ifndef PCB_VERSION_MAJOR
#define PCB_VERSION_MAJOR 0
#endif //PCB_VERSION_MAJOR

#ifndef PCB_VERSION_MINOR
#define PCB_VERSION_MINOR 5
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

/* PCB uses the following naming convention:
 *
 * - Names starting with "data type" should have "_" as next character.
 * - Names with the same prefix should have "_" as next character.
 * - If a name block does not contain any "long" word
 * (~8+ characters long), camelCase is used; snake_case otherwise
 * - If names refer to something similar, their prefix should
 * also be similar and the differing part should be connected
 * with "_". Additionally, if there is an established
 * default, the differing part can be omitted for the default one.
 * - If the resulting name using camelCase would hurt
 * readability, convert parts/all of it to snake_case.
 * - Conversely, if the resulting name using snake_case would hurt
 * readability, convert parts/all of it to camelCase.
 * - If a name contains names of at least 2 data types,
 * favor snake_case.
 * - Compile-time constants should favor CAPITALIZED SNAKE_CASE.
 * - Non-trivial data types' first letter should be CAPITALIZED.
 * - Name parts that can be inferred by analyzing target's use
 * case should be removed.
 * - Prefer names with full words over partial words.
 * - Prefer short and descriptive names for non-static variables
 * and more descriptive names for static variables.
 * - A combination of "type name", contents of "type name"
 * (if struct/union) and "variable name" should be enough
 * to infer the reason of the variable's existence.
 * - Avoid single-letter variable names, unless used
 * in a mathematical context.
 * - If a name can be made shorter without sacrificing
 * readability, it should...with restraint.
 * - Lastly, this is a guideline, not a strict set of rules.
 * Exceptions will happen.
 *
 *
 *
 * For example:
 *
 * typedef struct {
 *  char* data;
 *  size_t length;
 *  size_t capacity;
 * } "dynamic string";
 * "dynamic string" is non-trivial -> "Dynamic_string"?
 * No, the part "Dynamic" can be inferred by its intended
 * use case and functions manipulating it -> "_string"?
 * No, the underscore hurts readability -> "string"?
 * No, it's non-trivial -> "String"?
 * Maybe, what about "Str"?
 * No, "Str" is not a full english word -> back to "String".
 * Settled.
 *
 * Let's say that we implement 2 following functions:
 *
 * "String starts with String" with arguments (String, String)
 * and
 * "String starts with C string" with arguments (String, C String)
 *
 * Both manipulate "String" -> both start with "String_".
 * Both share "starts with" as a prefix:
 *  "starts" and "with" and not long enough, combine with camelCase.
 *  Now both start with "String_startsWith".
 * Both do something similar, but on different arguments,
 * therefore current name + the rest of the name should be
 * connected with "_".
 *
 * We get:
 * "String_startsWith_String"
 * and
 * "String_startsWith_CString".
 * The first one will be the default -> "String_startsWith".
 * The second one could be shortened to "String_startsWith_cstr"
 *
 *
 *
 * All in all, I think this is a reasonable naming convention.
 * Certainly better than strictly following camelCase/snake_case/etc.
 * Certainly better than a lack of any convention.
 * And most certainly better than whatever the fuck
 * libstdc++ uses internally...trust me, if styling code
 * was a marathon, libstdc++ would sprint in circles
 * until it lit the asphalt on fire and opened
 * a nether portal.
 */



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
#define PCB_PLATFORM_WASM 0
#define PCB_PLATFORM "Windows"
#elif defined(__linux__)
#define PCB_PLATFORM_WINDOWS 0
#define PCB_PLATFORM_LINUX 1
#define PCB_PLATFORM_BSD 0
#define PCB_PLATFORM_MACOS 0
#define PCB_PLATFORM_IOS 0
#define PCB_PLATFORM_WASM 0
#define PCB_PLATFORM "Linux"
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IOS
#define PCB_PLATFORM_WINDOWS 0
#define PCB_PLATFORM_LINUX 0
#define PCB_PLATFORM_BSD 0
#define PCB_PLATFORM_IOS 1
#define PCB_PLATFORM_MACOS 0
#define PCB_PLATFORM_WASM 0
#define PCB_PLATFORM "iOS"
#elif TARGET_OS_MAC
#define PCB_PLATFORM_WINDOWS 0
#define PCB_PLATFORM_LINUX 0
#define PCB_PLATFORM_BSD 0
#define PCB_PLATFORM_IOS 0
#define PCB_PLATFORM_MACOS 1
#define PCB_PLATFORM_WASM 0
#define PCB_PLATFORM "Mac OS"
#else
#error PCB Error: Unsupported Apple platform
#endif //Apple platforms
#elif defined(__wasm__)
#define PCB_PLATFORM_WINDOWS 0
#define PCB_PLATFORM_LINUX 0
#define PCB_PLATFORM_BSD 0
#define PCB_PLATFORM_IOS 0
#define PCB_PLATFORM_MACOS 0
#define PCB_PLATFORM_WASM 1
#define PCB_PLATFORM "WebAssembly"
#error PCB Error: WebAssembly target is currently not supported
#else
#error PCB Error: Unsupported platform
#define PCB_PLATFORM "Unknown"
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
#if PCB_PLATFORM_LINUX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif //_GNU_SOURCE
#else
#if !defined(_XOPEN_SOURCE) && !defined(_POSIX_C_SOURCE)
#if defined(__GLIBC__) && __GLIBC__+0 >= 2 && __GLIBC_MINOR__+0 < 10
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 700
#endif //glibc 2.10
#endif //only #define if no feature test macro is #defined
#endif //Use _GNU_SOURCE on Linux
#endif //POSIX sources used locally

//Section 1.2: Identify the compiler used to compile this code

#ifndef PCB_COMPILER
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
//https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
#define PCB_COMPILER_GCC (__GNUC__*100*100 + __GNUC_MINOR__*100 + __GNUC_PATCHLEVEL__)
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
//same schema as GCC
#define PCB_COMPILER_CLANG (__clang_major__*100*100 + __clang_minor__*100 + __clang_patchlevel__)
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
//MSVC uses a different versioning scheme, https://learn.microsoft.com/en-us/cpp/overview/compiler-versions
#define PCB_COMPILER_MSVC (_MSC_VER)
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
#if (PCB_COMPILER_GCC >= 30101) || PCB_COMPILER_CLANG
#define PCB_ForceInline inline __attribute__((always_inline))
#elif PCB_COMPILER_MSVC
#define PCB_ForceInline __forceinline
#else
#define PCB_ForceInline inline
#endif //Compilers
#endif //PCB_ForceInline

#ifndef PCB_restrict
#if defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 199901L
#define PCB_restrict restrict
#elif PCB_COMPILER_GCC || PCB_COMPILER_CLANG || PCB_COMPILER_MSVC
#define PCB_restrict __restrict
#else
#define PCB_restrict
#endif //why tf is there no "restrict" keyword in C++?!
#endif //PCB_restrict

#ifndef PCB_BeforeMain
#ifdef __cplusplus
//Using C++'s constructor trickery we can construct
//an empty object with a static lifetime, which means
//running a function at startup.
#define PCB_BeforeMain(f) static void f(void); \
struct f##__ { f##__() { f(); } }; static f##__ f##_; \
static void f(void)
#else //C
#if PCB_COMPILER_MSVC
//https://stackoverflow.com/questions/1113409/attribute-constructor-equivalent-in-vc
//https://github.com/nodejs/node/issues/41852
#define PCB_INITIALIZER_(f,p) PCB_DO_PRAGMA(section(".CRT$XCU",read)) \
static void f(void); \
__declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
__pragma(comment(linker,"/include:" p #f "_")) \
static void f(void)
#ifdef _WIN64
#define PCB_BeforeMain(f) PCB_INITIALIZER_(f,"")
#else
#define PCB_BeforeMain(f) PCB_INITIALIZER_(f,"_")
#endif
#elif PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB_BeforeMain(f) static __attribute__((constructor)) void f(void)
#else
#define PCB_BeforeMain(f) \
_Pragma("PCB Warning: function '" #f "' will not run before main because the compiler used does not support it") \
static void f(void)
#endif //Compilers
#endif //C++?
#endif //PCB_BeforeMain

#ifndef PCB_Unreachable
#if PCB_COMPILER_GCC >= 40500 || PCB_COMPILER_CLANG >= 30400
#define PCB_Unreachable __builtin_unreachable()
#elif PCB_COMPILER_MSVC
#define PCB_Unreachable (__assume(false))
#else
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#warning "PCB Warning: PCB_Unreachable does not mark unreachability"
#elif PCB_COMPILER_MSVC
#pragma message "PCB Warning: PCB_Unreachable does not mark unreachability"
#endif //compilers
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

//Get type of expression.
//Portable applications must check whether `PCB_Typeof` is #defined before use.
//In C++11+, `PCB_Typeof` expands to `decltype`
//NOTE: Be *very* careful when using in external-facing declarations. This WILL cause problems!
#ifndef PCB_Typeof
#if defined(__cplusplus) && defined(__cpp_decltype) && __cpp_decltype+0 >= 200707L
#define PCB_Typeof(expr) decltype(expr)
#elif defined(__STDC_VERSION__)
#if PCB_COMPILER_GCC
#if PCB_COMPILER_GCC >= 130000 && __STDC_VERSION__ >= 202311L
#define PCB_Typeof(expr) typeof(expr)
#else
#define PCB_Typeof(expr) __typeof__(expr)
#endif //GCC 13+ && C23
#elif PCB_COMPILER_CLANG
#if PCB_COMPILER_CLANG >= 160000 && __STDC_VERSION__ >= 202311L
#define PCB_Typeof(expr) typeof(expr)
#else
#define PCB_Typeof(expr) __typeof__(expr)
#endif //Clang 16+ && C23
#elif PCB_COMPILER_MSVC
#if PCB_COMPILER_MSVC >= 1939 && __STDC_VERSION__ >= 202311L
#define PCB_Typeof(expr) typeof(expr)
#endif //VS 2022 17.9 && C23
#endif //compilers
#endif //C++ && __cpp_decltype || C
#endif //PCB_Typeof

#ifndef PCB_HAS_INCLUDE
#if (PCB_COMPILER_GCC >= 50000) || \
    (PCB_COMPILER_CLANG >= 30100) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 202311L) || \
    (defined(__cplusplus) && __cplusplus+0 >= 201703L)
#define PCB_HAS_INCLUDE __has_include
#else
#define PCB_HAS_INCLUDE 1 //assume that #include is available
#endif //whether __has_include is available
#endif //PCB_HAS_INCLUDE



//Section 1.4: Import libc, unless this macro is defined as 0
#ifndef PCB_USE_LIBC
#define PCB_USE_LIBC 1
#endif //PCB_USE_LIBC

//TODO: only #include things that are actually required
#if defined(PCB_USE_LIBC) && PCB_USE_LIBC+0
//for "_s" functions
#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif //__STDC_WANT_LIB_EXT1__

#ifndef PCB_HAS_STDIO_H
#if PCB_HAS_INCLUDE(<stdio.h>)
#include <stdio.h>
#define PCB_HAS_STDIO_H
#endif //has stdio.h
#endif //PCB_HAS_STDIO_H

#ifndef PCB_HAS_STDLIB_H
#if PCB_HAS_INCLUDE(<stdlib.h>)
#include <stdlib.h>
#define PCB_HAS_STDLIB_H
#endif //has stdlib.h
#endif //PCB_HAS_STDLIB_H

#ifndef PCB_HAS_ASSERT_H
#if PCB_HAS_INCLUDE(<assert.h>)
#include <assert.h>
#define PCB_HAS_ASSERT_H
#endif //has assert.h
#endif //PCB_HAS_ASSERT_H

#ifndef PCB_HAS_STRING_H
#if PCB_HAS_INCLUDE(<string.h>)
#include <string.h>
#define PCB__HAS_STRING_H
#endif //has string.h
#if PCB_HAS_INCLUDE(<strings.h>)
#define PCB__HAS_STRINGS_H
#include <strings.h>
#endif //has strings.h
#if PCB_HAS_INCLUDE(<wchar.h>)
#include <wchar.h>
#define PCB__HAS_WCHAR_H
#endif //has wchar.h

#if defined(PCB__HAS_STRING_H)  && \
    defined(PCB__HAS_STRINGS_H) && \
    defined(PCB__HAS_STRING_H)
#define PCB_HAS_STRING_H
#endif

#endif //PCB_HAS_STRING_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
//A useful command to list errno info: errno -l | sort -k2 -n
#if PCB_HAS_INCLUDE(<errno.h>)
#include <errno.h>
#define PCB_HAS_ERRNO_H
#else
#ifndef errno
PCB_DeprecatedReason("errno is unavailable, this is a stub.") extern int errno_stub;
#define errno errno_stub
#endif //errno stub
#endif //has errno.h
#if PCB_HAS_INCLUDE(<inttypes.h>)
#include <inttypes.h>
#define PCB_HAS_INTTYPES_H
#endif //has inttypes.h
#if PCB_HAS_INCLUDE(<ctype.h>)
#include <ctype.h>
#define PCB_HAS_CTYPE_H
#endif //has ctype.h
#if PCB_HAS_INCLUDE(<time.h>)
#include <time.h>
#define PCB_HAS_TIME_H
#endif //has time.h
#else
//fallback for no booleans
#if !defined(__cplusplus) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__+0 < 202311L && !defined(bool)
#ifndef PCB_BOOL_LOCALLY_DEFINED
#define PCB_BOOL_LOCALLY_DEFINED
#define bool _Bool
#ifndef true
#define true 1
#endif //true
#ifndef false
#define false 0
#endif //false
#endif //PCB_BOOL_LOCALLY_DEFINED
#endif //bool

#endif //PCB_USE_LIBC?

//Section 1.5: Define functions/macros that the library uses from libc.
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
#endif //PCB_HAS_STRING_H
#endif //PCB_memcpy

#ifndef PCB_memmove
#ifdef PCB_HAS_STRING_H
#define PCB_memmove memmove
#endif //PCB_HAS_STRING_H
#endif //PCB_memmove

#ifndef PCB_memset
#ifdef PCB_HAS_STRING_H
#define PCB_memset memset
#endif //PCB_HAS_STRING_H
#endif //PCB_memset

#ifndef PCB_memcmp
#ifdef PCB_HAS_STRING_H
#define PCB_memcmp memcmp
#endif //PCB_HAS_STRING_H
#endif //PCB_memcmp

#ifndef PCB_strcmp
#ifdef PCB_HAS_STRING_H
#define PCB_strcmp strcmp
#endif //PCB_HAS_STRING_H
#endif //PCB_strcmp

#ifndef PCB_strncmp
#ifdef PCB_HAS_STRING_H
#define PCB_strncmp strncmp
#endif //PCB_HAS_STRING_H
#endif //PCB_strncmp

#ifndef PCB_strncasecmp
#ifdef PCB__HAS_STRINGS_H
#define PCB_strncasecmp strncasecmp
#endif //PCB__HAS_STRINGS_H
#endif //PCB_strncasecmp

#ifndef PCB_strlen
#ifdef PCB_HAS_STRING_H
#define PCB_strlen strlen
#endif //PCB_HAS_STRING_H
#endif //PCB_strlen

#ifndef PCB_strnlen
#if defined(PCB_HAS_STRING_H)
#ifdef __GLIBC__
#if __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 10
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE+0 >= 200809L
#define PCB_strnlen strnlen
#endif //POSIX source of strnlen
#else
#if defined(_GNU_SOURCE)
#define PCB_strnlen strnlen
#endif //GNU source of strnlen
#endif //glibc 2.10 checks
#endif //glibc
#endif //PCB_HAS_STRING_H
#endif //PCB_strnlen
//TODO: strpbrk

#ifndef PCB_isspace
#ifdef PCB_HAS_CTYPE_H
#define PCB_isspace isspace
#endif //PCB_HAS_CTYPE_H
#endif //PCB_isspace

#ifdef PCB_HAS_STDIO_H
//MSVC, again being an outlier, doesn't natively support positional argument
//syntax. Therefore we need to #define macros specifically for it...smh.
//https://learn.microsoft.com/en-us/cpp/c-runtime-library/printf-p-positional-parameters
//PCB_printf is not used by the library, but is provided anyway since
//`printf` is the most widely used function from the printf family.
#ifndef PCB_printf
#if PCB_COMPILER_MSVC
#define PCB_printf _printf_p
#else
#define PCB_printf printf
#endif //MSVC scheiße
#endif //PCB_printf

#ifndef PCB_fprintf
#if PCB_COMPILER_MSVC
#define PCB_fprintf _fprintf_p
#else
#define PCB_fprintf fprintf
#endif //MSVC scheiße
#endif //PCB_fprintf

#ifndef PCB_vfprintf
#if PCB_COMPILER_MSVC
#define PCB_vfprintf _vfprintf_p
#else
#define PCB_vfprintf vfprintf
#endif //MSVC scheiße
#endif //PCB_vfprintf

#ifndef PCB_snprintf
#if PCB_COMPILER_MSVC
//This one is weird. It's named as if it was sprintf, but takes in buffer size...
//TODO: verify that this function *actually* behaves like snprintf
#define PCB_snprintf _sprintf_p
#else
#define PCB_snprintf snprintf
#endif //MSVC scheiße
#endif //PCB_snprintf

#ifndef PCB_vsnprintf
#if PCB_COMPILER_MSVC
//Another weird one.
#define PCB_vsnprintf _vsprintf_p
#else
#define PCB_vsnprintf vsnprintf
#endif //MSVC scheiße
#endif //PCB_vsnprintf

//wchar_t variants
#ifndef PCB_wprintf
#if PCB_COMPILER_MSVC
#define PCB_wprintf _wprintf_p
#else
#define PCB_wprintf wprintf
#endif //MSVC scheiße
#endif //PCB_wprintf

#ifndef PCB_fwprintf
#if PCB_COMPILER_MSVC
#define PCB_fwprintf _fwprintf_p
#else
#define PCB_fwprintf fwprintf
#endif //MSVC scheiße
#endif //PCB_fwprintf

#ifndef PCB_vfwprintf
#if PCB_COMPILER_MSVC
#define PCB_vfwprintf _vfwprintf_p
#else
#define PCB_vfwprintf vfwprintf
#endif //MSVC scheiße
#endif //PCB_vfwprintf

//This one is even weirder than `snprintf`. There is no `snwprintf`/`swprintf`
//pair, only `swprintf` that functions like `snprintf`.
#ifndef PCB_swprintf
#if PCB_COMPILER_MSVC
#define PCB_swprintf _swprintf_p
#else
#define PCB_swprintf swprintf
#endif //MSVC scheiße
#endif //PCB_swprintf

#ifndef PCB_vswprintf
#if PCB_COMPILER_MSVC
//Another weird one.
#define PCB_vswprintf _vswprintf_p
#else
#define PCB_vswprintf vswprintf
#endif //MSVC scheiße
#endif //PCB_vswprintf

#ifndef PCB_fflush
#define PCB_fflush fflush
#endif //PCB_fflush

#ifndef PCB_stdout
#define PCB_stdout stdout
#endif //PCB_stdout

#ifndef PCB_stderr
#define PCB_stderr stderr
#endif //PCB_stderr


#endif //PCB_HAS_STDIO_H

//TODO: somehow provide assertions in environments without libc
#ifndef PCB_assert
#ifdef PCB_HAS_ASSERT_H
#define PCB_assert(expr) assert(expr)
#define PCB__ASSERT_HANDLED
#else
#define PCB_assert(expr) ((expr) ? (void)0 : PCB__assert_fail(#expr, __FILE__, __LINE__, __func__))
#endif //sources of assert
#else
#define PCB__ASSERT_HANDLED //assume available
#endif //PCB_assert

//Section 1.6: Define other useful macros
//Section 1.6.1: General purpose macros
#ifndef PCB_TODO
#define PCB_TODO(msg) PCB_assert(0 && msg " not yet implemented")
#endif //PCB_TODO

#ifndef PCB_ARRAY_LEN
//Get `arr`'s length. `arr` must be an array, otherwise the result is incorrect.
#define PCB_ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif //PCB_ARRAY_LEN

#ifndef PCB_SHIFT_UNCHECKED
//Bash-like shifting of `args` counted by `count`. If `count == 0`,
//the behavior is undefined.
#define PCB_SHIFT_UNCHECKED(count, args) (--(count), *(args)++)
#endif //PCB_SHIFT_UNCHECKED

#ifndef PCB_SHIFT
#ifdef PCB_DISABLE_ASSERT
#define PCB_SHIFT(count, args) PCB_SHIFT_UNCHECKED(count, args)
#else
//Bash-like shifting of `args` counted by `count`. Asserts that `count > 0`.
#define PCB_SHIFT(count, args) (PCB_assert((count) > 0), --(count), *(args)++)
#endif //PCB_DISABLE_ASSERT
#endif //PCB_SHIFT

#ifndef PCB__STRINGIFY
//Turn `x` preprocessor token into a string literal (helper).
#define PCB__STRINGIFY(x) #x
#endif //PCB__STRINGIFY

#ifndef PCB_STRINGIFY
//Turn `x` preprocessor token into a string literal.
#define PCB_STRINGIFY(x) PCB__STRINGIFY(x)
#endif //PCB_STRINGIFY

#ifndef PCB__CONCAT
//Concatenate `x` & `y` preprocessor tokens (helper).
#define PCB__CONCAT(x, y) x##y
#endif //PCB__CONCAT

#ifndef PCB_CONCAT
//Concatenate `x` & `y` preprocessor tokens.
#define PCB_CONCAT(x, y) PCB__CONCAT(x, y)
#endif //PCB_CONCAT

//Macro controlling certain safety checks within the library.
#ifndef PCB_SAFETY_CHECKS
#define PCB_SAFETY_CHECKS 1
#endif //PCB_SAFETY_CHECKS

//Macro used for state validation within the library.
//You may find this useful in your code, hence it's public.
#ifndef PCB_CHECK
#if PCB_SAFETY_CHECKS < 0
//If a safety check fails, abort.
#define PCB_CHECK(cond, retValOnTrue) PCB_assert(!(cond))
#elif PCB_SAFETY_CHECKS <= 2
//Fail gracefully (default).
#define PCB_CHECK(cond, retValOnTrue) if(cond) return retValOnTrue
#elif PCB_SAFETY_CHECKS > 2
//Disable safety checks. Improves performance, but risks undefined behavior.
#define PCB_CHECK(cond, retValOnTrue)
#endif //different levels of `PCB_SAFETY_CHECKS`
#endif //PCB_CHECK

//Macro used in "methods" (in a C++ sense) to validate that the instance passed
//is non-NULL. Disabled by default.
#ifndef PCB_CHECK_SELF
#if PCB_SAFETY_CHECKS <= 0
//Verify the instance argument. Action taken depends on `PCB_SAFETY_CHECKS`.
#define PCB_CHECK_SELF(self, retValOnTrue) PCB_CHECK((self) == NULL, retValOnTrue)
#else
//Do not verify the instance argument (default).
#define PCB_CHECK_SELF(self, retValOnTrue)
#endif //different levels of `PCB_SAFETY_CHECKS`
#endif //PCB_CHECK_SELF

//Macro used for pointer variables that should not point to the same memory,
//but it might not be the case for safety reasons.
#ifndef PCB_maybe_restrict
#if PCB_SAFETY_CHECKS > 1
//Assume the underlying thing is not aliased by any other pointer.
//Improves performance, but risks undefined behavior.
#define PCB_maybe_restrict PCB_restrict
#else
//Assume the underlying thing may be aliased by another pointer (default).
#define PCB_maybe_restrict
#endif //PCB_SAFETY_CHECKS > 1
#endif //PCB_maybe_restrict

#ifndef PCB_maybe_inline
#ifdef PCB_NO_INLINE_EXPORTS
#define PCB_maybe_inline PCBAPI
#else
#define PCB_maybe_inline static PCB_ForceInline
#endif //PCB_NO_INLINE_EXPORTS
#endif //PCB_maybe_inline

//Section 1.6.2: template<*> struct vector in C let's goooo

#ifndef PCB_VEC_INITIAL_CAPACITY
#define PCB_VEC_INITIAL_CAPACITY 64
#endif //PCB_VEC_INITIAL_CAPACITY

#ifndef PCB_Vec_reserve
#ifdef __cplusplus
#define PCB_Vec_reserve(vec, howMany) do {                  \
    size_t new__capacity__ = (vec)->capacity;               \
    if(new__capacity__ == 0)                                \
        new__capacity__ = PCB_VEC_INITIAL_CAPACITY;         \
    while((vec)->capacity + (howMany) > new__capacity__) {  \
        new__capacity__ *= 2;                               \
    } if(new__capacity__ == (vec)->capacity) break;         \
    void* new__data__ = (void*)PCB_realloc(                 \
        (vec)->data, new__capacity__ * sizeof(*(vec)->data) \
    ); if(new__data__ == NULL) break;                       \
    PCB_memcpy(                                             \
        &(vec)->data, &new__data__, sizeof(new__data__)     \
    ); (vec)->capacity = new__capacity__;                   \
} while(0)
#else
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
    } if(new__capacity__ == (vec)->capacity) break;         \
    void* new__data__ = (void*)PCB_realloc(                 \
        (vec)->data, new__capacity__ * sizeof(*(vec)->data) \
    ); if(new__data__ == NULL) break;                       \
    (vec)->data = new__data__;                              \
    (vec)->capacity = new__capacity__;                      \
} while(0)
#endif //C++
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

#ifndef PCB_Vec_pop_unchecked
/**
 * @brief Pops the last element from `vec`.
 * If `vec->length == 0`, the behavior is undefined.
 */
#define PCB_Vec_pop_unchecked(vec) ((vec)->data[--(vec)->length])
#endif //PCB_Vec_pop_unchecked

#ifndef PCB_Vec_pop
#ifdef PCB_DISABLE_ASSERT
#define PCB_Vec_pop(vec) PCB_Vec_pop_unchecked(vec)
#else
/**
 * @brief Pops the last element from `vec`.
 */
#define PCB_Vec_pop(vec) \
    (PCB_assert((vec)->length > 0), (vec)->data[--(vec)->length])
#endif //PCB_DISABLE_ASSERT
#endif //PCB_Vec_pop

#ifndef PCB_Vec_last_unchecked
/**
 * @brief Returns a pointer to the last element of `vec`.
 * If `vec->length == 0`, the behavior is undefined.
 */
#define PCB_Vec_last_unchecked(vec) (&(vec)->data[(vec)->length - 1])
#endif //PCB_Vec_last_unchecked

#ifndef PCB_Vec_last
#ifdef PCB_DISABLE_ASSERT
#define PCB_Vec_last(vec) PCB_Vec_last_unchecked(vec)
#else
/**
 * @brief Returns a pointer to the last element of `vec`.
 */
#define PCB_Vec_last(vec) \
    (PCB_assert((vec)->length > 0), &(vec)->data[(vec)->length - 1])
#endif //PCB_DISABLE_ASSERT
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

#ifndef PCB_Vec_isEmpty
#define PCB_Vec_isEmpty(vec) ((vec)->data == NULL || (vec)->length == 0)
#endif //PCB_Vec_isEmpty

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

#ifndef PCB_Vec_forEach_it
#ifdef PCB_Typeof
#define PCB_Vec_forEach_it(vec, itName, ...)        \
for(                                                \
    PCB_Typeof((vec)->data) itName = (vec)->data;   \
    itName != (vec)->data + (vec)->length; itName++ \
)
#else
/**
 * @brief Traditional for-each with an iterator.
 * Adding elements is not allowed as it may invalidate the iterator.
 *
 * An example usage is as follows:
 * ```c
 * Vec_int v_new = {0};
 * ...
 * PCB_Vec_forEach_it(&v, it, int) {
 *  (*it) += 69;
 * }
 * ...
 * ```
 */
#define PCB_Vec_forEach_it(vec, itName, underlyingType) \
for(                                                    \
    underlyingType *itName = (vec)->data;               \
    itName != (vec)->data + (vec)->length; itName++     \
)
#endif //PCB_Typeof?
#endif //PCB_Vec_forEach_it

#ifndef PCB_Vec_enumerate
#ifdef PCB_Typeof
#define PCB_Vec_enumerate(vec, i, it, enumPair, ...)                                \
for(                                                                                \
    struct { size_t i; PCB_Typeof((vec)->data) it; } enumPair = { 0, (vec)->data }; \
    enumPair.i < (vec)->length; enumPair.i++, enumPair.it++                         \
)
#else
/**
 * @brief Enumerate `vec` with index `i` and pointer-to-element `it`.
 * Due to limitations of C, `i` and `it` have to be wrapped inside a struct
 * named `enumPair`.
 * Adding elements is not allowed as it may invalidate the iterator.
 *
 * An example usage is as follows
 * ```c
 * typedef struct {
 *     const char* const* data;
 *     size_t length;
 *     size_t capacity;
 * } CStrings;
 * ...
 * CStrings cstrs = {0};
 * ...
 * PCB_Vec_enumerate(&cstrs, i, it, iter, const char*) {
 *     printf("%4lu | %s\n", iter.i, iter.it);
 * }
 * ```
 */
#define PCB_Vec_enumerate(vec, i, it, enumPair, type)               \
for(                                                                \
    struct { size_t i; type *it; } enumPair = { 0, (vec)->data };   \
    enumPair.i < (vec)->length; enumPair.i++, enumPair.it++         \
)
#endif //PCB_Typeof?
#endif //PCB_Vec_enumerate

//Section 1.6.3: Macros for C++ compatibility
#ifndef PCB_ZEROED
#if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 202311L)
#define PCB_ZEROED {}
#else
#define PCB_ZEROED {0}
#endif //C++ || >=C23
#endif //PCB_ZEROED

#ifndef PCB_ZEROED_T
#ifdef __cplusplus
#define PCB_ZEROED_T(T) T{}
#else
#define PCB_ZEROED_T(T) (T)PCB_ZEROED
#endif //C++?
#endif //PCB_ZEROED_T

#ifndef PCB_CLITERAL
#ifdef __cplusplus
#define PCB_CLITERAL(Type) Type
#else
#define PCB_CLITERAL(Type) (Type)
#endif //C++
#endif //PCB_CLITERAL


//Section 1.6.4: Macros for views, slices
#ifndef PCB_View_Vec_unchecked
/**
 * @brief Constructs a view on vector `vec` in the range of
 * [`start`, `end`). If `vec->data == NULL` or `start > end` or
 * `end > vec->length`, the behavior is undefined.
 */
#define PCB_View_Vec_unchecked(vec, start, end) \
    { (vec)->data + (start), (end) - (start) }
#endif //PCB_View_Vec_unchecked

#ifndef PCB_View_Arr_unchecked
/**
 * @brief Constructs a view on array (!) `arr` in the range of [`start`, `end`).
 * Used similarly to `PCB_View_Vec_unchecked`, except it's only valid for arrays,
 * like `int arr[16];`.
 * If `start > end`, the behavior is undefined.
 */
#define PCB_View_Arr_unchecked(arr, start, end) \
    { &(arr)[start], (end) - (start) }
#endif //PCB_View_Arr_unchecked

#ifndef PCB_View_Ptr_unchecked
/**
 * @brief Constructs a view on a pointer `ptr` in the range of [`start`, `end`).
 * If `ptr == NULL || start > end` or `end` goes out of bounds,
 * the behavior is undefined.
 */
#define PCB_View_Ptr_unchecked(ptr, start, end) \
    { (ptr) + (start), (end) - (start) }
#endif //PCB_View_Ptr_unchecked

#ifndef PCB_View_Vec
#ifdef PCB_DISABLE_ASSERT
#define PCB_View_Vec(vec, start, end) PCB_View_Vec_unchecked(vec, start, end)
#else
/**
 * @brief Constructs a view on vector `vec` in the range of
 * [`start`, `end`).
 *
 * Example:
 * ```c
 * typedef struct { int* data; size_t length; size_t capacity; } Ints;
 * typedef struct { const int* data; size_t length; } Ints_view;
 * ...
 * Ints my_cool_integers = {0};
 * ...
 * Ints_view look_at_my_integers = PCB_View_Vec(&my_cool_integers, 6, 9);
 * ```
 */
#define PCB_View_Vec(vec, start, end)  {                                \
    ((end) - (start) != 0 ? PCB_assert((vec)->data != NULL) : (void)0,  \
     (vec)->data + (start)),                                            \
    (PCB_assert((ssize_t)(start) <= (ssize_t)(end)),                    \
     PCB_assert((ssize_t)(end)   <= (ssize_t)(vec)->length),            \
     (end) - (start))                                                   \
}
//For those wondering about casts to ssize_t: it's to suppress warnings about
//"unsigned comparison with 0 is always <true/false>" when passing literals
//to `start`/`end`. There is no other way around it.
#endif //PCB_DISABLE_ASSERT
#endif //PCB_View_Vec

#ifndef PCB_View_Arr
#ifdef PCB_DISABLE_ASSERT
#define PCB_View_Arr(arr, start, end) PCB_View_Arr_unchecked(arr, start, end)
#else
/**
 * @brief Constructs a view on array (!) `arr` in the range of [`start`, `end`).
 * Used similarly to `PCB_View_Vec`, except it's only valid for arrays,
 * like `int arr[16];`.
 */
#define PCB_View_Arr(arr, start, end) {                     \
     &(arr)[start],                                         \
    (PCB_assert((ssize_t)(start) <= (ssize_t)(end)),        \
     PCB_assert((ssize_t)(end)   <= PCB_ARRAY_LEN(arr)),    \
     (end) - (start))                                       \
}
#endif //PCB_DISABLE_ASSERT
#endif //PCB_View_Arr

#ifndef PCB_View_Ptr
#ifdef PCB_DISABLE_ASSERT
#define PCB_View_Ptr(ptr, start, end) PCB_View_Ptr_unchecked(ptr, start, end)
#else
/**
 * @brief Constructs a view on pointer `ptr` in the range of [`start`, `end`).
 * If `end` goes out of bounds, the behavior is undefined.
 */
#define PCB_View_Ptr(ptr, start, end) {                         \
    ((end) - (start) != 0 ? PCB_assert((ptr) != NULL) : (void)0,\
     (ptr) + (start)),                                          \
    (PCB_assert((ssize_t)(start) <= (ssize_t)(end)),            \
     (end) - (start))                                           \
}
#endif //PCB_DISABLE_ASSERT
#endif //PCB_View_Ptr


#ifndef PCB_View_Vec_T
/**
 * @brief Constructs a view of type `viewType` on vector `vec` in the range of
 * [`start`, `end`).
 * Primarily meant to be used when passing views by value to functions.
 * Example:
 * ```c
 * typedef struct { int* data; size_t length; size_t capacity; } Ints;
 * typedef struct { const int* data; size_t length; } Ints_view;
 * void print_ints(Ints_view ints) {
 *     for(size_t i = 0; i < ints.length; i++) {
 *         printf("%d ", ints.data[i]);
 *     }
 *     printf("\n");
 * }
 * ...
 * Ints my_cool_integers = {0};
 * ...
 * print_ints(PCB_View_Vec_T(&my_cool_integers, 6, 9, Ints_view));
 * ```
 */
#define PCB_View_Vec_T(vec, start, end, viewType) \
    (PCB_CLITERAL(viewType) PCB_View_Vec(vec, start, end))
#endif //PCB_View_Vec_T

#ifndef PCB_View_Arr_T
/**
 * @brief Constructs a view of type `viewType` on array (!) `arr` in the range of
 * [`start`, `end`).
 * Used similarly to `PCB_View_Vec_T`, except it's only valid for arrays,
 * like `int arr[16];`.
 */
#define PCB_View_Arr_T(arr, start, end, viewType) \
    (PCB_CLITERAL(viewType) PCB_View_Arr(arr, start, end))
#endif //PCB_View_Arr_T

#ifndef PCB_View_Ptr_T
/**
 * @brief Constructs a view of type `viewType` on pointer `ptr` in the range of
 * [`start`, `end`).
 * If `end` goes out of bounds, the behavior is undefined.
 */
#define PCB_View_Ptr_T(ptr, start, end, viewType) \
    (PCB_CLITERAL(viewType) PCB_View_Ptr(ptr, start, end))
#endif //PCB_View_Ptr_T



#ifndef PCB_View_Vec_A
/**
 * @brief Constructs a view on the entire vector `vec`.
 */
#define PCB_View_Vec_A(vec) PCB_View_Vec(vec, 0, (vec)->length)
#endif //PCB_View_Vec_A

#ifndef PCB_View_Arr_A
/**
 * @brief Constructs a view on the entire array (!) `arr`.
 * Used similarly to `PCB_View_Vec_A`, except it's only valid for arrays,
 * like `int arr[16];`.
 */
#define PCB_View_Arr_A(arr) PCB_View_Arr(arr, 0, PCB_ARRAY_LEN(arr))
#endif //PCB_View_Arr_A

#ifndef PCB_View_Ptr_A
/**
 * @brief Constructs a view on pointer `ptr` in the range of [0, `end`).
 * This differs from both `PCB_View_Vec_A` and `PCB_View_Arr_A` where `end` is
 * known. In the case of pointers, you are the one that provides the upper bound.
 *
 * If `end` goes out of bounds, the behavior is undefined.
 */
#define PCB_View_Ptr_A(ptr, end) PCB_View_Ptr(ptr, 0, end)
#endif //PCB_View_Ptr_A


#ifndef PCB_View_Vec_A_T
/**
 * @brief Constructs a view of type `viewType` on the entire vector `vec`.
 */
#define PCB_View_Vec_A_T(vec, viewType) \
    PCB_View_Vec_T(vec, 0, (vec)->length, viewType)
#endif //PCB_View_Vec_A_T

#ifndef PCB_View_Arr_A_T
/**
 * @brief Constructs a view of type `viewType` on the entire array (!) `arr`.
 * Used similarly to `PCB_View_Vec_A_T`, except it's only valid for arrays,
 * like `int arr[16];`.
 */
#define PCB_View_Arr_A_T(arr, viewType) \
    PCB_View_Arr_T(arr, 0, PCB_ARRAY_LEN(arr), viewType)
#endif //PCB_View_Arr_A_T

#ifndef PCB_View_Ptr_A_T
/**
 * @brief Constructs a view of type `viewType` on a pointer `ptr`
 * in the range of [0, `end`).
 * This differs from both `PCB_View_Vec_A_T` and `PCB_View_Arr_A_T` where `end` is
 * known. In the case of pointers, you are the one that provides the upper bound.
 *
 * If `end` goes out of bounds, the behavior is undefined.
 */
#define PCB_View_Ptr_A_T(ptr, viewType, end) PCB_View_Ptr_T(ptr, 0, end, viewType)
#endif //PCB_View_Ptr_A_T



#ifndef PCB_Slice_Vec
/**
 * @brief Constructs a slice of vector `vec` in the range of
 * [`start`, `end`).
 * Functionally identical to `PCB_View_Vec`. Use with slice types for clarity.
 */
#define PCB_Slice_Vec(vec, start, end) PCB_View_Vec(vec, start, end)
#endif //PCB_Slice_Vec

#ifndef PCB_Slice_Arr
/**
 * @brief Constructs a slice of array (!) `arr` in the range of
 * [`start`, `end`).
 * Functionally identical to `PCB_View_Arr`. Use with slice types for clarity.
 */
#define PCB_Slice_Arr(arr, start, end) PCB_View_Arr(arr, start, end)
#endif //PCB_Slice_Arr

#ifndef PCB_Slice_Ptr
/**
 * @brief Constructs a slice of pointer `ptr` in the range of
 * [`start`, `end`).
 * Functionally identical to `PCB_View_Ptr`. Use with slice types for clarity.
 */
#define PCB_Slice_Ptr(ptr, start, end) PCB_View_Ptr(ptr, start, end)
#endif //PCB_Slice_Ptr


#ifndef PCB_Slice_Vec_T
/**
 * @brief Constructs a slice of type `sliceType` on vector `vec`
 * in the range of [`start`, `end`).
 * Functionally identical to `PCB_View_Vec_T`. Use with slice types for clarity.
 */
#define PCB_Slice_Vec_T(vec, start, end, sliceType) \
    PCB_View_Vec_T(vec, start, end, sliceType)
#endif //PCB_Slice_Vec_T

#ifndef PCB_Slice_Arr_T
/**
 * @brief Constructs a slice of type `sliceType` on array (!) `arr`
 * in the range of [`start`, `end`).
 * Functionally identical to `PCB_View_Arr_T`. Use with slice types for clarity.
 */
#define PCB_Slice_Arr_T(arr, start, end, sliceType) \
    PCB_View_Arr_T(arr, start, end, sliceType)
#endif //PCB_Slice_Arr_T

#ifndef PCB_Slice_Ptr_T
/**
 * @brief Constructs a slice of type `sliceType` on pointer `ptr`
 * in the range of [`start`, `end`).
 * Functionally identical to `PCB_View_Ptr_T`. Use with slice types for clarity.
 */
#define PCB_Slice_Ptr_T(ptr, start, end, sliceType) \
    PCB_View_Ptr_T(ptr, start, end, sliceType)
#endif //PCB_Slice_Ptr_T


#ifndef PCB_Slice_Vec_A
/**
 * @brief Constructs a slice on the entire vector `vec`.
 * Functionally identical to `PCB_View_Vec_A`. Use with slice types for clarity.
 */
#define PCB_Slice_Vec_A(vec) PCB_Slice_Vec(vec, 0, (vec)->length)
#endif //PCB_Slice_Vec_A

#ifndef PCB_Slice_Arr_A
/**
 * @brief Constructs a slice on the entire array (!) `arr`.
 * Functionally identical to `PCB_View_Arr_A`. Use with slice types for clarity.
 */
#define PCB_Slice_Arr_A(arr) PCB_Slice_Arr(arr, 0, PCB_ARRAY_LEN(arr))
#endif //PCB_Slice_Arr_A

#ifndef PCB_Slice_Ptr_A
/**
 * @brief Constructs a slice on pointer `ptr` in the range of [0, `end`).
 * This differs from both `PCB_Slice_Vec_A` and `PCB_Slice_Arr_A` where `end` is
 * known. In the case of pointers, you are the one that provides the upper bound.
 *
 * Functionally identical to `PCB_View_Ptr_A`. Use with slice types for clarity.
 */
#define PCB_Slice_Ptr_A(ptr, end) PCB_Slice_Ptr(ptr, 0, end)
#endif //PCB_Slice_Ptr_A


#ifndef PCB_Slice_Vec_A_T
/**
 * @brief Constructs a slice of type `sliceType` on the entire vector `vec`.
 * Functionally identical to `PCB_View_Vec_A_T`. Use with slice types for clarity.
 */
#define PCB_Slice_Vec_A_T(vec, sliceType) \
    PCB_Slice_Vec_T(vec, 0, (vec)->length, sliceType)
#endif //PCB_Slice_Vec_A_T

#ifndef PCB_Slice_Arr_A_T
/**
 * @brief Constructs a slice of type `sliceType` on the entire array (!) `arr`.
 * Functionally identical to `PCB_View_Arr_A_T`. Use with slice types for clarity.
 */
#define PCB_Slice_Arr_A_T(arr, sliceType) \
    PCB_Slice_Arr_T(arr, 0, PCB_ARRAY_LEN(arr), sliceType)
#endif //PCB_Slice_Arr_A_T

#ifndef PCB_Slice_Ptr_A_T
/**
 * @brief Constructs a slice of type `viewType` on a pointer `ptr`
 * in the range of [0, `end`).
 * This differs from both `PCB_Slice_Vec_A_T` and `PCB_Slice_Arr_A_T` where `end` is
 * known. In the case of pointers, you are the one that provides the upper bound.
 *
 * Functionally identical to `PCB_View_Ptr_A_T`. Use with slice types for clarity.
 */
#define PCB_Slice_Ptr_A_T(ptr, sliceType, end) \
    PCB_Slice_Ptr_T(ptr, 0, end, sliceType)
#endif //PCB_Slice_Ptr_A_T



//Section 1.6.5: Other macros
#ifndef PCB_VA_forEach_until
#define PCB_VA_forEach_until(args, argType, end, name)      \
for(                                                        \
    argType name = va_arg((args), argType);                 \
    name != (end);                                          \
    name = va_arg((args), argType)                          \
)
#endif //PCB_VA_forEach_until



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
#include <sys/shm.h>
#include <sys/wait.h>
#include <dirent.h>
#include <spawn.h>
#include <signal.h>
#endif //platform-specific APIs



//Section 1.8: declarations of all library functions
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


//The user may not need declarations, for example if only macros are used.
//This macro allows for a slight preprocessor optimization by omitting declarations.
#ifndef PCB_NO_DECLARATIONS
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


/* A dynamic array of ASCII characters with a trailing zero at the end - a string.
 * Has a concrete implementation unlike other dynamic arrays.
 * The trailing zero is not included in its length.
 *
 * You can safely pass a `PCB_String*` to functions that expect a
 * `PCB_StringView*` since they share the prefix.
 *
 * You can, in most situations, safely pass a `const PCB_StringView*`
 * to functions that expect a `const PCB_String*`.
 *
 * If a function relates to memory management, it likely reads the `capacity`
 * field, which isn't present in a `PCB_StringView`, so you can't
 * pass it there (I mean, duh).
 *
 * For example, you can safely pass a `const PCB_StringView*` instead of
 * `const PCB_String*` into `PCB_String_append`, but you can't do that
 * with `PCB_String_clone`.
 */
typedef struct {
    char* data;
    size_t length;
    size_t capacity;
} PCB_String;

/* A non-owning view at a portion of some string.
 * Likely does not end with a zero, keep that in mind when passing
 * `data` to a function expecting a C string.
 *
 * You can safely pass a `PCB_String*` to functions that expect a
 * `PCB_StringView*` since they share the prefix.
 *
 * You can, in most situations, safely pass a `const PCB_StringView*`
 * to functions that expect a `const PCB_String*`.
 *
 * If a function relates to memory management, it likely reads the `capacity`
 * field, which isn't present in a `PCB_StringView`, so you can't
 * pass it there (I mean, duh).
 *
 * For example, you can safely pass a `const PCB_StringView*` instead of
 * `const PCB_String*` into `PCB_String_append`, but you can't do that
 * with `PCB_String_clone`.
 */
typedef struct {
    const char* data;
    size_t length;
} PCB_StringView;

typedef struct {
    PCB_String* data;
    size_t length;
    size_t capacity;
} PCB_Strings;

typedef struct {
    PCB_StringView* data;
    size_t length;
    size_t capacity;
} PCB_StringViews;

typedef struct {
    const char** data;
    size_t length;
    size_t capacity;
} PCB_CStrings;

typedef struct {
    const char* const* data;
    size_t length;
} PCB_CStringsView;

typedef struct {
    const char* key;
    const char* value;
} PCB_CStringPair;

typedef struct {
    PCB_CStringPair* data;
    size_t length;
    size_t capacity;
} PCB_CStringPairs;

typedef PCB_CStrings PCB_ShellCommand;

#ifndef PCB_CStrings_append
#define PCB_CStrings_append(cstrs, str) PCB_Vec_append(cstrs, str)
#endif //PCB_CStrings_append
#ifndef PCB_ShellCommand_append_arg
#define PCB_ShellCommand_append_arg PCB_CStrings_append
#endif //PCB_ShellCommand_append_arg

#ifndef PCB_CStrings_append_many
#define PCB_CStrings_append_many(cstrs, ...) \
    PCB_Vec_append_variadic(cstrs, const char*, __VA_ARGS__)
#endif //PCB_CStrings_append_many
#ifndef PCB_ShellCommand_append_args
#define PCB_ShellCommand_append_args PCB_CStrings_append_many
#endif //PCB_ShellCommand_append_args


typedef struct {
#if PCB_PLATFORM_WINDOWS
    HANDLE handle;
    DWORD status;
#elif PCB_PLATFORM_POSIX
    pid_t handle;
    int status;
#else
    int handle; //stub
#endif //platform-dependent handles to processes
} PCB_Process;

#ifndef PCB_PROCESS_INVALID_HANDLE
#if PCB_PLATFORM_WINDOWS
#define PCB_PROCESS_INVALID_HANDLE INVALID_HANDLE_VALUE
#elif PCB_PLATFORM_POSIX
/* 0 is the kernel (or part of it), i.e. no userspace process can be 0.
 * Any negative PID is also invalid as defined by POSIX.
 * See https://pubs.opengroup.org/onlinepubs/9699919799/.
 */
#define PCB_PROCESS_INVALID_HANDLE -1
#else
#define PCB_PROCESS_INVALID_HANDLE 0 //stub
#endif //platform
#endif //PCB_PROCESS_INVALID_HANDLE

typedef struct {
    PCB_Process* data;
    size_t length;
    size_t capacity;
} PCB_Processes;


typedef struct PCB_Arena PCB_Arena;
/**
 * @brief A prefix of `PCB_Arena` for metadata.
 */
typedef struct {
    size_t length;
    size_t capacity;
    PCB_Arena* next;
} PCB_Arena_Prefix;


typedef enum {
    /* Unknown argv syntax, causes `PCB_BuildContext`'s options
     * to be passed without processing.
     */
    PCB_ARGVSYNTAX_UNKNOWN,
    //POSIX argv syntax ("-...(=...)", for example "-DLEVEL=2").
    PCB_ARGVSYNTAX_POSIX,
    //Microsoft argv syntax ("/...(:...)", for example "/DLEVEL:2").
    PCB_ARGVSYNTAX_MS,
} PCB_ArgvSyntax;

/* Enum for compiler identification at *runtime* (hence the _RT suffix)
 * rather than compile time (without the _RT suffix).
 */
typedef enum {
    PCB_COMPILER_RT_UNKNOWN,
    PCB_COMPILER_RT_GCC,
    PCB_COMPILER_RT_CLANG,
    PCB_COMPILER_RT_MSVC,
} PCB_Compiler_RT;

typedef enum {
    PCB_BUILDTYPE_EXEC,
    PCB_BUILDTYPE_STATICLIB,
    PCB_BUILDTYPE_DYNAMICLIB,
} PCB_BuildType;

typedef struct {
    /* Path to the compiler executable to use for C and C++ respectively.
     * Defaults to the compiler's name used to build this file.
     */
    const char* compilerPath;
    //Path to the build directory for caching object files. Defaults to "build/".
    const char* buildPath;
    //Name of the final executable/shared object.
    const char* outputPath;
    /* Vector of paths to source directories/individual files.
     * Currently only 1 source directory is supported,
     * while individual files are not implemented yet.
     */
    PCB_CStrings sources;
    //Vector of paths to include directories.
    PCB_CStrings includes;
    //Vector of names of libraries to link dynamically.
    PCB_CStrings libs;
    //Vector of names of libraries to link *statically*.
    PCB_CStrings staticLibs;
    /* Vector of additional paths to pass to the compiler
     * to search for specified libraries.
     */
    PCB_CStrings librarySearchPaths;
    /* Vector of compiler optimization flags.
     * Not a singular `const char*` since, for example in GCC,
     * you can pass "-f*" optimization flags on top of "-O*" flags.
     */
    PCB_CStrings optimizationFlags;
    //Vector of debug flags. Put your own sanitizer flags here.
    PCB_CStrings debugFlags;
    /* Vector of warning flags, as well as warning-as-error flags.
     * Interpreted directly, a.k.a. if you add "1234",
     * the compiler will receive "1234" as a flag.
     */
    PCB_CStrings warningFlags;
    //Vector of flags for the preprocessor (defs and undefs).
    struct {
        PCB_CStringPairs defines;
        PCB_CStrings undefines;
    } preprocessorFlags;
    //Vector of other compiler flags not covered by the rest of this struct.
    PCB_CStrings otherCompilerFlags;
    //Vector of other linker flags not covered by the rest of this struct.
    PCB_CStrings otherLinkerFlags;
    //Internal buffer used for enumerating source paths.
    PCB_String currentSourcePath;
    //Internal buffer used for enumerating build paths w.r.t. the source path.
    PCB_String currentBuildPath;
    //Internal buffer used for commands when building.
    PCB_ShellCommand commandBuffer;
    //Internal vector of child process handles.
    PCB_Processes processes;
    //Internal arena allocator. May be safely reset after building and used for your own things.
    //DO NOT ALLOCATE ANYTHING PRIOR TO BUILDING!!! THE ALLOCATION WILL BE OVERRIDDEN!
    PCB_Arena* arena;
    //Internal buffer used for accumulating source file paths for compilation.
    PCB_CStrings sourceFiles;
    //Internal buffer used for accumulating object file paths for linking.
    PCB_CStrings objectFiles;
    /* The language standard used to compile source files.
     * Defaults to the standard used to build this file.
     * Setting it to 0 stops the standard flag from being added.
     */
    long standard;

    //Flags for the build context.
    //TODO: add useful stuff here
    union {
        unsigned int all;
//temporary macro for choosing between an unnamed struct if in C11+ or a named one
#ifndef PCB_TEMP
#if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__+0 < 201112L)
#define PCB_TEMP fields
#else
#define PCB_TEMP
#endif //C++ || <C11
#endif //PCB_TEMP
        struct {
            /* Specifies how many commands should be run in parallel.
             *
             * A value of 0, 1, >1 means
             * no parallelism,
             * running "number of cores in the system" commands in parallel,
             * running this exact amount of commands in parallel respectively.
             */
            unsigned char parallel;
            //Whether to force recompilation of all detected source files.
            unsigned char alwaysBuild : 1;
            /* Specifies the command-line argument parsing syntax.
             * See the `PCB_ArgvSyntax` enum for details.
             */
            PCB_ArgvSyntax argvSyntax : 2;
            /* Whether to use GNU extensions.
             * Only relevant with compilers that support it.
             * Otherwise it should be set to false.
             */
            unsigned char gnu : 1;
            //TODO: implementation & docs
            PCB_Compiler_RT compilerUsed : 3;
            /* Whether to use a C compiler for C files in a C++ build.
             * Setting this flag in C will cause C++ files to be compiled
             * with a C++ compiler instead of being skipped.
             */
            unsigned char ccInCpp : 1;
            //Whether anything was rebuilt. Only use when the entire build succeeded.
            unsigned char rebuiltAnything : 1;
            //TODO: implementation & docs
            PCB_BuildType buildType : 2;
            unsigned int _unused : 12;
        } PCB_TEMP;
#undef PCB_TEMP
    } flags;
} PCB_BuildContext;

#ifndef PCB_BuildContext_flags
#if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__+0 < 201112L)
#define PCB_BuildContext_flags(ctx) (ctx)->flags.fields
#else
#define PCB_BuildContext_flags(ctx) (ctx)->flags
#endif //C++ || <C11
#endif //PCB_BuildContext_flags

typedef enum {
    PCB_BUILDOPTION_NONE = 0,
    //Sets build path to "build/", adds "src/" to sources and "include/" to includes.
    PCB_BUILDOPTION_DEFAULT_PATHS = 1 << 1,
    //Sets compiler path, language standard to the compiler name,
    //standard used to compile this file and the compiler's argv syntax.
    PCB_BUILDOPTION_DEFAULT_COMPILER = 1 << 2,
    //Adds a list of warnings from `PCB__BuildContext_addDefaultWarnings()`.
    PCB_BUILDOPTION_DEFAULT_WARNINGS = 1 << 3,
    //Sets some fields to commonly used defaults.
    PCB_BUILDOPTION_DEFAULTS =
        PCB_BUILDOPTION_DEFAULT_PATHS    |
        PCB_BUILDOPTION_DEFAULT_COMPILER |
        PCB_BUILDOPTION_DEFAULT_WARNINGS,
    //Causes debug symbols to be emitted into object files. TODO: in MSVC
    PCB_BUILDOPTION_DEBUG = 1 << 4,
    //Equivalent to -O3 or /O2. For more granularity modify the build context manually.
    PCB_BUILDOPTION_OPTIMIZE = 1 << 5,
    //Turns the thread sanitizer (TSan) on if available. Incompatible with ASan and LSan.
    PCB_BUILDOPTION_TSAN = 1 << 6,
    //Turns the address sanitizer (ASan) on if available. Incompatible with TSan.
    PCB_BUILDOPTION_ASAN = 1 << 7,
    //Turns the leak sanitizer (LSan) on if available. Incompatible with TSan.
    PCB_BUILDOPTION_LSAN = 1 << 8,
    //Turns the undefined behavior sanitizer (UBSan) on if available.
    PCB_BUILDOPTION_UBSAN = 1 << 9,
    /* There are more sanitizers available, but they are compiler-exclusive:
     * Memory sanitizer (clang only):
     * https://clang.llvm.org/docs/MemorySanitizer.html
     * Fuzzing (MSVC only):
     * https://learn.microsoft.com/en-us/cpp/build/reference/fsanitize?view=msvc-170
     */

    //TODO: more options
} PCB_BuildOption;




/**
 * @brief Logs a `printf`-like string to either stdout or stderr
 * based on `level`.
 *
 * @param level log level
 *
 * if `PCB_LOGLEVEL_ERROR(_NL)` or `PCB_LOGLEVEL_FATAL(_NL)`,  logs to stderr,
 * otherwise logs to stdout
 */
PCBAPI void PCBCALL PCB_log(
    PCB_LogLevel level,
    const char* fmt,
    ...
) PCB_Printf_Format(2, 3);

#ifndef PCB_logTrace
#ifdef PCB_DEBUG
#if PCB_DEBUG+0 > 2
#define PCB_logTrace(...) do { \
    PCB_log(PCB_LOGLEVEL_TRACE_NL, "[" __FILE__ "/%s:" PCB_STRINGIFY(__LINE__) "] ", __func__); \
    PCB_log(PCB_LOGLEVEL_NONE, __VA_ARGS__); \
} while(0)
#elif PCB_DEBUG+0 == 2
#define PCB_logTrace(...) PCB_log(PCB_LOGLEVEL_TRACE, __VA_ARGS__)
#else
#define PCB_logTrace(...)
#endif //PCB_DEBUG > 1
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



/**
 * @brief Get the error code of the last error that has occured.
 *
 * WARNING: for the sake of attempting to be cross-platform, the return value
 * is *only* relevant in the context of this library. Do NOT use for error
 * handling outside. This is because Windows has its own error storage entirely
 * separate to libc, which conflict, making it very difficult to write a centralized
 * error handler.
 *
 * Note to future maintainers: this *is* possible in the context of this library,
 * BUT every function that errors out on libc MUST call `SetLastError(0)`; likewise,
 * every function that errors out on WinAPI MUST set `errno` to 0. This puts a whole
 * lot of strain on the implementation though, so TODO: I guess it'll be a
 * pain in the ass to do this.
 *
 * @return 0 if no error has occured,
 *
 * On Windows: a negative value if the error comes from libc (`errno`),
 * a positive value otherwise (WinAPI).
 *
 * On POSIX systems: `-errno`.
 *
 * Generally, if the return value is negative, the error comes from libc.
 * Otherwise it's platform-dependent.
 */
PCBAPI int PCBCALL PCB_GetError(void);
/**
 * @brief Clear the error obtainable with `PCB_GetError`.
 */
PCBAPI void PCBCALL PCB_ClearError(void);
/**
 * @brief Get the error string corresponding to `errnum` into `buf` of
 * `bufSize` size.
 *
 * WARNING: `errnum` ****MUST**** be obtained by calling `PCB_GetError`,
 * otherwise you lose **ALL** portability with regards to error handling
 * AND are **GUARANTEED** to get incorrect results.
 *
 * DO NOT BLINDLY PASS `errno` OR `GetLastError()` WITHOUT READING THE DOCS ABOVE!!
 *
 * @return 0 on success,
 * otherwise an error code according to the schema above is returned and
 * the previous error code is preserved.
 */
PCBAPI int PCBCALL PCB_GetErrorString(
    int errnum,
    char* buf,
    size_t bufSize
);
/**
 * @brief Log the latest error obtained from `PCB_GetError()` to stderr.
 * Otherwise functions similarly to `printf`.
 *
 * @param fmt `printf`-like format string
 */
PCBAPI void PCBCALL PCB_logLatestError(
    const char* fmt,
    ...
) PCB_Printf_Format(1, 2);

/**
 * @brief Creates a directory in the given `path`.
 * Returns whether the operation succeeded.
 *
 * Failure by "it already exists" is treated as success.
 *
 * On Linux, permission field of the created directory is `rwxrwxr-x`.
 * @param path path/to/directory/to/create, not transitive
 */
PCBAPI bool PCBCALL PCB_mkdir(const char* path);
/**
 * @brief Checks if a filesystem entry exists.
 *
 * @param path path/to/thing/in/filesystem
 * @return `true` if exists, `false` if doesn't, a negative value
 * otherwise. To get the error code call `PCB_GetError`.
 */
PCBAPI int PCBCALL PCB_FS_Exists(const char* path);
/**
 * @brief Get the file type of `path`.
 * Note that if `path` refers to a symbolic link, the value returned
 * contains a bitwise OR of the underlying file type and `PCB_FILETYPE_SYMLINK`.
 * Therefore you shouldn't use `==` directly with the return value.
 * To ignore the symlink bit, apply bitwise AND with `PCB_FILETYPE_SYMLINK_IGN`
 * to remove it.
 * @param path path/to/thing/in/filesystem
 */
PCBAPI PCB_FileType PCBCALL PCB_FS_GetType(const char* path);
/**
 * @brief Get the modification time of a filesystem entry.
 *
 * @return 0 on error, 1 if the entry doesn't exist, some other integer otherwise.
 */
PCBAPI uint64_t PCBCALL PCB_FS_GetModificationTime(const char* path);
/**
* @brief Loads entire file from `path` into a dynamically allocated buffer.
* @return `true` on success, `false` on error; to get the error
* code call `PCB_GetError()`.
*/
PCBAPI bool PCBCALL PCB_FS_ReadEntireFile(
    const char* path,
    PCB_String* buf
);



/**
 * @brief Frees `str`'s buffer and resets fields to 0.
 */
PCBAPI void PCBCALL PCB_String_destroy(PCB_String* PCB_restrict str);
#ifndef PCB_String_reset
/**
 * @brief Resets `str` to hold no string. Does nothing if `str->data == NULL`.
 */
#define PCB_String_reset(str) while((str)->data != NULL) { \
    (str)->data[(str)->length = 0] = '\0'; break; \
}
#endif //PCB_String_reset
/**
 * @brief Reserves `howMany` *additional* bytes in `str`.
 * @return whether the operation succeeded: fails on realloc failure.
 */
PCBAPI bool PCBCALL PCB_String_reserve(
    PCB_String* PCB_restrict str,
    const size_t howMany
);
/**
 * @brief Resizes `str` to fit a string of `targetLength` length.
 * Truncates the string to `targetLength` if `targetLength < str->length`.
 * Does nothing if `targetLength == str->length`.
 * Behaves identically to `PCB_String_reserve` otherwise.
 * @return whether the operation succeeded: fails on realloc failure.
 */
PCBAPI bool PCBCALL PCB_String_resize(
    PCB_String* PCB_restrict str,
    const size_t targetLength
);
/**
 * @brief Appends `other` to `str`.
 *
 * Note: it is permitted that `other == str`.
 * @return whether the operation succeeded:
 * fails on realloc failure or if `other == NULL`.
 */
PCBAPI bool PCBCALL PCB_String_append(
    PCB_String* str,
    const PCB_String* other
);
/**
 * @brief Appends `sv` to `str`. `sv` may be empty.
 *
 * Note: it is NOT permitted that `sv` is a view of `str` as it may be
 * invalidated by a realloc.
 * @return whether the operation succeeded: fails on realloc failure.
 */
PCBAPI bool PCBCALL PCB_String_append_sv(
    PCB_String* PCB_restrict str,
    PCB_StringView sv
);
/**
 * @brief Appends `cstr` to `str`.
 *
 * Note: it is NOT permitted that `cstr` overlaps with `str->data`, i.e. points to
 * a range [`str->data`, `str->data + str->length`) as it may be
 * invalidated by a realloc.
 * @return whether the operation succeeded:
 * fails on realloc failure or if `cstr == NULL` or the note above.
 */
PCBAPI bool PCBCALL PCB_String_append_cstr(
    PCB_String* PCB_restrict str,
    const char* PCB_maybe_restrict cstr
);
/**
 * @brief Appends C-strings from `cstrs` to `str`.
 *
 * If all entries in `cstrs` are empty C-strings or if `cstrs` is empty,
 * nothing is appended.
 * NULL entries and entries that overlap with `str` (see above)
 * in `cstrs` are skipped.
 * @return number of entries in `cstrs` appended, -1 on error.
 */
PCBAPI ssize_t PCBCALL PCB_String_append_cstrs(
    PCB_String* PCB_restrict str,
    PCB_CStringsView cstrs
);
/**
 * @brief Appends variable number of C-strings to `str`.
 *
 * C-strings overlapping with `str->data` are skipped.
 *
 * The last argument MUST be `NULL`. Otherwise the behavior is undefined.
 * @return number of C-strings appended, 0 on error.
 */
PCBAPI ssize_t PCBCALL PCB_String_append_cstr_v(
    PCB_String* PCB_restrict str,
    ...
);
/**
 * @brief Appends `c` to `str` `howManyTimes` times.
 * `c == '\0'` is treated as a no-op.
 * @return whether the operation succeeded: can fail on realloc failure.
 */
PCBAPI bool PCBCALL PCB_String_append_chars(
    PCB_String* PCB_restrict str,
    const char c,
    const size_t howManyTimes
);
/**
 * @brief Appends a `printf`-like formatted string to `str`.
 *
 * Note: it is NOT permitted to append a formatted string to `str` based on
 * `str` itself, i.e.`fmt` cannot overlap with `[str->data, str->data + str->length`).
 *
 * This function is only available if PCB was compiled with stdio.h present.
 * Otherwise it does nothing and always returns `false`.
 *
 * @return whether the operation succeeded: can fail on realloc failure.
 */
PCBAPI bool PCBCALL PCB_String_appendf(
    PCB_String* PCB_restrict str,
    const char* PCB_maybe_restrict fmt,
    ...
) PCB_Printf_Format(2, 3);
/**
 * @brief Inserts `other` into `str` at position `position`.
 * Inserting `str` into itself (`str == other`) is not allowed.
 * @return whether the operation succeeded:
 * fails on invalid arguments passed or if realloc failed.
 */
PCBAPI bool PCBCALL PCB_String_insert(
    PCB_String* PCB_maybe_restrict str,
    const PCB_String* PCB_maybe_restrict other,
    size_t position
);
/**
 * @brief Inserts `sv` into `str` at position `position`
 *
 * Note: It is NOT permitted that `sv` overlaps with `str`, i.e. views a range
 * [`str->data`, `str->data + str->length`) as it may be invalidated by a realloc.
 * @return whether the operation succeeded.
 */
PCBAPI bool PCBCALL PCB_String_insert_sv(
    PCB_String* PCB_restrict str,
    PCB_StringView sv,
    size_t position
);
/**
 * @brief Inserts `cstr` into `str` at `position`.
 *
 * Note: it is NOT permitted that `cstr` overlaps with `str->data`, i.e. points to
 * a range [`str->data`, `str->data + str->length`) as it may be
 * invalidated by a realloc.
 * @return whether the operation succeeded: fails on
 * invalid arguments passed or if realloc failed.
 */
PCBAPI bool PCBCALL PCB_String_insert_cstr(
    PCB_String* PCB_restrict str,
    const char* PCB_maybe_restrict cstr,
    size_t position
);
/**
 * @brief Inserts C-string from `cstrs` into `str` at position `position`.
 *
 * If all entries in `cstrs` are empty C-strings or if `cstrs` is empty,
 * nothing is appended.
 * NULL entries and entries that overlap with `str->data` (see above)
 * in `cstrs` are skipped.
 * @return number of entries in `cstrs` inserted, -1 on error.
 */
PCBAPI ssize_t PCBCALL PCB_String_insert_cstrs(
    PCB_String* PCB_restrict str,
    PCB_CStringsView cstrs,
    size_t position
);
/**
 * @brief Inserts variable number of C-strings into `str` at position `position`.
 * The last argument MUST be `NULL`.
 *
 * C-strings overlapping with `str->data` are skipped.
 *
 * @return number of entries in `cstrs` inserted, -1 on error.
 */
PCBAPI ssize_t PCBCALL PCB_String_insert_cstr_v(
    PCB_String* PCB_restrict str,
    size_t position,
    ...
);
/**
 * @brief Inserts `c` into `str` at position `position` `howManyTimes` times.
 * Inserting '\0' is not allowed.
 * @return whether the operation succeeded:
 * fails on invalid arguments passed or if realloc failed.
 */
PCBAPI bool PCBCALL PCB_String_insert_chars(
    PCB_String* PCB_restrict str,
    const char c,
    size_t howManyTimes,
    size_t position
);
/**
 * @brief Inserts a `printf`-like formatted string into `str` at position `position`.
 *
 * Note: it is NOT permitted to insert a formatted string to `str` based on
 * `str` itself, i.e.`fmt` cannot overlap with `str->data`.
 *
 * This function is only available if the library was compiled with stdio.h present.
 * Otherwise it does nothing and always returns `false`.
 *
 * @return whether the operation succeeded:
 * fails on invalid arguments passed or if realloc failed.
 */
PCBAPI bool PCBCALL PCB_String_insertf(
    PCB_String* PCB_restrict str,
    const char* PCB_maybe_restrict fmt,
    size_t position,
    ...
) PCB_Printf_Format(2, 4);
/**
 * @brief Replaces characters in the range `[start, start + length)`
 * in `str` with `other`.
 *
 * Note: It is NOT permitted that `other` overlaps with `str`, i.e. views a range
 * [`str->data`, `str->data + str->length`) as it may be invalidated by a realloc.
 * @return whether the operation succeded: TODO
 */
PCBAPI bool PCBCALL PCB_String_replace_range(
    PCB_String* PCB_restrict str,
    size_t start,
    size_t length,
    PCB_StringView other
);
/**
 * @brief Replaces characters in the range `[start, start + length)`
 * in `str` with the character `c`.
 * Replacing with '\0' is not allowed.
 * @return whether the operation succeded: 
 */
PCBAPI bool PCBCALL PCB_String_replace_range_chars(
    PCB_String* PCB_restrict str,
    size_t start,
    size_t length,
    char c
);
/**
 * @brief Removes characters in the range `[start, start + length)`.
 * @return whether the operation succeded: can only fail on invalid range passed
 */
PCBAPI bool PCBCALL PCB_String_remove_range(
    PCB_String* PCB_restrict str,
    size_t start,
    size_t length
);
/**
 * @brief Makes `c` the last character in `str`.
 * If `c` is not the last character, it appends it.
 * Otherwise does nothing.
 * @return `false` if reallocation failed, `true` otherwise
 */
PCBAPI bool PCBCALL PCB_String_setSuffix_char(
    PCB_String* PCB_restrict str,
    const char c
);
/**
 * @brief Truncate `str` until `c` is found. If `c` is not found, `str` is *not*
 * truncated.
 * @return `true` if successfully truncated,`false` if `c` was not found
 */
PCBAPI bool PCBCALL PCB_String_truncate_until_char(
    PCB_String* PCB_restrict str,
    const char c
);
/**
 * @brief Clones `str`.
 * @return an initialized `PCB_String` structure or a zeroed out one on failure
 */
PCBAPI PCB_String PCBCALL PCB_String_clone(
    const PCB_String* PCB_restrict str
);
/**
 * @brief Compares `a` and `b` lexicographically.
 * @return
 * 0 on equality or if `a` and `b` are invalid,
 *
 * a negative value if `a` < `b` or `b` is invalid,
 *
 * a positive value if `a` > `b` or `a` is invalid.
 */
PCBAPI int PCBCALL PCB_String_compare(
    const PCB_String* a,
    const PCB_String* b
);
/**
 * @brief Compares `a` and `b`, case insensitive version*.
 * @return same as `PCB_String_compare`.
 *
 * * - the comparison is done as if `a` and `b` are ASCII strings and ignores
 * Unicode
 */
PCBAPI int PCBCALL PCB_String_compare_ci(
    const PCB_String* a,
    const PCB_String* b
);
/**
 * @brief Compares `a` and `b` lexicographically, version with a C string.
 * @return same as `PCB_String_compare`.
 */
PCBAPI int PCBCALL PCB_String_compare_cstr(
    const PCB_String* PCB_restrict a,
    const char* PCB_restrict b
);
/**
 * @brief Compares `a` and `b`, case insensitive version* with a C string.
 * @return same as `PCB_String_compare`.
 *
 * * - the comparison is done as if `a` and `b` are ASCII strings and ignores
 * Unicode
 */
PCBAPI int PCBCALL PCB_String_compare_cstr_ci(
    const PCB_String* PCB_restrict a,
    const char* PCB_restrict b
);
/**
 * @brief Checks whether `a` and `b` are equal.
 * Faster than `PCB_String_compare` since it can shortcut on differing lengths.
 */
PCBAPI bool PCBCALL PCB_String_eq(
    const PCB_String* a,
    const PCB_String* b
);
/**
 * @brief Checks if `str` starts with `other`.
 * If any of them are empty, returns false.
 * @return whether `str` starts with `other`
 */
PCBAPI bool PCBCALL PCB_String_startsWith(
    const PCB_String* str,
    const PCB_String* other
);
/**
 * @brief Checks if `str` starts with `other`.
 * If `str` is empty, returns false.
 * @return whether `str` starts with `other`
 */
PCBAPI bool PCBCALL PCB_String_startsWith_cstr(
    const PCB_String* PCB_restrict str,
    const char* PCB_restrict other
);
/**
 * @brief Checks if `str` ends with `other`.
 * If any of them are empty, returns false.
 * @return whether `str` ends with `other`
 */
PCBAPI bool PCBCALL PCB_String_endsWith(
    const PCB_String* str,
    const PCB_String* other
);
/**
 * @brief Checks if `str` ends with `other`.
 * If `str` is empty, returns false.
 * @return whether `str` ends with `other`
 */
PCBAPI bool PCBCALL PCB_String_endsWith_cstr(
    const PCB_String* PCB_restrict str,
    const char* PCB_restrict other
);
#ifndef PCB_String_isEmpty
#define PCB_String_isEmpty(str) ((str)->data == NULL || (str)->length == 0)
#endif //PCB_String_isEmpty
/**
 * @brief Converts `str` to uppercase.
 *
 * Note: ignores Unicode.
 */
PCBAPI void PCBCALL PCB_String_toUpperCase(
    PCB_String* PCB_restrict str
);
/**
 * @brief Converts `str` to lowercase.
 *
 * Note: ignores Unicode.
 */
PCBAPI void PCBCALL PCB_String_toLowerCase(
    PCB_String* PCB_restrict str
);
/**
 * @brief Converts a clone of `str` to uppercase.
 *
 * Note: ignores Unicode.
 * @return an initialized `PCB_String` structure or a zeroed out one on failure
 */
PCBAPI PCB_String PCBCALL PCB_String_toUpperCase_copy(
    const PCB_String* PCB_restrict str
);
/**
 * @brief Converts a clone of `str` to lowercase.
 *
 * Note: ignores Unicode.
 * @return an initialized `PCB_String` structure or a zeroed out one on failure
 */
PCBAPI PCB_String PCBCALL PCB_String_toLowerCase_copy(
    const PCB_String* PCB_restrict str
);
/**
 * @brief Pops the last character in `str`.
 * If `str` is empty, returns `\0` without modification.
 */
PCBAPI char PCBCALL PCB_String_pop(
    PCB_String* PCB_restrict str
);
/**
 * @brief Pops `howMany` characters from `str` into `out`.
 * If `howMany > str->length`, `howMany` is clamped to `str->length`.
 * If `out` is NULL, characters are discarded.
 * The caller must ensure that `out` can hold at least `howMany` bytes.
 * Note: it is NOT permitted that `out` overlaps with `str->data`, i.e. points to
 * a range [`str->data`, `str->data + str->length`) as it may trigger undefined
 * behavior.
 * @return number of characters popped
 */
PCBAPI size_t PCBCALL PCB_String_pop_many(
    PCB_String* PCB_restrict str,
    size_t howMany,
    char* PCB_restrict out
);
/**
 * @brief Removes `other->length` characters from `str` if they match.
 * @return new length, 0 on error
 */
PCBAPI size_t PCBCALL PCB_String_removeSuffix(
    PCB_String* str,
    const PCB_String* other
);
/**
 * @brief Creates a new `PCB_String` from `sv`.
 * @return an initialized `PCB_String` structure or a zeroed out one on failure
 */
PCBAPI PCB_String PCBCALL PCB_String_from_StringView(const PCB_StringView* sv);
/**
 * @brief Creates a new `PCB_String` from `cstrs` joined with `delimiter.
 * @return an initialized `PCB_String` structure or a zeroed out one on failure
 */
PCBAPI PCB_String PCBCALL PCB_String_from_CStrings(
    const PCB_CStrings* PCB_restrict cstrs,
    const char* PCB_restrict delimiter
);

/**
 * @brief Find `n`th occurence of a substring `sub` in `sv`. Start searching
 * from the character at index `start`.
 * @return non-empty `PCB_StringView` on success or an empty one on invalid
 * arguments or if `sub` was not found.
 */
PCBAPI PCB_StringView PCBCALL PCB_StringView_substr_n(
    PCB_StringView sv,
    const PCB_StringView sub,
    size_t n,
    size_t start
);
/**
 * @brief Splits `sv` by `delim` into string views pointing to it.
 * @return non-empty `PCB_StringViews` on success or an empty one on error.
 */
PCBAPI PCB_StringViews PCBCALL PCB_StringView_split(
    PCB_StringView sv,
    PCB_StringView delim
);
/**
 * @brief Splits `sv` by `delim` into substring copies.
 * @return non-empty `PCB_Strings` on success or an empty one on error.
 */
PCBAPI PCB_Strings PCBCALL PCB_StringView_split_copy(
    PCB_StringView sv,
    PCB_StringView delim
);
/**
 * @brief Splits `sv` by whitespace characters into string views pointing to it.
 * @return non-empty `PCB_StringViews` on success or an empty one on error.
 */
PCBAPI PCB_StringViews PCBCALL PCB_StringView_split_whitespace(
    PCB_StringView sv
);
/**
 * @brief Splits `sv` by whitespace characters into substring copies.
 * @return non-empty `PCB_Strings` on success or an empty one on error.
 */
PCBAPI PCB_Strings PCBCALL PCB_StringView_split_whitespace_copy(
    PCB_StringView sv
);
/**
 * @brief Find the `n`th occurence of any byte in `accept` inside `sv`.
 * @return a sub`PCB_StringView` with:
 *
 * - `data` pointing to the `n`th occurence of a byte in `sv`
 *   matching one of the bytes in `accept`,
 *
 * - `length` equal to `sv.length` - the amount of bytes skipped
 *
 * or an empty `PCB_StringView` if `sv` is empty or `accept` is empty or `n == 0`
 * or if no byte from `accept` was found in `sv`.
 */
PCBAPI PCB_StringView PCBCALL PCB_StringView_findCharFrom_n(
    PCB_StringView sv,
    PCB_StringView accept,
    size_t n
);
/**
 * @brief Find the `n`th occurence of any byte NOT in `accept` inside `sv`.
 * @return a sub`PCB_StringView` with:
 *
 * - `data` pointing to the `n`th occurence of a byte in `sv`
 *   matching none of the bytes in `accept`,
 *
 * - `length` equal to `sv.length` - the amount of bytes skipped
 *
 * or an empty `PCB_StringView` if `sv` is empty or `accept` is empty or `n == 0`
 * or if no byte outside of `accept` was found in `sv`.
 */
PCBAPI PCB_StringView PCBCALL PCB_StringView_findCharNotFrom_n(
    PCB_StringView sv,
    PCB_StringView accept,
    size_t n
);

PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_from_String(
    const PCB_String* PCB_restrict str
);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_from_cstr(
    const char* PCB_restrict str
);
PCB_maybe_inline bool PCBCALL PCB_String_replace_range_cstr(
    PCB_String* PCB_restrict str,
    size_t start,
    size_t length,
    const char* PCB_restrict cstr
);
/**
 * The block below declares every combination of
 * "get either the 1st or `n`th substring from either a StringView
 * or a String, where the substring is either a String(View) or a C-string,
 * either getting a view to it or copying it". They all eventually call
 * `PCB_StringView_substr_n`.
 *
 * For example, "PCB_StringView_subcstr_n" ->
 * "find the `n`th substring, which is a C-string, inside a StringView and return a view to it",
 * while "PCB_String_substr" ->
 * "find the 1st substring, which is a String, inside a String and return a view to it".
 *
 * If you want an expandable copy of the string, use `PCB_String_from_StringView`.
 *
 * There are no functions that accept arguments 1) and 2) respectively for
 * `const PCB_String*`  & `PCB_StringView` or
 * `PCB_StringView`     & `const PCB_String*` or
 * `const char*`        & `PCB_StringView` or
 * `const char*`        & `const PCB_String*` or
 * return a `const char*`
 * since that'd require 32x more functions for a total
 * of a whopping 256 combinations, which is bogus. Deal with it yourself.
 */
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_substr   (PCB_StringView    sv,  PCB_StringView    sub, size_t start);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_subcstr  (PCB_StringView    sv,  const char*       sub, size_t start);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_subcstr_n(PCB_StringView    sv,  const char*       sub, size_t n,    size_t start);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_substr       (const PCB_String* str, const PCB_String* sub, size_t start);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_subcstr      (const PCB_String* str, const char*       sub, size_t start);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_substr_n     (const PCB_String* str, const PCB_String* sub, size_t n,    size_t start);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_subcstr_n    (const PCB_String* str, const char*       sub, size_t n,    size_t start);
/**
 * The same as above is done here for `PCB_StringView_split(_copy)`.
 * "PCB_String_split_copy" ->
 * "split String into Strings separated by a String",
 * "PCB_StringView_split_cstr" ->
 * "split StringView into StringViews separated by a C-string".
 * "PCB_StringView_split_char" ->
 * "split StringView into StringViews separated by a byte".
 */
PCB_maybe_inline PCB_StringViews PCBCALL PCB_StringView_split_cstr       (PCB_StringView    sv,  const char*       delim);
PCB_maybe_inline PCB_StringViews PCBCALL PCB_StringView_split_char       (PCB_StringView    sv,  const char        delim);
PCB_maybe_inline PCB_StringViews PCBCALL PCB_String_split                (const PCB_String* str, const PCB_String* delim);
PCB_maybe_inline PCB_StringViews PCBCALL PCB_String_split_cstr           (const PCB_String* str, const char*       delim);
PCB_maybe_inline PCB_StringViews PCBCALL PCB_String_split_char           (const PCB_String* str, const char        delim);
PCB_maybe_inline PCB_StringViews PCBCALL PCB_String_split_whitespace     (const PCB_String* str);
PCB_maybe_inline PCB_Strings     PCBCALL PCB_StringView_split_cstr_copy  (PCB_StringView    sv,  const char*       delim);
PCB_maybe_inline PCB_Strings     PCBCALL PCB_StringView_split_char_copy  (PCB_StringView    sv,  const char        delim);
PCB_maybe_inline PCB_Strings     PCBCALL PCB_String_split_copy           (const PCB_String* str, const PCB_String* delim);
PCB_maybe_inline PCB_Strings     PCBCALL PCB_String_split_cstr_copy      (const PCB_String* str, const char*       delim);
PCB_maybe_inline PCB_Strings     PCBCALL PCB_String_split_char_copy      (const PCB_String* str, const char        delim);
PCB_maybe_inline PCB_Strings     PCBCALL PCB_String_split_whitespace_copy(const PCB_String* str);
/* Similarly for `PCB_StringView_findCharFrom_n`. */
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_findCharFrom          (PCB_StringView    sv,  PCB_StringView    accept);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_findCharFrom_cstr     (PCB_StringView    sv,  const char*       accept);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_findCharFrom_cstr_n   (PCB_StringView    sv,  const char*       accept, size_t n);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_findCharFrom              (const PCB_String* str, const PCB_String* accept);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_findCharFrom_n            (const PCB_String* str, const PCB_String* accept, size_t n);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_findCharFrom_cstr         (const PCB_String* str, const char*       accept);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_findCharFrom_cstr_n       (const PCB_String* str, const char*       accept, size_t n);
/* Similarly for `PCB_StringView PCBCALL_findCharNotFrom_n`. */
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_findCharNotFrom       (PCB_StringView    sv,  PCB_StringView    accept);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_findCharNotFrom_cstr  (PCB_StringView    sv,  const char*       accept);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_findCharNotFrom_cstr_n(PCB_StringView    sv,  const char*       accept, size_t n);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_findCharNotFrom           (const PCB_String* str, const PCB_String* accept);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_findCharNotFrom_n         (const PCB_String* str, const PCB_String* accept, size_t n);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_findCharNotFrom_cstr      (const PCB_String* str, const char*       accept);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_findCharNotFrom_cstr_n    (const PCB_String* str, const char*       accept, size_t n);


/**
 * @brief Returns a `PCB_Process` structure with data
 * about itself. Not implemented.
 */
PCBAPI PCB_Process PCBCALL PCB_Process_self(void);
/**
 * @brief Create a `process` struct with invalid fields (all other functions
 * rely on this fact). This function MUST be used instead of standard
 * zero-initialization, otherwise the behavior is undefined.
*/
PCBAPI PCB_Process PCBCALL PCB_Process_init(void);
/**
 * @brief Checks whether `process` is a valid process.
 * NOTE: does NOT check whether `process` is an existing process.
 */
PCBAPI bool PCBCALL PCB_Process_isValid(const PCB_Process* process);
/**
 * @brief Waits for `process` to exit.
 * @return `true` if `process` exited, `false` on error; call `PCB_GetError()`
 * to get the error code.
 */
PCBAPI bool PCBCALL PCB_Process_waitForExit(PCB_Process* process);
/**
* @brief Checks if `process` exited.
* @return `true` if `process` exited, `false` if not, -1 on error;
* call `PCB_GetError()` to get the error code.
*/
PCBAPI int PCBCALL PCB_Process_checkExit(PCB_Process* process);
/**
* @brief Get the exit code of `process`.
* @return
* On Windows: `process->status` (it is mapped directly to an exit code there),
* On POSIX systems:
* - value in the range of [0, 255] for a normal exit,
* - `-s-1` if terminated by a signal with a numeric value `s`,
* - -1 otherwise - this can mean that `process` was stopped/continued; use
* `WIFSTOPPED`, `WSTOPSIG`, `WIFCONTINUED` macros inside
* `#if PCB_PLATFORM_POSIX` on `process->status`
* to handle that case since it's platform-specific.
*/
PCBAPI int PCBCALL PCB_Process_getExitCode(const PCB_Process* process);
/**
 * @brief Destroys the passed `process` structure, invalidates
 * its member fields.
 */
PCBAPI void PCBCALL PCB_Process_destroy(PCB_Process* process);

/**
 * @brief Waits for any process in `processes` to exit.
 * The entry for the process that exits is invalidated by calling
 * `PCB_Process_destroy` on it.
 * @return the exit code of the exited process or -1 on error;
 * call `PCB_GetError()` to get the error code.
 */
PCBAPI int PCBCALL PCB_Processes_waitForAny(PCB_Processes* processes);
/**
 * @brief Waits for a subset of processes in `processes` in a range of
 * [`start`, `end`). Ignores invalid entries.
 * Contrary to `PCB_Processes_waitForAny` it does NOT invalidate entries.
 * @return
 * - 0 if all processes exited with code 0,
 * - -1 if `processes == NULL` (errno is set to EFAULT) or
 *   `end > processes->length` (errno is set to EINVAL),
 * - `n` if `n`th (at index `n-1`) process did not exit with code 0
 * (subtract 1 to get the index), call `PCB_Process_getExitCode` on it
 * to get the exit code,
 * - `-n-1` if waiting for the `n`th (at index `n-1`) process failed
 * (flip the sign and add 2 to get the index).
 *
 * In the last 2 cases, processes after the `n`th one are NOT waited on;
 * the caller must retry waiting on the rest.
 */
PCBAPI int PCBCALL PCB_Processes_waitForRange(
    PCB_Processes* PCB_restrict processes,
    size_t start,
    size_t end
);
/**
 * @brief Waits for all processes in `processes` to exit.
 * Ignores invalid entries.
 * Contrary to `PCB_Processes_waitForAny` it does NOT invalidate entries.
 * This function is equivalent to
 * `PCB_Processes_waitForRange(processes, 0, processes->length)`.
 * Read its documentation before using this function.
 * @return See `PCB_Processes_waitForRange`.
 */
PCBAPI int PCBCALL PCB_Processes_waitForAll(PCB_Processes* processes);

/**
 * @brief Spawns a child process, which runs `command` concurrently.
 * @return a valid `PCB_Process` structure with information about the
 * child process or a structure with an invalid `handle` field on error.
 * To check it, use `PCB_Process_isValid`. The error is logged automatically.
 *
 * On POSIX systems, if `command` is not null-terminated, this function will
 * append `NULL` to `command` prior to calling `exec` and remove it afterwards.
 */
PCBAPI PCB_Process PCBCALL PCB_ShellCommand_runBg(PCB_ShellCommand* command);
/**
 * @brief Runs `command` and waits for it to exit.
 * @return the exit code of `command` or -1 on error,
 * to get the error code call `PCB_GetError()`. The error is logged automatically.
 */
PCBAPI int PCBCALL PCB_ShellCommand_runAndWait(PCB_ShellCommand* command);



/**
 * @brief Creates a new arena allocator with `size` bytes as initial capacity.
 * @return a valid pointer to the arena
 * or NULL if `size == 0` or the allocation failed
 */
PCBAPI PCB_Arena* PCBCALL PCB_Arena_init(size_t size);
/**
 * @brief Initialize a `PCB_Arena` in a chunk of memory pointed to by `mem`
 * and size `memsize`.
 * @return a valid pointer to the arena or NULL if `memsize` is insufficient to
 * hold the arena.
 */
PCBAPI PCB_Arena* PCBCALL PCB_Arena_init_in(void* mem, size_t memsize);
/**
 * @brief Allocates `size` bytes in `arena`.
 * The actual number of bytes allocated will be rounded up to pointer size.
 * @return a valid pointer to the allocated buffer aligned to pointer size
 * or NULL if `size == 0` or if allocation failed
 */
PCBAPI void* PCBCALL PCB_Arena_alloc(PCB_Arena* arena, size_t size);
/**
 * @brief Resets `arena` as if nothing was allocated.
 */
PCBAPI void PCBCALL PCB_Arena_reset(PCB_Arena* arena);
/**
 * @brief Destroys `arena`, i.e. frees blocks contained within it.
 * After this call, `arena` becomes a dangling pointer!
 */
PCBAPI void PCBCALL PCB_Arena_destroy(PCB_Arena* arena);
/**
 * @brief `sprintf`s a new string in `arena`.
 * @return pointer to the allocated string or NULL on error
 *
 * This function is only available if PCB was compiled with stdio.h present.
 * Otherwise it always returns NULL.
 */
PCBAPI char* PCBCALL PCB_Arena_asprintf(
    PCB_Arena* arena,
    const char* fmt, ...
) PCB_Printf_Format(2, 3);
/**
 * @brief Duplicates `str` in `arena`.
 * @return pointer to the duplicated string or NULL if `str == NULL` or if
 * allocation failed.
 */
PCBAPI char* PCBCALL PCB_Arena_strdup(
    PCB_Arena* arena,
    const char* str
);
/**
 * @brief Duplicates `str` in `arena`, copying at most `n` bytes.
 * @return pointer to the duplicated string or NULL if `str == NULL` or if
 * allocation failed.
 */
PCBAPI char* PCBCALL PCB_Arena_strndup(
    PCB_Arena* arena,
    const char* str,
    size_t n
);


/**
 * @brief Get the number of cores in the system.
 * A value of 0 is returned on platforms that are not supported.
 */
PCBAPI size_t PCBCALL PCB_getNumberOfCores(void);


/**
 * @brief Get the string version of the C standard from an integer value
 * `standard` (for example, `199901` for "c99").
 * @return a pointer to a string literal or NULL if a match wasn't found
 */
PCBAPI const char* PCBCALL PCB_GetCStandardStr(long standard);
/**
 * @brief Get the string version of the C standard from an integer value
 * `standard` (for example, `202002` for "c++20").
 * @return a pointer to a string literal or NULL if a match wasn't found
 */
PCBAPI const char* PCBCALL PCB_GetCppStandardStr(long standard);

/**
 * @brief Get the integer version of the C standard from a C string.
 *
 * @param standard C string containing "cX" where "X" identifies
 * the standard (for example "c99" for `199901`).
 * 'c' in "cX" MUST be lowercase, such is the requirement of GCC/Clang
 * for "-std=" flag.
 * @return non-zero integer value of a standard or 0 if a match wasn't found.
 * For C89, the value returned is exceptionally `1` since it didn't have
 * a specific value associated.
 */
PCBAPI long PCBCALL PCB_GetCStandardInt(const char* standard);
/**
 * @brief Get the integer version of the C++ standard from a C string
 *
 * @param standard C string containing "c++X", where "X" identifies
 * the standard (for example "c++20" for `202002`).
 * 'c' in "c++X" MUST be lowercase, such is the requirement of GCC/Clang
 * for "-std=" flag.
 * @return non-zero integer value of a standard or 0 if a match wasn't found.
 */
PCBAPI long PCBCALL PCB_GetCppStandardInt(const char* standard);


/**
 * @brief Initializes the passed `context`.
 *
 * @param flags PCB_BuildOptions OR'ed together
 * @return 0 on success or non-zero value on error; TODO: docs for error values
 */
PCBAPI int PCBCALL PCB_BuildContext_init(PCB_BuildContext* context, int flags);
/**
 * @brief Create a PCB_BuildContext struct.
 *
 * This function exists for lazy users. It is generally
 * recommended to zero-initialize a `PCB_BuildContext`
 * and pass it to `PCB_BuildContext_init`, because it
 * allows for detecting initialization errors.
 *
 * @param flags PCB_BuildOptions OR'ed together
 * @return an initialized `PCB_BuildContext` struct.
 * Keep in mind that this function may silently fail,
 * which can cause subtle bugs.
 */
PCBAPI PCB_BuildContext PCBCALL PCB_BuildContext_create(int flags);
PCBAPI int PCBCALL PCB_build_fromContext(PCB_BuildContext* context);



/* --------------------------------------------------------------- */
/*--------------------  libc fallbacks  ---------------------------*/
/* --------------------------------------------------------------- */

#ifndef PCB_memcpy
PCBAPI void* PCBCALL PCB_memcpy(
    void* PCB_restrict dest, const void* PCB_restrict src, size_t n
);
#endif //PCB_memcpy unavailable externally

#ifndef PCB_memmove
PCBAPI void* PCBCALL PCB_memmove(void* dest, const void* src, size_t n);
#endif //PCB_memmove unavailable externally

#ifndef PCB_memset
PCBAPI void* PCBCALL PCB_memset(void* dest, int v, size_t n);
#endif //PCB_memset unavailable externally

#ifndef PCB_memcmp
PCBAPI int PCBCALL PCB_memcmp(const void* p1, const void* p2, size_t n);
#endif //PCB_memcmp

#ifndef PCB_strcmp
PCBAPI int PCBCALL PCB_strcmp(const char *s1, const char *s2);
#endif //PCB_strcmp

#ifndef PCB_strncmp
PCBAPI int PCBCALL PCB_strncmp(const char *s1, const char *s2, size_t n);
#endif //PCB_strncmp

#ifndef PCB_strncasecmp
PCBAPI int PCBCALL PCB_strncasecmp(const char *s1, const char *s2, size_t n);
#endif //PCB_strncasecmp

#ifndef PCB_strlen
PCBAPI size_t PCBCALL PCB_strlen(const char *s);
#endif //PCB_strlen

#ifndef PCB_strnlen
PCBAPI size_t PCBCALL PCB_strnlen(const char *s, size_t n);
#endif //PCB_strnlen

#ifndef PCB_isspace
PCBAPI int PCBCALL PCB_isspace(int ch);
#endif //PCB_isspace

#if !defined(PCB__ASSERT_HANDLED)
PCBAPI PCB_NoReturn void PCBCALL PCB__assert_fail(
    const char* exprStr, const char* file, unsigned int line, const char* func
);
#endif //PCB_HAS_ASSERT_H

#endif //PCB_NO_DECLARATIONS

#ifdef PCB_IMPLEMENTATION

#ifndef PCB_IMPLEMENTATION_LOG
#define PCB_IMPLEMENTATION_LOG
#endif //PCB_IMPLEMENTATION_LOG

#ifndef PCB_IMPLEMENTATION_ERR
#define PCB_IMPLEMENTATION_ERR
#endif //PCB_IMPLEMENTATION_ERR

#ifndef PCB_IMPLEMENTATION_FS
#define PCB_IMPLEMENTATION_FS
#endif //PCB_IMPLEMENTATION_FS

#ifndef PCB_IMPLEMENTATION_STRING
#define PCB_IMPLEMENTATION_STRING
#endif //PCB_IMPLEMENTATION_STRING

#ifndef PCB_IMPLEMENTATION_PROCESS
#define PCB_IMPLEMENTATION_PROCESS
#endif //PCB_IMPLEMENTATION_PROCESS

#ifndef PCB_IMPLEMENTATION_ARENA
#define PCB_IMPLEMENTATION_ARENA
#endif //PCB_IMPLEMENTATION_ARENA

#ifndef PCB_IMPLEMENTATION_BUILD
#define PCB_IMPLEMENTATION_BUILD
#endif //PCB_IMPLEMENTATION_BUILD

#endif //PCB_IMPLEMENTATION

//Section 2: Implementation of various functions

//these functions should be moved somewhere else, for now they're here
#ifdef PCB_IMPLEMENTATION
#ifdef PCB_DEBUG_SELF
#define PCB__logTrace PCB_logTrace
#else
#define PCB__logTrace(...)
#endif //PCB_DEBUG_SELF

#if defined(PCB_DEBUG_SELF) && PCB_DEBUG_SELF+0
#define PCB__logDebug PCB_logDebug
#else
#define PCB__logDebug(...)
#endif //PCB_DEBUG_SELF

#ifndef PCB_HAS_ERRNO_H
int errno_stub = 11;
#endif //PCB_HAS_ERRNO_H
#ifndef PCB_strcmp
int PCB_strcmp(const char* s1, const char* s2) {
    const unsigned char* x1 = (const unsigned char*)s1;
    const unsigned char* x2 = (const unsigned char*)s2;
    while(*x1 && *x1 == *x2) { ++x1; ++x2; }
    return (*x1 > *x2) - (*x1 < *x2);
}
#define PCB_strcmp PCB_strcmp
#endif //PCB_strcmp

#ifndef PCB_strncmp
int PCB_strncmp(const char* s1, const char* s2, size_t n) {
    const unsigned char* x1 = (const unsigned char*)s1;
    const unsigned char* x2 = (const unsigned char*)s2;
    while(n > 0 && *x1 && *x1 == *x2) { ++x1; ++x2; --n; }
    return n == 0 ? 0 : ((*x1 > *x2) - (*x1 < *x2));
}
#define PCB_strncmp PCB_strncmp
#endif //PCB_strncmp

#ifndef PCB_strncasecmp
static int PCB_toupper(int ch) { return (ch >= 'a' && ch <= 'a') ? ch - 'a' - 'A' : ch; }
static int PCB_tolower(int ch) { return (ch >= 'A' && ch <= 'Z') ? ch + 'a' - 'A' : ch; }

//https://stackoverflow.com/questions/7299119/source-code-for-strncasecmp-function
int PCB_strncasecmp(const char* s1, const char* s2, size_t n) {
    if(n == 0) return 0;
    const unsigned char* x1 = (const unsigned char*)s1;
    const unsigned char* x2 = (const unsigned char*)s2;
    while(n > 0 && PCB_tolower(*x1) == PCB_tolower(x2)) { ++x1; ++x2; --n; }
    unsigned char c1 = PCB_tolower(*x1), c2 = PCB_tolower(*x2);
    return n == 0 ? 0 : ((c1 > c2) - (c1 < c2));
}
#define PCB_strncasecmp PCB_strncasecmp
#endif //PCB_strncasecmp

#ifndef PCB_strlen
size_t PCB_strlen(const char* s) {
    const char* cursor = s; while(*cursor++);
    return (size_t)(cursor - s);
}
#define PCB_strlen PCB_strlen
#endif //PCB_strlen

#ifndef PCB_strnlen
size_t PCB_strnlen(const char* s, size_t n) {
    const char* cursor = s;
    while(n > 0 && *cursor) { ++cursor; --n; }
    return (size_t)(cursor - s);
}
#define PCB_strnlen PCB_strnlen
#endif //PCB_strnlen

#ifndef PCB_memcpy
void* PCB_memcpy(void* PCB_restrict dest, const void* PCB_restrict src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while(n > 0) *d++ = *s++, --n;
    return dest;
}
#define PCB_memcpy PCB_memcpy
#endif //PCB_memcpy

#ifndef PCB_memmove
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
#endif //PCB_memmove

#ifndef PCB_memset
void* PCB_memset(void* s, int v, size_t n) {
    char* p = (char*)s;
    while(n-- > 0) *p++ = (char)v;
    return s;
}
#define PCB_memset PCB_memset
#endif //PCB_memset

#ifndef PCB_memcmp
int PCB_memcmp(const void* p1, const void* p2, size_t n) {
    const unsigned char* x1 = (const unsigned char*)p1;
    const unsigned char* x2 = (const unsigned char*)p2;
    while(*x1 == *x2 && n > 0) { ++x1; ++x2; --n; }
    return (*x1 > *x2) - (*x1 < *x2);
}
#define PCB_memcmp PCB_memcmp
#endif //PCB_memcmp

#ifndef PCB_isspace
int PCB_isspace(int ch) {
    switch(ch) {
        case ' ' : case '\t': case '\n':
        case '\r': case '\v': case '\f':
            return 1;
    } return 0;
}
#define PCB_isspace PCB_isspace
#endif //PCB_isspace

#if !defined(PCB__ASSERT_HANDLED)
void PCB__assert_fail(
    const char* exprStr, const char* file, unsigned int line, const char* func
) {
#if defined(PCB_HAS_STDIO_H) && defined(PCB_HAS_STDLIB_H)
    fprintf(stderr, "%s:%d:%s: assertion \"%s\" failed.", file, line, func, exprStr);
    abort();
#else
    (void)exprStr; (void)file; (void)line; (void)func;
#error "PCB Error: Assertions cannot be provided or are not implemented for the current platform."
}
#endif //sources of assertions
#endif //PCB_HAS_ASSERT_H
#endif //PCB_IMPLEMENTATION


#if defined(PCB_IMPLEMENTATION_LOG) || defined(PCB_IMPLEMENTATION_ERR)
#ifndef PCB_fprintf
#error "PCB Error: PCB requires PCB_fprintf defined, but none is available. Perhaps you can't use libc, in which case you need to #define it manually."
#define PCB_fprintf(stream, fmt, ...) //stub
#endif //PCB_fprintf
#ifndef PCB_vfprintf
#define PCB_vfprintf(stream, fmt, args) //stub
#error "PCB Error: PCB requires PCB_vfprintf defined, but none is available. Perhaps you can't use libc, in which case you need to #define it manually."
#endif //PCB_vfprintf

#ifndef PCB_stdout
#define PCB_stdout 0 //stub
#error "PCB Error: PCB requires PCB_stdout, but none is available. Perhaps you can't use libc, in which case you need to #define it manually."
#endif //PCB_stdout
#ifndef PCB_stderr
#define PCB_stderr 0 //stub
#error "PCB Error: PCB requires PCB_stderr, but none is available. Perhaps you can't use libc, in which case you need to #define it manually."
#endif //PCB_stderr
#endif //PCB_IMPLEMENTATION_LOG, PCB_IMPLEMENTATION_ERR

#if defined(PCB_IMPLEMENTATION_LOG)
#ifndef PCB_fflush
#define PCB_fflush(stream) //stub
#error "PCB Error: PCB requires PCB_fflush, but none is available. Perhaps you can't use libc, in which case you need to #define it manually."
#endif //PCB_fflush
#endif //PCB_IMPLEMENTATION_LOG

//Section 2.1: Logging, messages, error handling
#ifdef PCB_IMPLEMENTATION_LOG
void PCB_log(PCB_LogLevel level, const char* fmt, ...) {
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
            PCB_fprintf(stdout, "[");
            SetConsoleTextAttribute(hStderr, 8); PCB_fprintf(stdout, "Trace");
            SetConsoleTextAttribute(hStderr, 0xf); PCB_fprintf(stdout, "]\t");
#else
            PCB_fprintf(stdout, "[\033[38;5;238mTrace\033[0m]\t");
#endif
            break;
        case PCB_LOGLEVEL_DEBUG:
        case PCB_LOGLEVEL_DEBUG_NL:
#if PCB_PLATFORM_WINDOWS
        PCB_fprintf(stdout, "[");
        SetConsoleTextAttribute(hStderr, 0xb); PCB_fprintf(stdout, "Debug");
        SetConsoleTextAttribute(hStderr, 0xf); PCB_fprintf(stdout, "]\t");
#else
        PCB_fprintf(stdout, "[\033[38;5;51mDebug\033[0m]\t");
#endif
        break;
    case PCB_LOGLEVEL_INFO:
    case PCB_LOGLEVEL_INFO_NL:
        PCB_fprintf(stdout, "[Info]\t");
        break;
    case PCB_LOGLEVEL_WARN:
    case PCB_LOGLEVEL_WARN_NL:
#if PCB_PLATFORM_WINDOWS
        PCB_fprintf(stdout, "[");
        SetConsoleTextAttribute(hStderr, 6); PCB_fprintf(stdout, "Warn");
        SetConsoleTextAttribute(hStderr, 0xf); PCB_fprintf(stdout, "]\t");
#else
        PCB_fprintf(stdout, "[\033[38;5;214mWarn\033[0m]\t");
#endif
        break;
    case PCB_LOGLEVEL_ERROR:
    case PCB_LOGLEVEL_ERROR_NL:
#if PCB_PLATFORM_WINDOWS
        PCB_fprintf(stderr, "[");
        SetConsoleTextAttribute(hStderr, 0xc); PCB_fprintf(stderr, "Error");
        SetConsoleTextAttribute(hStderr, 0xf); PCB_fprintf(stderr, "]\t");
#else
        PCB_fprintf(stderr, "[\033[38;5;9mError\033[0m]\t");
#endif
        break;
    case PCB_LOGLEVEL_FATAL:
    case PCB_LOGLEVEL_FATAL_NL:
#if PCB_PLATFORM_WINDOWS
        PCB_fprintf(stderr, "[");
        SetConsoleTextAttribute(hStderr, 4); PCB_fprintf(stderr, "Fatal");
        SetConsoleTextAttribute(hStderr, 0xf); PCB_fprintf(stderr, "]\t");
#else
        PCB_fprintf(stderr, "[\033[1m\033[38;5;1mFatal\033[0m]\t");
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
            PCB_vfprintf(stdout, fmt, args); break;
        case PCB_LOGLEVEL_ERROR: case PCB_LOGLEVEL_ERROR_NL:
        case PCB_LOGLEVEL_FATAL: case PCB_LOGLEVEL_FATAL_NL:
            PCB_vfprintf(stderr, fmt, args); break;
    }
    va_end(args);
    switch(level) {
        case PCB_LOGLEVEL_NONE:  case PCB_LOGLEVEL_TRACE:
        case PCB_LOGLEVEL_DEBUG: case PCB_LOGLEVEL_INFO:
        case PCB_LOGLEVEL_WARN:
            PCB_fprintf(stdout, "\n"); break;
        case PCB_LOGLEVEL_ERROR: case PCB_LOGLEVEL_FATAL:
            PCB_fprintf(stderr, "\n"); break;
        case PCB_LOGLEVEL_NONE_NL:  case PCB_LOGLEVEL_TRACE_NL:
        case PCB_LOGLEVEL_DEBUG_NL: case PCB_LOGLEVEL_INFO_NL:
        case PCB_LOGLEVEL_WARN_NL:  case PCB_LOGLEVEL_ERROR_NL:
        case PCB_LOGLEVEL_FATAL_NL:
            break;
    }
}
#endif //PCB_IMPLEMENTATION_LOG


#ifdef PCB_IMPLEMENTATION_ERR
int PCB_GetError(void) {
#if PCB_PLATFORM_WINDOWS
    if(errno == 0) return (int)GetLastError();
    else return -errno;
#elif PCB_PLATFORM_POSIX
    return -errno;
#else
    return 0; //stub
#endif //platform
}

void PCB_ClearError(void) {
#if PCB_PLATFORM_WINDOWS
    SetLastError(0);
#endif //Windows scheiße
    errno = 0;
}

int PCB_GetErrorString(int errnum, char* buf, size_t bufSize) {
#if PCB_PLATFORM_WINDOWS
    if(errnum < 0) {
#if defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 201112L && defined(__STDC_LIB_EXT1__)
        return -strerror_s(buf, bufSize, -errnum);
#else
        PCB_snprintf(buf, bufSize, "%s", strerror(-errnum));
        return 0;
#endif //C11 shenanigans
    }
    DWORD err = GetLastError(); //preserve last error
    DWORD l = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errnum, 0, buf, (DWORD)bufSize, NULL
    );
    if(l == 0) {
        DWORD newErr = GetLastError();
        SetLastError(err);
        return (int)newErr;
    }
    return 0;
#elif PCB_PLATFORM_POSIX
    //PCB_GetError() maps errno values to their negative
    //counterparts for cross-platformness
    errnum = -errnum;
//this code right here is a very good example of xkcd 927
#ifdef _GNU_SOURCE
    char* errStr = strerror_r(errnum, buf, bufSize);
    if(buf != errStr) PCB_snprintf(buf, bufSize, "%s", errStr);
    return 0;
#elif defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE+0 >= 200112L
    int err = errno; //preserve last error
    int code = strerror_r(errnum, buf, bufSize);
    if(code >= 0) return -code; //glibc >= 2.13
    int newErr = errno; errno = err; return newErr; //glibc < 2.13
#elif defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 201112L && defined(__STDC_LIB_EXT1__)
    return -strerror_s(buf, bufSize, errnum);
#else
    PCB_snprintf(buf, bufSize, "%s", strerror(errnum));
    return 0;
#endif //this is really annoying...
#endif //platform
}

void PCB_logLatestError(const char* fmt, ...) {
    char buf[256] = PCB_ZEROED;
#if PCB_PLATFORM_WINDOWS
    bool addNl = PCB_GetError() <= 0;
#endif //error strings from WinAPI are ended with '\n'
    if(PCB_GetErrorString(
        PCB_GetError(), buf, sizeof(buf)
    ) != 0) {
        PCB_log(PCB_LOGLEVEL_ERROR, "Failed to get error string...");
        return; //wtf are we supposed to do if getting the error string fails...
    }
    PCB_log(PCB_LOGLEVEL_ERROR_NL, "%s", ""); //quick'n'dirty hack
    va_list args;
    va_start(args, fmt);
    PCB_vfprintf(stderr, fmt, args);
    va_end(args);
#if PCB_PLATFORM_WINDOWS
    if(addNl) PCB_fprintf(stderr, ": %s\n", buf);
    else PCB_fprintf(stderr, ": %s", buf);
#else
    PCB_fprintf(stderr, ": %s\n", buf);
#endif
}
#endif //PCB_IMPLEMENTATION_ERR


//Section 2.2: Platform-independent (sort of) filesystem functions
#ifdef PCB_IMPLEMENTATION_FS
bool PCB_mkdir(const char* path) {
#if PCB_PLATFORM_POSIX
    if(mkdir(path, 0775) == -1) {
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
    errno = 0;
    if(!CreateDirectory(path, NULL)) {
        DWORD err = GetLastError();
        if(err == ERROR_ALREADY_EXISTS) return true;
        PCB_logLatestError("Failed to create directory \"%s\"", path);
        return false;
    }
    return true;
#endif //platform
}

int PCB_FS_Exists(const char* path) {
    PCB_FileType type = PCB_FS_GetType(path);
    if(type == PCB_FILETYPE_ERROR) return -1;
    return type != PCB_FILETYPE_NONE;
}

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

    BY_HANDLE_FILE_INFORMATION fileinfo = PCB_ZEROED;
    BOOL b = GetFileInformationByHandle(hFile, &fileinfo);
    CloseHandle(hFile);
    if(!b) return 0;

    SetLastError(0);
    uint64_t modTime = fileinfo.ftLastWriteTime.dwLowDateTime;
    modTime += (uint64_t)(fileinfo.ftLastWriteTime.dwHighDateTime) << 32;
    return modTime;
#elif PCB_PLATFORM_POSIX
    struct stat fileinfo = PCB_ZEROED;
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

bool PCB_FS_ReadEntireFile(const char* path, PCB_String* buf) {
    if(path == NULL || buf == NULL) return false;
    bool success = false;
    FILE* f; int64_t s;
    f = fopen(path, "rb");
    if(f == NULL) return false;
    if(fseek(f, 0, SEEK_END) == -1) {
#if PCB_PLATFORM_WINDOWS
        SetLastError(0);
#endif
        goto end;
    }
#if PCB_PLATFORM_WINDOWS
    s = _ftelli64(f);
#else
    s = ftell(f);
#endif
    if(s == -1) goto end;
    if(fseek(f, 0, SEEK_SET) == -1) {
#if PCB_PLATFORM_WINDOWS
        SetLastError(0);
#endif
        goto end;
    }
    if(!PCB_String_reserve(buf, (size_t)s)) goto end;
    fread(buf->data + buf->length, 1, (size_t)s, f);
    if(ferror(f)) goto end;
    buf->data[buf->length += (size_t)s] = '\0';
    success = true;
    end:
    if(f != NULL) fclose(f);
    return success;
}

#endif //PCB_IMPLEMENTATION_FS


//Section 2.3: Strings, string views, vectors of strings...
#ifdef PCB_IMPLEMENTATION_STRING
void PCB_String_destroy(PCB_String* PCB_restrict str) {
    PCB_CHECK_SELF(str,);
    PCB_Vec_destroy(str);
}

bool PCB_String_reserve(PCB_String* PCB_restrict str, const size_t howMany) {
    PCB_CHECK_SELF(str, false);
    const size_t newSize = str->length + howMany + 1; //'\0'
    if(newSize <= str->capacity) return true;
    size_t newCapacity = str->capacity == 0 ? PCB_VEC_INITIAL_CAPACITY : str->capacity;
    while(newSize > newCapacity) newCapacity *= 2;
    char* newData = (char*)PCB_realloc(str->data, newCapacity);
    if(newData == NULL) return false;
    str->data = newData; str->capacity = newCapacity;
    return true;
}

bool PCB_String_resize(PCB_String* PCB_restrict str, const size_t targetLength) {
    PCB_CHECK_SELF(str, false);
    if(targetLength == str->length) return true;
    else if(targetLength < str->length) {
        str->data[targetLength] = '\0';
        str->length = targetLength;
        return true;
    }
    return PCB_String_reserve(str, targetLength - str->length);
}

bool PCB_String_append(PCB_String* str, const PCB_String* other) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(other == NULL, false);
    if(PCB_String_isEmpty(other)) return true;
    if(!PCB_String_reserve(str, other->length)) return false; //with '\0'
    PCB_memcpy(str->data + str->length, other->data, other->length + 1);
    str->length += other->length;
    return true;
}

bool PCB_String_append_sv(PCB_String* PCB_restrict str, PCB_StringView sv) {
    PCB_CHECK_SELF(str, false);
    if(PCB_String_isEmpty(&sv)) return true;
    char* PCB_maybe_restrict data = sv.data;
    PCB_CHECK(str->data <= data && data <= str->data + str->length, false);
    if(!PCB_String_reserve(str, sv.length)) return false;
    PCB_memcpy(str->data + str->length, data, sv.length);
    str->data[str->length += sv.length] = '\0';
    return true;
}

bool PCB_String_append_cstr(
    PCB_String* PCB_restrict str,
    const char* PCB_maybe_restrict cstr
) {
    if(cstr == NULL) return false;
    size_t len = PCB_strlen(cstr);
    if(!PCB_String_reserve(str, len)) return false; //again, with '\0'
    PCB_memcpy(str->data + str->length, cstr, len + 1);
    str->length += len;
    return true;
}

ssize_t PCB_String_append_cstrs(
    PCB_String* PCB_restrict str, PCB_CStringsView cstrs
) {
    PCB_CHECK_SELF(str, -1);
    if(PCB_Vec_isEmpty(&cstrs)) return 0;
    size_t cstrsLength = 0;
    PCB_Vec_forEach_it(&cstrs, it, const char* const) {
        if(*it == NULL) continue;
        if(str->data <= *it && *it <= str->data + str->length) continue;
        cstrsLength += PCB_strlen(*it);
        if((cstrsLength + str->length) * sizeof(*str->data) > SIZE_MAX/2) return -1;
    }
    if(cstrsLength == 0) return 0;
    if(!PCB_String_reserve(str, cstrsLength)) return -1;
    char* cursor = str->data + str->length;
    ssize_t appended = 0;
    PCB_Vec_forEach_it(&cstrs, it, const char* const) {
        if(*it == NULL) continue;
        if(str->data <= *it && *it <= str->data + str->length) continue;
        size_t l = PCB_strlen(*it);
        PCB_memcpy(cursor, *it, l);
        cursor += l; ++appended;
    }
    str->data[str->length += cstrsLength] = '\0';
    return appended;
}

ssize_t PCB_String_append_cstr_v(PCB_String* PCB_restrict str, ...) {
    PCB_CHECK_SELF(str, -1);
    va_list args;
    size_t argsLength = 0;
    va_start(args, str);
    PCB_VA_forEach_until(args, const char*, NULL, arg) {
        if(str->data <= arg && arg <= str->data + str->length) continue;
        argsLength += PCB_strlen(arg);
        if(argsLength + str->length > SIZE_MAX/2) return -1;
    }
    va_end(args);
    if(argsLength == 0) return 0;

    if(!PCB_String_reserve(str, argsLength)) return -1;

    ssize_t appended = 0;
    char* cursor = str->data + str->length;
    va_start(args, str);
    PCB_VA_forEach_until(args, const char*, NULL, arg) {
        if(str->data <= arg && arg <= str->data + str->length) continue;
        size_t l = PCB_strlen(arg);
        PCB_memcpy(cursor, arg, l);
        cursor += l; ++appended;
    }
    va_end(args);

    str->data[str->length += argsLength] = '\0';
    return appended;
}

bool PCB_String_append_chars(
    PCB_String* PCB_restrict str, const char c, const size_t howManyTimes
) {
    PCB_CHECK_SELF(str, false);
    if(c == '\0') return true;
    if(!PCB_String_reserve(str, howManyTimes)) return false;
    PCB_memset(str->data + str->length, c, howManyTimes);
    str->data[str->length += howManyTimes] = '\0';
    return true;
}

bool PCB_String_appendf(
    PCB_String* PCB_restrict str, const char* PCB_maybe_restrict fmt, ...
) {
#ifdef PCB_HAS_STDIO_H
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(fmt == NULL, false);
    PCB_CHECK(str->data <= fmt && fmt <= str->data + str->length, false);

    va_list args;
    va_start(args, fmt);
    //'\0' is implicitly stored at the end
    const size_t lengthRequired = (size_t)PCB_vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if(!PCB_String_reserve(str, lengthRequired)) return false;

    va_start(args, fmt); //                               '\0'
    PCB_vsnprintf(str->data + str->length, lengthRequired + 1, fmt, args);
    va_end(args);
    str->length += lengthRequired;
    return true;
#else
    (void)str; (void)fmt;
    return false;
#endif //PCB_HAS_STDIO_H
}

bool PCB_String_insert(
    PCB_String* PCB_maybe_restrict str, const PCB_String* PCB_maybe_restrict other,
    size_t position
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(position > str->length, false);
    if(PCB_String_isEmpty(other)) return true; //the latter should be impossible
    PCB_CHECK(str == other || str->data == other->data, false); // <---

    if(!PCB_String_reserve(str, other->length)) return false;
    PCB_memmove(str->data + position + other->length, str->data + position, str->length - position);
    PCB_memcpy(str->data + position, other->data, other->length);
    str->data[str->length += other->length] = '\0';
    return true;
}

bool PCB_String_insert_sv(
    PCB_String* PCB_restrict str,
    PCB_StringView sv,
    size_t position
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(position > str->length, false);
    if(PCB_String_isEmpty(&sv)) return true;
    const char* PCB_maybe_restrict data = sv.data;
    PCB_CHECK(str->data <= data && data <= str->data + str->length, false);

    if(!PCB_String_reserve(str, sv.length)) return false;
    PCB_memmove(str->data + position + sv.length, str->data + position, str->length - position);
    PCB_memcpy(str->data + position, data, sv.length);
    str->data[str->length += sv.length] = '\0';
    return true;
}

bool PCB_String_insert_cstr(
    PCB_String* PCB_restrict str, const char* PCB_maybe_restrict cstr,
    size_t position
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(cstr == NULL, false);
    PCB_CHECK(position > str->length, false);
    PCB_CHECK(str->data <= cstr && cstr <= str->data + str->length, false);

    size_t len = PCB_strlen(cstr);
    if(len == 0) return true;
    if(!PCB_String_reserve(str, len)) return false;
    PCB_memmove(str->data + position + len, str->data + position, str->length - position);
    PCB_memcpy(str->data + position, cstr, len);
    str->data[str->length += len] = '\0';
    return true;
}

ssize_t PCB_String_insert_cstrs(
    PCB_String* PCB_restrict str, PCB_CStringsView cstrs, size_t position
) {
    PCB_CHECK_SELF(str, -1);
    PCB_CHECK(position > str->length, -1);
    if(PCB_Vec_isEmpty(&cstrs)) return 0;

    size_t cstrsLength = 0;
    PCB_Vec_forEach_it(&cstrs, it, const char* const) {
        if(*it == NULL) continue;
        if(str->data <= *it && *it <= str->data + str->length) continue;
        cstrsLength += PCB_strlen(*it);
        if(cstrsLength + str->length > SIZE_MAX/2) return -1;
    }
    if(cstrsLength == 0) return 0;

    if(!PCB_String_reserve(str, cstrsLength)) return -1;
    PCB_memmove(
        str->data + position + cstrsLength,
        str->data + position,
        str->length - position
    );

    char* cursor = str->data + position;
    ssize_t appended = 0;
    PCB_Vec_forEach_it(&cstrs, it, const char* const) {
        if(*it == NULL) continue;
        if(str->data <= *it && *it <= str->data + str->length) continue;
        size_t l = PCB_strlen(*it);
        PCB_memcpy(cursor, *it, l);
        cursor += l; ++appended;
    }
    str->data[str->length += cstrsLength] = '\0';
    return appended;
}

ssize_t PCB_String_insert_cstr_v(
    PCB_String* PCB_restrict str, size_t position, ...
) {
    PCB_CHECK_SELF(str, -1);
    va_list args;
    size_t argsLength = 0;
    va_start(args, position);
    PCB_VA_forEach_until(args, const char*, NULL, arg) {
        if(str->data <= arg && arg <= str->data + str->length) continue;
        argsLength += PCB_strlen(arg);
        if(argsLength + str->length > SIZE_MAX/2) return -1;
    }
    va_end(args);
    if(argsLength == 0) return 0;

    if(!PCB_String_reserve(str, argsLength)) return -1;
    PCB_memmove(
        str->data + position + argsLength,
        str->data + position,
        str->length - position
    );

    ssize_t inserted = 0;
    char* cursor = str->data + position;
    va_start(args, position);
    PCB_VA_forEach_until(args, const char*, NULL, arg) {
        if(str->data <= arg && arg <= str->data + str->length) continue;
        size_t l = PCB_strlen(arg);
        PCB_memcpy(cursor, arg, l);
        cursor += l; ++inserted;
    }
    va_end(args);
    str->data[str->length += argsLength] = '\0';
    return inserted;
}

bool PCB_String_insert_chars(
    PCB_String* PCB_restrict str, const char c,
    size_t howManyTimes, size_t position
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(position > str->length, false); PCB_CHECK(c == '\0', false);
    if(howManyTimes == 0) return true;
    if(!PCB_String_reserve(str, howManyTimes)) return false;
    PCB_memmove(str->data + position + howManyTimes, str->data + position, str->length - position);
    PCB_memset(str->data + position, c, howManyTimes);
    str->data[str->length += howManyTimes] = '\0';
    return true;
}

bool PCB_String_insertf(
    PCB_String* PCB_restrict str, const char* PCB_maybe_restrict fmt,
    size_t position, ...
) {
#ifdef PCB_HAS_STDIO_H
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(position > str->length, false);
    PCB_CHECK(str->data <= fmt && fmt <= str->data + str->length, false);

    va_list args;
    va_start(args, position);
    const size_t lengthRequired = (size_t)PCB_vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if(!PCB_String_reserve(str, lengthRequired)) return false;

    //PCB_vsnprintf will override the last character with '\0', so we need to save it
    char overridden = str->data[position];
    PCB_memmove(str->data + position + lengthRequired, str->data + position, str->length - position);
    va_start(args, position);
    PCB_vsnprintf(str->data + position, lengthRequired + 1, fmt, args);
    va_end(args);
    str->data[position + lengthRequired] = overridden;
    str->data[str->length += lengthRequired] = '\0';
    return true;
#else
    (void)str; (void)fmt; (void)position;
    return false;
#endif //PCB_HAS_STDIO_H
}

bool PCB_String_replace_range(
    PCB_String* PCB_restrict str,
    size_t start,
    size_t length,
    PCB_StringView other
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(start >= str->length, false);
    PCB_CHECK(start + length > str->length, false);
    if(PCB_String_isEmpty(&other)) return true;
    PCB_CHECK(str->data <= other.data && other.data <= str->data + str->length, false);
    char* const after = str->data + start + length;
    if(other.length > length) {
        const size_t diff = other.length - length;
        if(!PCB_String_reserve(str, diff)) return false;
        PCB_memmove(after + diff, after, str->length - (start + length));
        PCB_memcpy(str->data + start, other.data, other.length);
        str->data[str->length += diff] = '\0';
    }
    else if(other.length < length) {
        const size_t diff = length - other.length;
        PCB_memcpy(str->data + start, other.data, other.length);
        PCB_memmove(after - diff, after, str->length - (start + length));
        str->data[str->length -= diff] = '\0';
    } else {
        PCB_memcpy(str->data + start, other.data, length);
    }
    return true;
}

bool PCB_String_replace_range_chars(
    PCB_String* str,
    size_t start,
    size_t length,
    char c
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(start >= str->length, false);
    PCB_CHECK(start + length > str->length, false);
    PCB_CHECK(c == '\0', false);
    PCB_memset(str->data + start, c, length);
    return true;
}

bool PCB_String_remove_range(
    PCB_String* PCB_restrict str, size_t start, size_t length
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(start >= str->length, false);
    PCB_CHECK(start + length > str->length, false);
    if(length == 0) return true; //would cause a redundant memmove
    PCB_memmove(
        str->data + start,
        str->data + start + length,
        str->length - (start + length) + 1 //'\0'
    );
    str->length -= length; return true;
}

bool PCB_String_setSuffix_char(
    PCB_String* PCB_restrict str, const char c
) {
    PCB_CHECK_SELF(str, false);
    if(str->capacity == 0 && !PCB_String_reserve(str, 1)) return false;
    if(str->length == 0) {
        str->data[0] = c; str->data[++str->length] = '\0';
        return true;
    }
    if(str->data[str->length - 1] != c) {
        if(!PCB_String_reserve(str, 1)) return false;
        str->data[str->length] = c;
        str->data[++str->length] = '\0';
    }
    return true;
}

bool PCB_String_truncate_until_char(
    PCB_String* PCB_restrict str, const char c
) {
    PCB_CHECK_SELF(str, false);
    if(PCB_String_isEmpty(str)) return true;
    if(str->data[str->length - 1] == c) return true;
    const char* cursor = str->data + str->length - 1;
    while(cursor != str->data && *cursor != c) { --cursor; }
    if(cursor == str->data) {
        if(*cursor != c) return false;
        str->data[str->length = 1] = '\0';
        return true;
    }
    const size_t newLength = (size_t)(cursor + 1 - str->data);
    str->data[str->length = newLength] = '\0';
    return true;
}

PCB_String PCB_String_clone(const PCB_String* PCB_restrict str) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(PCB_String_isEmpty(str), PCB_ZEROED_T(PCB_String));
    PCB_String s = PCB_ZEROED;
    s.data = (char*)PCB_realloc(NULL, str->length + 1);
    if(s.data == NULL) return s;
    s.length = str->length;
    s.capacity = str->capacity;
    PCB_memcpy(s.data, str->data, s.length + 1);
    return s;
}

int PCB_String_compare(const PCB_String* a, const PCB_String* b) {
    PCB_CHECK_SELF(a, 0); PCB_CHECK_SELF(b, 0);
    // if(a->data == NULL && b->data == NULL) return 0;
    // else if(a->data == NULL) return 1;
    // else if(b->data == NULL) return -1;
    if(a->data == NULL) return b->data != NULL;
    else if(b->data == NULL) return -1;
    return a->length == b->length
        ? PCB_memcmp(a->data, b->data, a->length)
        : PCB_strcmp(a->data, b->data);
}

int PCB_String_compare_ci(const PCB_String* a, const PCB_String* b) {
    PCB_CHECK_SELF(a, 0); PCB_CHECK_SELF(b, 0);
    if(a->data == NULL) return b->data != NULL;
    else if(b->data == NULL) return -1;
    return strncasecmp(a->data, b->data, a->length);
}

int PCB_String_compare_cstr(
    const PCB_String* PCB_restrict a, const char* PCB_restrict b
) {
    PCB_CHECK_SELF(a, 0); PCB_CHECK_SELF(b, 0);
    if(a->data == NULL) return b != NULL;
    else if(b == NULL) return -1;
    return PCB_strcmp(a->data, b);
}

int PCB_String_compare_cstr_ci(
    const PCB_String* PCB_restrict a, const char* PCB_restrict b
) {
    PCB_CHECK_SELF(a, 0); PCB_CHECK_SELF(b, 0);
    if(a->data == NULL) return b != NULL;
    else if(b == NULL) return -1;
    return strncasecmp(a->data, b, a->length);
}

bool PCB_String_eq(const PCB_String* a, const PCB_String* b) {
    PCB_CHECK_SELF(a, false); PCB_CHECK_SELF(b, false);
    if(a->data == NULL || b->data == NULL) return false;
    if(a->length != b->length) return false;
    return PCB_memcmp(a->data, b->data, a->length);
}

bool PCB_String_startsWith(const PCB_String* str, const PCB_String* other) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(other == NULL, false);

    if(str == other) return true;
    if(PCB_String_isEmpty(str) || PCB_String_isEmpty(other)) return false;
    if(other->length > str->length) return false;
    return !PCB_memcmp(str->data, other->data, other->length);
}

bool PCB_String_startsWith_cstr(
    const PCB_String* PCB_restrict str, const char* PCB_restrict other
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(other == NULL, false);

    if(PCB_String_isEmpty(str)) return false;
    const size_t len = PCB_strlen(other);
    if(len > str->length) return false;
    return !PCB_memcmp(str->data, other, len);
}

bool PCB_String_endsWith(const PCB_String* str, const PCB_String* other) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(other == NULL, false);

    if(str == other) return true;
    if(PCB_String_isEmpty(str) || PCB_String_isEmpty(other)) return false;
    if(other->length > str->length) return false;
    return !PCB_memcmp(
        str->data + str->length - other->length,
        other->data, other->length
    );
}

bool PCB_String_endsWith_cstr(
    const PCB_String* PCB_restrict str, const char* PCB_restrict other
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(other == NULL, false);

    if(PCB_String_isEmpty(str)) return false;
    const size_t len = PCB_strlen(other);
    if(len > str->length) return false;
    return !PCB_memcmp(str->data + str->length - len, other, len);
}

void PCB_String_toUpperCase(PCB_String* PCB_restrict str) {
    PCB_CHECK_SELF(str,);
    for(size_t i = 0; i < str->length; i++) {
        if(str->data[i] >= 'a' && str->data[i] <= 'z') {
            str->data[i] -= 'a' - 'A';
        }
    }
}

void PCB_String_toLowerCase(PCB_String* PCB_restrict str) {
    PCB_CHECK_SELF(str,);
    for(size_t i = 0; i < str->length; i++) {
        if(str->data[i] >= 'A' && str->data[i] <= 'Z') {
            str->data[i] += 'a' - 'A';
        }
    }
}

PCB_String PCB_String_toUpperCase_copy(const PCB_String* PCB_restrict str) {
    PCB_CHECK_SELF(str, PCB_ZEROED_T(PCB_String));
    PCB_String copy = PCB_String_clone(str);
    PCB_String_toUpperCase(&copy);
    return copy;
}

PCB_String PCB_String_toLowerCase_copy(const PCB_String* PCB_restrict str) {
    PCB_CHECK_SELF(str, PCB_ZEROED_T(PCB_String));
    PCB_String copy = PCB_String_clone(str);
    PCB_String_toLowerCase(&copy);
    return copy;
}

char PCB_String_pop(PCB_String* PCB_restrict str) {
    PCB_CHECK_SELF(str, 0x15); //NAK, as if "Why are you passing NULL here? I'm disappointed."
    if(str->length == 0) return '\0';
    char c = str->data[str->length - 1];
    str->data[--str->length] = '\0';
    return c;
}

size_t PCB_String_pop_many(
    PCB_String* PCB_restrict str, size_t howMany, char* PCB_restrict out
) {
    PCB_CHECK_SELF(str, 0);
    if(howMany == 0) return 0;
    if(PCB_String_isEmpty(str)) return 0;
    if(howMany > str->length) howMany = str->length;
    if(out != NULL) {
        PCB_CHECK(str->data <= out && out <= str->data + str->length, 0);
        PCB_memcpy(out, str->data + str->length - howMany, howMany + 1);
    }
    str->data[str->length -= howMany] = '\0';
    return howMany;
}

size_t PCB_String_removeSuffix(PCB_String* str, const PCB_String* other) {
    PCB_CHECK_SELF(str, 0);
    PCB_CHECK(other == NULL, 0);
    if(PCB_String_endsWith(str, other))
        str->data[str->length -= other->length] = '\0';
    return str->length;
}

PCB_String PCB_String_from_StringView(const PCB_StringView* sv) {
    if(sv == NULL || sv->data == NULL) return PCB_ZEROED_T(PCB_String);
    size_t capacity = PCB_VEC_INITIAL_CAPACITY;
    while(capacity < (sv->length + 1)) capacity *= 2; //+1 for '\0'
    PCB_String s = PCB_ZEROED;
    s.data = (char*)PCB_realloc(NULL, capacity);
    if(s.data == NULL) return s;
    s.length = sv->length;
    s.capacity = capacity;
    PCB_memcpy(s.data, sv->data, s.length);
    s.data[s.length] = '\0';
    return s;
}

PCB_String PCB_String_from_CStrings(
    const PCB_CStrings* PCB_restrict cstrs,
    const char* PCB_restrict delimiter
) {
    if(cstrs == NULL || cstrs->data == NULL || cstrs->length == 0 || delimiter == NULL)
        return PCB_ZEROED_T(PCB_String);
    size_t totalLength = 0;
    for(size_t i = 0; i < cstrs->length; totalLength += PCB_strlen(cstrs->data[i++]));
    //delimiter isn't placed at the end                  v    '\0'
    totalLength += PCB_strlen(delimiter) * (cstrs->length - 1) + 1;

    PCB_String str = PCB_ZEROED;
    str.data = (char*)PCB_realloc(NULL, totalLength);
    if(str.data == NULL) return str;
    str.length = totalLength - 1; //we don't count the '\0'
    str.capacity = totalLength;

    char* cursor = str.data;
    for(size_t i = 0; i < cstrs->length - 1; i++) {
        const char* current = cstrs->data[i];
        for(; *current; *cursor++ = *current++);  //this is cursed...
        for(current = delimiter; *current; *cursor++ = *current++);
    }

    for(const char* current = cstrs->data[cstrs->length - 1]; *current; *cursor++ = *current++);
    *cursor = '\0';
    return str;
}

PCB_StringView PCB_StringView_substr_n(
    PCB_StringView sv, const PCB_StringView sub, size_t n, size_t start
) {
    PCB_CHECK(PCB_String_isEmpty(&sv),  PCB_ZEROED_T(PCB_StringView));
    PCB_CHECK(PCB_String_isEmpty(&sub), PCB_ZEROED_T(PCB_StringView));
    PCB_CHECK(n == 0, PCB_ZEROED_T(PCB_StringView));
    PCB_CHECK(start > sv.length, PCB_ZEROED_T(PCB_StringView));
    sv.data += start; sv.length -= start;
    PCB_StringView s = sub;
    //TODO: "premature optimization is the root of all evil",
    //this loop would benefit greatly from vectorization
    while(n > 0) {
        while(sv.length > 0 && sv.data[0] != s.data[0]) {
            sv.data++; sv.length--;
        }
        if(sv.length == 0) return PCB_ZEROED_T(PCB_StringView);
        while(s.length > 0 && sv.data[0] == s.data[0]) {
            sv.data++; sv.length--;
            s.data++;  s.length--;
        }
        if(s.length == 0) n -= 1;
        s = sub; //search again
    }
    sv.data -= sub.length;
    sv.length = sub.length;
    return sv;
}

PCB_StringViews PCB_StringView_split(
    PCB_StringView sv,
    PCB_StringView delim
) {
    PCB_StringViews views = PCB_ZEROED;
    PCB_CHECK(PCB_String_isEmpty(&delim), views);

    PCB_StringView cur = PCB_StringView_substr(sv, delim, 0);
    while(cur.data != NULL && cur.length > 0) {
        size_t slice_len = (size_t)(&cur.data[0] - &sv.data[0]);
        PCB_Vec_append(&views, (PCB_CLITERAL(PCB_StringView){ sv.data, slice_len }));
        sv.length -= slice_len + cur.length;
        sv.data   += slice_len + cur.length;
        cur = PCB_StringView_substr(sv, delim, 0);
    }
    if(sv.length > 0) PCB_Vec_append(&views, sv);
    return views;
}

PCB_Strings PCB_StringView_split_copy(
    PCB_StringView sv, PCB_StringView delim
) {
    PCB_Strings strs = PCB_ZEROED;
    PCB_CHECK(PCB_String_isEmpty(&delim), strs);

    PCB_StringView cur = PCB_StringView_substr(sv, delim, 0);
    PCB_String str;
    while(cur.data != NULL && cur.length > 0) {
        str = PCB_ZEROED_T(PCB_String);
        size_t slice_len = (size_t)(&cur.data[0] - &sv.data[0]);
        PCB_String_append_sv(&str, PCB_CLITERAL(PCB_StringView){sv.data, slice_len});
        PCB_Vec_append(&strs, str);
        sv.length -= slice_len + cur.length;
        sv.data   += slice_len + cur.length;
        cur = PCB_StringView_substr(sv, delim, 0);
    }
    if(sv.length > 0) {
        str = PCB_ZEROED_T(PCB_String);
        PCB_String_append_sv(&str, sv);
        PCB_Vec_append(&strs, str);
    }
    return strs;
}

PCB_StringViews PCB_StringView_split_whitespace(PCB_StringView sv) {
    PCB_CHECK(PCB_String_isEmpty(&sv), PCB_ZEROED_T(PCB_StringViews));
    PCB_StringViews views = PCB_ZEROED;
    const char*     cur = NULL;
    while(true) {
        while(sv.length > 0 &&  PCB_isspace(sv.data[0])) { sv.data++; sv.length--; }
        if(sv.length == 0) break;
        cur = sv.data;
        while(sv.length > 0 && !PCB_isspace(sv.data[0])) { sv.data++; sv.length--; }
        PCB_Vec_append(
            &views,
            (PCB_CLITERAL(PCB_StringView){cur, (size_t)(sv.data - cur)})
        );
    }
    return views;
}

PCB_Strings PCB_StringView_split_whitespace_copy(PCB_StringView sv) {
    PCB_CHECK(PCB_String_isEmpty(&sv), PCB_ZEROED_T(PCB_Strings));
    PCB_Strings strs = PCB_ZEROED;
    const char* cur  = NULL;
    PCB_String str;
    while(true) {
        str = PCB_ZEROED_T(PCB_String);
        while(sv.length > 0 &&  PCB_isspace(sv.data[0])) { sv.data++; sv.length--; }
        if(sv.length == 0) break;
        cur = sv.data;
        while(sv.length > 0 && !PCB_isspace(sv.data[0])) { sv.data++; sv.length--; }
        PCB_String_append_sv(&str, PCB_CLITERAL(PCB_StringView){ cur, (size_t)(sv.data - cur) });
        PCB_Vec_append(&strs, str);
    }
    return strs;
}

PCB_StringView PCB_StringView_findCharFrom_n(
    PCB_StringView sv, PCB_StringView accept, size_t n
) {
    PCB_CHECK(PCB_String_isEmpty(&sv),     PCB_ZEROED_T(PCB_StringView));
    PCB_CHECK(PCB_String_isEmpty(&accept), PCB_ZEROED_T(PCB_StringView));
    PCB_CHECK(n == 0,                      PCB_ZEROED_T(PCB_StringView));
    PCB_StringView cur = sv;
    while(true) {
        for(size_t i = 0; i < accept.length; i++) {
            if(*cur.data == accept.data[i]) {
                n--; break;
            }
        }
        if(n == 0) break;
        else {
            cur.data++; cur.length--;
            if(cur.length == 0) return PCB_ZEROED_T(PCB_StringView);
        }
    }
    return cur;
}

PCB_StringView PCB_StringView_findCharNotFrom_n(
    PCB_StringView sv, PCB_StringView accept, size_t n
) {
    PCB_CHECK(PCB_String_isEmpty(&sv),     PCB_ZEROED_T(PCB_StringView));
    PCB_CHECK(PCB_String_isEmpty(&accept), PCB_ZEROED_T(PCB_StringView));
    PCB_CHECK(n == 0,                      PCB_ZEROED_T(PCB_StringView));
    PCB_StringView cur = sv;
    while(true) {
        bool anyOfAccept = false;
        for(size_t i = 0; i < accept.length; i++) {
            if(*cur.data == accept.data[i]) {
                anyOfAccept = true; break;
            }
        }
        if(!anyOfAccept) { --n; }
        if(n == 0) break;
        else {
            cur.data++; cur.length--;
            if(cur.length == 0) return PCB_ZEROED_T(PCB_StringView);
        }
    }
    return cur;
}
#endif //PCB_IMPLEMENTATION_STRING

#if !defined(PCB_NO_INLINE_EXPORTS) || (defined(PCB_NO_INLINE_EXPORTS) && defined(PCB_IMPLEMENTATION_STRING))
PCB_maybe_inline PCB_StringView PCB_StringView_from_String(
    const PCB_String* PCB_restrict str
) {
    PCB_CHECK_SELF(str, PCB_ZEROED_T(PCB_StringView));
    return PCB_View_Vec_A_T(str, PCB_StringView);
}

PCB_maybe_inline PCB_StringView PCB_StringView_from_cstr(
    const char* PCB_restrict str
) {
    PCB_CHECK(str == NULL, PCB_ZEROED_T(PCB_StringView));
    return PCB_CLITERAL(PCB_StringView){ str, PCB_strlen(str) };
}

PCB_maybe_inline bool PCB_String_replace_range_cstr(
    PCB_String* PCB_restrict str,
    size_t start, size_t length,
    const char* PCB_restrict cstr
) { return PCB_String_replace_range(str, start, length, PCB_StringView_from_cstr(cstr)); }

PCB_maybe_inline PCB_StringView PCB_StringView_substr(
    PCB_StringView sv, PCB_StringView sub, size_t start
) { return PCB_StringView_substr_n(sv, sub, 1, start); }

PCB_maybe_inline PCB_StringView PCB_StringView_subcstr(
    PCB_StringView sv, const char* sub, size_t start
) { return PCB_StringView_substr(sv, PCB_StringView_from_cstr(sub), start); }

PCB_maybe_inline PCB_StringView PCB_StringView_subcstr_n(
    PCB_StringView sv, const char* sub, size_t n, size_t start
) { return PCB_StringView_substr_n(sv, PCB_StringView_from_cstr(sub), n, start); }

PCB_maybe_inline PCB_StringView PCB_String_substr(
    const PCB_String* str, const PCB_String* sub, size_t start
) { return PCB_StringView_substr(PCB_StringView_from_String(str), PCB_StringView_from_String(sub), start); }

PCB_maybe_inline PCB_StringView PCB_String_subcstr(
    const PCB_String* str, const char* sub, size_t start
) { return PCB_StringView_subcstr(PCB_StringView_from_String(str), sub, start); }

PCB_maybe_inline PCB_StringView PCB_String_substr_n(
    const PCB_String* str, const PCB_String* sub, size_t n, size_t start
) { return PCB_StringView_substr_n(PCB_StringView_from_String(str), PCB_StringView_from_String(sub), n, start); }

PCB_maybe_inline PCB_StringView PCB_String_subcstr_n(
    const PCB_String* str, const char* sub, size_t n, size_t start
) { return PCB_StringView_subcstr_n(PCB_StringView_from_String(str), sub, n, start); }

/*-----------------split, no copy-----------------*/

PCB_maybe_inline PCB_StringViews PCB_StringView_split_cstr(
    PCB_StringView sv, const char* delim
) {
    return PCB_StringView_split(sv, PCB_StringView_from_cstr(delim));
}

PCB_maybe_inline PCB_StringViews PCB_StringView_split_char(
    PCB_StringView sv, const char delim
) {
    char c[2] = { delim, '\0' };
    return PCB_StringView_split(sv, PCB_StringView_from_cstr(c));
}

PCB_maybe_inline PCB_StringViews PCB_String_split(
    const PCB_String* str, const PCB_String* delim
) {
    return PCB_StringView_split(
        PCB_StringView_from_String(str), PCB_StringView_from_String(delim)
    );
}

PCB_maybe_inline PCB_StringViews PCB_String_split_cstr(
    const PCB_String* str, const char* delim
) {
    return PCB_StringView_split(
        PCB_StringView_from_String(str), PCB_StringView_from_cstr(delim)
    );
}

PCB_maybe_inline PCB_StringViews PCB_String_split_char(
    const PCB_String* str, const char delim
) {
    char c[2] = { delim, '\0' };
    return PCB_StringView_split(
        PCB_StringView_from_String(str), PCB_StringView_from_cstr(c)
    );
}

PCB_maybe_inline PCB_StringViews PCB_String_split_whitespace(
    const PCB_String* str
) {
    return PCB_StringView_split_whitespace(PCB_StringView_from_String(str));
}

/*-----------------split, do copy-----------------*/

PCB_maybe_inline PCB_Strings PCB_StringView_split_cstr_copy(
    PCB_StringView sv, const char* delim
) {
    return PCB_StringView_split_copy(sv, PCB_StringView_from_cstr(delim));
}

PCB_maybe_inline PCB_Strings PCB_StringView_split_char_copy(
    PCB_StringView sv, const char delim
) {
    char c[2] = { delim, '\0' };
    return PCB_StringView_split_copy(sv, PCB_StringView_from_cstr(c));
}

PCB_maybe_inline PCB_Strings PCB_String_split_copy(
    const PCB_String* str, const PCB_String* delim
) {
    return PCB_StringView_split_copy(
        PCB_StringView_from_String(str),
        PCB_StringView_from_String(delim)
    );
}

PCB_maybe_inline PCB_Strings PCB_String_split_cstr_copy(
    const PCB_String* str, const char* delim
) {
    return PCB_StringView_split_copy(
        PCB_StringView_from_String(str), PCB_StringView_from_cstr(delim)
    );
}

PCB_maybe_inline PCB_Strings PCB_String_split_char_copy(
    const PCB_String* str, const char delim
) {
    char c[2] = { delim, '\0' };
    return PCB_StringView_split_copy(
        PCB_StringView_from_String(str), PCB_StringView_from_cstr(c)
    );
}

PCB_maybe_inline PCB_Strings PCB_String_split_whitespace_copy(
    const PCB_String* str
) {
    return PCB_StringView_split_whitespace_copy(PCB_StringView_from_String(str));
}

/* Similarly for `PCB_StringView_findCharFrom_n`. */

PCB_maybe_inline PCB_StringView PCB_StringView_findCharFrom(
    PCB_StringView sv, PCB_StringView accept
) {
    return PCB_StringView_findCharFrom_n(sv, accept, 1);
}

PCB_maybe_inline PCB_StringView PCB_StringView_findCharFrom_cstr(
    PCB_StringView sv, const char* accept
) {
    return PCB_StringView_findCharFrom_n(
        sv, PCB_StringView_from_cstr(accept), 1
    );
}

PCB_maybe_inline PCB_StringView PCB_StringView_findCharFrom_cstr_n(
    PCB_StringView sv, const char* accept, size_t n
) {
    return PCB_StringView_findCharFrom_n(
        sv, PCB_StringView_from_cstr(accept), n
    );
}

PCB_maybe_inline PCB_StringView PCB_String_findCharFrom(
    const PCB_String* str, const PCB_String* accept
) {
    return PCB_StringView_findCharFrom_n(
        PCB_StringView_from_String(str), PCB_StringView_from_String(accept), 1
    );
}

PCB_maybe_inline PCB_StringView PCB_String_findCharFrom_n(
    const PCB_String* str, const PCB_String* accept, size_t n
) {
    return PCB_StringView_findCharFrom_n(
        PCB_StringView_from_String(str), PCB_StringView_from_String(accept), n
    );
}

PCB_maybe_inline PCB_StringView PCB_String_findCharFrom_cstr(
    const PCB_String* str, const char* accept
) {
    return PCB_StringView_findCharFrom_n(
        PCB_StringView_from_String(str), PCB_StringView_from_cstr(accept), 1
    );
}

PCB_maybe_inline PCB_StringView PCB_String_findCharFrom_cstr_n(
    const PCB_String* str, const char* accept, size_t n
) {
    return PCB_StringView_findCharFrom_cstr_n(
        PCB_StringView_from_String(str), accept, n
    );
}

/* Similarly for `PCB_StringView_findCharNotFrom_n`. */

PCB_maybe_inline PCB_StringView PCB_StringView_findCharNotFrom(
    PCB_StringView sv, PCB_StringView accept
) {
    return PCB_StringView_findCharNotFrom_n(sv, accept, 1);
}

PCB_maybe_inline PCB_StringView PCB_StringView_findCharNotFrom_cstr(
    PCB_StringView sv, const char* accept
) {
    return PCB_StringView_findCharNotFrom_n(
        sv, PCB_StringView_from_cstr(accept), 1
    );
}

PCB_maybe_inline PCB_StringView PCB_StringView_findCharNotFrom_cstr_n(
    PCB_StringView sv, const char* accept, size_t n
) {
    return PCB_StringView_findCharNotFrom_n(
        sv, PCB_StringView_from_cstr(accept), n
    );
}

PCB_maybe_inline PCB_StringView PCB_String_findCharNotFrom(
    const PCB_String* str, const PCB_String* accept
) {
    return PCB_StringView_findCharNotFrom_n(
        PCB_StringView_from_String(str), PCB_StringView_from_String(accept), 1
    );
}

PCB_maybe_inline PCB_StringView PCB_String_findCharNotFrom_n(
    const PCB_String* str, const PCB_String* accept, size_t n
) {
    return PCB_StringView_findCharNotFrom_n(
        PCB_StringView_from_String(str), PCB_StringView_from_String(accept), n
    );
}

PCB_maybe_inline PCB_StringView PCB_String_findCharNotFrom_cstr(
    const PCB_String* str, const char* accept
) {
    return PCB_StringView_findCharNotFrom_n(
        PCB_StringView_from_String(str), PCB_StringView_from_cstr(accept), 1
    );
}

PCB_maybe_inline PCB_StringView PCB_String_findCharNotFrom_cstr_n(
    const PCB_String* str, const char* accept, size_t n
) {
    return PCB_StringView_findCharNotFrom_cstr_n(
        PCB_StringView_from_String(str), accept, n
    );
}
#endif //PCB_IMPLEMENTATION_STRING (inline)

//Section 2.4: Platform-independent (sort of) process functions.
#ifdef PCB_IMPLEMENTATION_PROCESS
PCB_Process PCB_Process_self(void) {
    PCB_TODO("PCB_Process_self");
#if PCB_PLATFORM_WINDOWS

#elif PCB_PLATFORM_POSIX

#endif
}

PCB_Process PCB_Process_init(void) {
    PCB_Process p = PCB_ZEROED;
    p.handle = PCB_PROCESS_INVALID_HANDLE;
#if PCB_PLATFORM_WINDOWS
    p.status = STILL_ACTIVE;
#endif //platform
    return p;
}

bool PCB_Process_isValid(const PCB_Process* process) {
#if PCB_PLATFORM_WINDOWS
    return process->handle != INVALID_HANDLE_VALUE;
#elif PCB_PLATFORM_POSIX
    return process->handle > (pid_t)0;
#endif
}

bool PCB_Process_waitForExit(PCB_Process* process) {
#if PCB_PLATFORM_WINDOWS
    DWORD val = WaitForSingleObject(process->handle, INFINITE);
    if(val == WAIT_FAILED) {
        errno = 0; return false;
    } //WAIT_TIMEOUT is impossible since INFINITE is provided as wait time
    PCB_assert(GetExitCodeProcess(process->handle, &process->status));
    PCB_assert(process->status != STILL_ACTIVE);
    return true;
#elif PCB_PLATFORM_POSIX
    pid_t id = -1;
    wait: id = waitpid(process->handle, &process->status, 0);
    if(id == -1) {
        if(errno == EINTR) goto wait;
        return false;
    }
    PCB_assert(id != 0);
    return true;
#endif //platform
}

int PCB_Process_checkExit(PCB_Process* process) {
#if PCB_PLATFORM_WINDOWS
    if(!GetExitCodeProcess(process->handle, &process->status)) {
        errno = 0; return -1;
    };
    return process->status != STILL_ACTIVE;
#elif PCB_PLATFORM_POSIX
    pid_t id = waitpid(process->handle, &process->status, WNOHANG);
    if(id == -1) return -1;
    if(id == 0) return false; //no child changed state
    if(WIFEXITED(process->status) || WIFSIGNALED(process->status)) {
        return true;
    }
    return false;
#endif //platform
}

int PCB_Process_getExitCode(const PCB_Process* process) {
#if PCB_PLATFORM_WINDOWS
    return process->status;
#elif PCB_PLATFORM_POSIX
    if(WIFEXITED(process->status)) return WEXITSTATUS(process->status);
    if(WIFSIGNALED(process->status)) return -WTERMSIG(process->status) - 1;
    return -1;
#endif //platform
}

void PCB_Process_destroy(PCB_Process* process) {
#if PCB_PLATFORM_WINDOWS
    CloseHandle(process->handle);
    process->status = STILL_ACTIVE;
#endif //platform
    process->handle = PCB_PROCESS_INVALID_HANDLE;
}

int PCB_Processes_waitForAny(PCB_Processes* processes) {
    if(processes == NULL) {
#if PCB_PLATFORM_WINDOWS
        SetLastError(0);
#endif
        errno = EFAULT; return -1;
    }
    int status = 0;
    size_t invalid = 0;
    while(true) {
        size_t i = 0;
        for(; i < processes->length; i++) {
            PCB_Process* p = &processes->data[i];
            if(!PCB_Process_isValid(p)) { ++invalid; continue; }
            int check = PCB_Process_checkExit(p);
            if(check == -1) return -1;
            if(check) {
                status = PCB_Process_getExitCode(p);
                PCB_Process_destroy(p);
                return status;
            }
        }
        if(i == processes->length) { //no child exited
            if(invalid == processes->length) { //all entries are invalid
#if PCB_PLATFORM_WINDOWS
                SetLastError(0);
#endif
                errno = EINVAL; return -1;
            } //otherwise yield the time slice to wait
#if PCB_PLATFORM_WINDOWS
            Sleep(0);
#elif PCB_PLATFORM_POSIX
            /* There is no way to yield the time slice
             * under POSIX in a cross-platform way AFAIK,
             * ignoring `sched_yield`, which merely yields the CPU.
             * Therefore, we shall sleep.............
             */
            struct timespec t;
            t.tv_nsec = 20 * 1000 * 1000; t.tv_sec = 0;
            nanosleep(&t, NULL);
#endif //platform
        }
    }
    PCB_Unreachable;
}

//TODO: untested
int PCB_Processes_waitForRange(
    PCB_Processes* PCB_restrict processes,
    size_t start,
    size_t end
) {
    if(processes == NULL) {
#if PCB_PLATFORM_WINDOWS
        SetLastError(0);
#endif
        errno = EFAULT; return -1;
    }
    if(end > processes->length) {
#if PCB_PLATFORM_WINDOWS
        SetLastError(0);
#endif
        errno = EINVAL; return -1;
    }
    int status = 0;
    for(size_t i = start; i < end; i++) {
        PCB_Process* p = &processes->data[i];
        if(!PCB_Process_isValid(p)) continue;
        if(!PCB_Process_waitForExit(p)) {
            //this is an annoying way to signal which entry errored out,
            //but the only one without using any additional structures.
            return -(int)i - 2;
        }
        int exitCode = PCB_Process_getExitCode(p);
        if(exitCode != 0) return (int)(i + 1);
    }
    return status;
}

int PCB_Processes_waitForAll(PCB_Processes* processes) {
    return PCB_Processes_waitForRange(processes, 0, processes->length);
}

//check whether `vfork` is available according to vfork(2)...I love you glibc <3
#if PCB_PLATFORM_POSIX
#ifdef PCB_HAS_VFORK
#error "PCB_HAS_VFORK macro should not be defined prior to this place"
#else
#ifdef __GLIBC__
#if __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 12
//and now, hell begins
#if (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE+0 >= 500) && !(defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE+0 >= 200809L)
#define PCB_TEMP 1
#else
#define PCB_TEMP 0
#endif //temporary convenience macro
#if __GLIBC_MINOR__ == 19
#if PCB_TEMP && defined(_DEFAULT_SOURCE) && defined(_BSD_SOURCE)
#define PCB_HAS_VFORK
#endif //glibc 2.19 check
#elif __GLIBC_MINOR__ > 19
#if PCB_TEMP && defined(_DEFAULT_SOURCE)
#define PCB_HAS_VFORK
#endif //glibc >2.19 check
#else
#if PCB_TEMP && defined(_BSD_SOURCE)
#define PCB_HAS_VFORK
#endif //glibc 2.12-2.18 check
#endif //glibc 2.12+ checks
#undef PCB_TEMP
#else
#if defined(_BSD_SOURCE) || (defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE+0) >= 500)
#define PCB_HAS_VFORK
#endif //glibc check (<2.12)
#endif //glibc checks
#endif //glibc
#endif //PCB_HAS_VFORK
#endif //POSIX-only thing

PCB_Process PCB_ShellCommand_runBg(PCB_ShellCommand* command) {
    if(command == NULL || command->data == NULL || command->length < 1) {
        PCB_log(PCB_LOGLEVEL_ERROR, "Cannot run an empty command");
        PCB_ClearError(); errno = EINVAL;
        return PCB_Process_init();
    }
#if PCB_PLATFORM_WINDOWS
    STARTUPINFO startupinfo   = PCB_ZEROED; startupinfo.cb = sizeof(startupinfo);
    PROCESS_INFORMATION pInfo = PCB_ZEROED;

    PCB_String s = PCB_ZEROED; //Windows wants a flat string instead of char* const*
    //a heuristic prediction to minimize reallocs
    PCB_String_reserve(&s, 8 * command->length);
    for(size_t i = 0; i < command->length; i++) {
        //other whitespace characters are nonsensical inside a shell command...right?
        bool needs_quotes =
            strpbrk(command->data[i], " \t\v") != NULL ||
            command->data[i][0] == '\0'; //""
        if(needs_quotes) PCB_String_append_chars(&s, '"', 1);
        const char* cursor = command->data[i];
        while(*cursor) {
            const char* needs_escaping = strpbrk(cursor, "\"\'\\");
            if(needs_escaping == NULL) {
                PCB_String_append_cstr(&s, cursor);
                break;
            }
            int l = (int)(needs_escaping - cursor);
            PCB_String_appendf(&s, "%.*s\\%c", l, cursor, *needs_escaping);
            cursor += l + 1;
        }
        if(needs_quotes) PCB_String_append_chars(&s, '"', 1);

        PCB_String_append_chars(&s, ' ', 1);
    }
    PCB_String_pop(&s); //remove trailing ' '

    errno = 0;
    BOOL success = CreateProcessA(
        NULL, s.data, NULL, NULL, true, 0, NULL, NULL, &startupinfo, &pInfo
    );
    PCB_String_destroy(&s);
    if(!success) {
        PCB_logLatestError("Failed to create a child process");
        return PCB_Process_init();
    }
    CloseHandle(pInfo.hThread);
    PCB_Process process = PCB_Process_init();
    process.handle = pInfo.hProcess;
    return process;
#elif PCB_PLATFORM_POSIX
    //the caller may depend on the lack of null termination afterwards
    bool hadNullLast = true;
    if(command->data[command->length - 1] != NULL) {
        PCB_ShellCommand_append_arg(command, NULL);
        hadNullLast = false;
    }
    int code = 0;
    PCB_Process child = PCB_Process_init();
#ifdef PCB_HAS_VFORK
    child.handle = vfork();
#else
    //TODO: maybe the exit code is sufficient to pass error codes?
    //checking what error has occured in the child is impossible with fork(2) without some IPC
    int tmpPipe[2] = { -1, -1 };
    ssize_t r = -1;
    if(pipe(tmpPipe) < 0) {
        PCB_logLatestError("Failed to create a temporary pipe");
        code = -errno; goto end;
    }
    if(fcntl(tmpPipe[1], F_SETFD, FD_CLOEXEC) < 0) {
        PCB_logLatestError("Failed to set the temporary pipe to 'close-on-exec'");
        code = -errno; goto end;
    }
    child.handle = fork();
#endif //PCB_HAS_VFORK?
    if(child.handle == -1) {
        PCB_logLatestError("Failed to create a child process");
        code = -errno; goto end;
    }
    else if(child.handle == 0) {
        close(tmpPipe[0]);
        execvp(command->data[0], (char* const*)command->data);
        code = errno;
#ifdef PCB_HAS_VFORK
        PCB_logLatestError("Failed to execute shell command");
#else
        write(tmpPipe[1], &code, sizeof(code));
        close(tmpPipe[1]);
#endif //PCB_HAS_VFORK?
        _exit(255);
    }
#ifndef PCB_HAS_VFORK
    close(tmpPipe[1]); tmpPipe[1] = -1;
repeat:
    r = read(tmpPipe[0], &code, sizeof(code));
    if(r < 0) {
        if(errno == EINTR) goto repeat;
        PCB_Unreachable; //at least it should be...
    } else if(r > 0) {
        errno = code;
        code = -code;
    } //0 means nothing was written and no error has occured
#endif //PCB_HAS_VFORK
end:
#ifndef PCB_HAS_VFORK
    if(tmpPipe[1] >= 0) close(tmpPipe[1]);
    if(tmpPipe[0] >= 0) close(tmpPipe[0]);
#endif //!PCB_HAS_VFORK?
    if(code != 0) {
        if(child.handle > 0) waitpid(child.handle, NULL, 0); //reap the child on error
        child.handle = -code;
    }
    if(!hadNullLast) --command->length;
    return child;
#endif //platform-dependent way of running a shell command
}

int PCB_ShellCommand_runAndWait(PCB_ShellCommand* command) {
    if(command->length < 1) {
        PCB_log(PCB_LOGLEVEL_ERROR, "Cannot run an empty command");
        PCB_ClearError(); errno = EINVAL; return -1;
    }

    PCB_Process process = PCB_ShellCommand_runBg(command);
    if(!PCB_Process_isValid(&process)) return -1;
    if(!PCB_Process_waitForExit(&process)) {
        PCB_logLatestError("Failed to wait for shell command to exit");
        return -1;
    }
    int code = PCB_Process_getExitCode(&process);
    PCB_Process_destroy(&process);
    return code;
}
#endif //PCB_IMPLEMENTATION_PROCESS



//Section 2.5: other platform-independent stuff
#ifdef PCB_IMPLEMENTATION_ARENA
PCB_Arena* PCB_Arena_init(size_t size) {
    if(size == 0) return NULL;
    size_t capacity = 1;
    while(capacity < size) capacity *= 2;
    PCB_Arena_Prefix* arena = (PCB_Arena_Prefix*)PCB_realloc(NULL, capacity + sizeof(*arena));
    if(arena == NULL) {
#if PCB_PLATFORM_WINDOWS
        SetLastError(0);
#endif
        return NULL;
    }
    arena->length = 0;
    arena->capacity = capacity / sizeof(void*);
    arena->next = NULL;
    return (PCB_Arena*)arena;
}

PCB_Arena* PCB_Arena_init_in(void* mem, size_t memsize) {
    if(memsize <= sizeof(PCB_Arena_Prefix)) return NULL;
    PCB_Arena_Prefix* arena = (PCB_Arena_Prefix*)mem;
    arena->length = 0;
    arena->capacity = (memsize - sizeof(PCB_Arena_Prefix)) / sizeof(void*);
    arena->next = NULL;
    return (PCB_Arena*)arena;
}

void* PCB_Arena_alloc(PCB_Arena* arena, size_t size) {
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    //size rounded to a multiple of pointer size
    size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    if(size == 0) return NULL;
    try_alloc:
    if(a->length + (size / sizeof(void*)) > a->capacity) {
        if(a->next != NULL)  {
            a = (PCB_Arena_Prefix*)a->next;
            goto try_alloc;
        }
        size_t capacity = ((PCB_Arena_Prefix*)arena)->capacity;
        while(capacity < size) capacity *= 2;
        a->next = PCB_Arena_init(capacity);
        if(a->next == NULL) return NULL;
        a = (PCB_Arena_Prefix*)a->next;
    }
    void* data = (void*)((char*)a + sizeof(*a) + a->length * sizeof(void*));
    a->length += size / sizeof(void*);
    return data;
}

void PCB_Arena_reset(PCB_Arena* arena) {
    PCB_Arena_Prefix* next = (PCB_Arena_Prefix*)(((PCB_Arena_Prefix*)arena)->next);
    PCB_Arena_Prefix* current = (PCB_Arena_Prefix*)arena;
    while(true) {
        current->length = 0;
        if(next == NULL) break;
        current = next;
        next = (PCB_Arena_Prefix*)next->next;
    }
}

void PCB_Arena_destroy(PCB_Arena* arena) {
    PCB_Arena_Prefix* next = (PCB_Arena_Prefix*)(((PCB_Arena_Prefix*)arena)->next);
    while(true) {
        PCB_free(arena);
        arena = (PCB_Arena*)next;
        if(arena == NULL) break;
        next = (PCB_Arena_Prefix*)next->next;
    }
}

#ifdef PCB_HAS_STDIO_H
char* PCB_Arena_asprintf(PCB_Arena* arena, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const size_t lengthRequired = (size_t)PCB_vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);
    char* text = (char*)PCB_Arena_alloc(arena, lengthRequired);
    if(text == NULL)  return NULL;
    va_start(args, fmt);
    const size_t printedLength = (size_t)PCB_vsnprintf(text, lengthRequired, fmt, args);
    va_end(args); //         '\0'
    PCB_assert(printedLength + 1 == lengthRequired);
    return text;
}
#else
char* PCB_Arena_asprintf(PCB_Arena* arena, const char* fmt, ...) {
    (void)arena; (void)fmt;
    return NULL;
}
#endif //PCB_HAS_STDIO_H?

char* PCB_Arena_strdup(PCB_Arena* arena, const char* str) {
    if(str == NULL) return NULL;
    size_t len = PCB_strlen(str) + 1; // '\0'
    char* text = (char*)PCB_Arena_alloc(arena, len);
    if(text == NULL) return NULL;
    PCB_memcpy(text, str, len);
    return text;
}

char* PCB_Arena_strndup(PCB_Arena* arena, const char* str, size_t n) {
    if(str == NULL) return NULL;
    size_t len = PCB_strnlen(str, n); //           '\0'
    char* text = (char*)PCB_Arena_alloc(arena, len + 1);
    if(text == NULL) return NULL;
    PCB_memcpy(text, str, len);
    text[len] = '\0'; //`str` may not end with '\0'
    return text;
}
#endif //PCB_IMPLEMENTATION_ARENA

//uncategorized functions should be put here
#ifdef PCB_IMPLEMENTATION
size_t PCB_getNumberOfCores(void) {
#if PCB_PLATFORM_WINDOWS
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif PCB_PLATFORM_POSIX
    return (size_t)sysconf(_SC_NPROCESSORS_ONLN);
#else
    return 0;
#endif //platform
}
#endif //PCB_IMPLEMENTATION

//Section 2.6: build capability
#ifdef PCB_IMPLEMENTATION_BUILD
const char* PCB_GetCStandardStr(long standard) {
    switch(standard) {
        case 1L:      return "c89"; //see below why 1
//https://sourceforge.net/p/predef/wiki/Standards
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
        case 199409L: return "iso9899:199409";
#endif //GCC/Clang have this as a flag for C95, dunno about MSVC
        case 199901L: return "c99";
        case 201112L: return "c11";
        case 201710L: return "c17";
#if PCB_COMPILER_GCC && __GNUC__ <= 13 && __GNUC__ >= 9
        case 202000L: return "c2x";
#endif //gcc's "c2x", deprecated in GCC14
        case 202311L: return "c23";
#if   PCB_COMPILER_GCC && __GNUC__ >= 15
        case 202500L: return "c2y";
#elif PCB_COMPILER_CLANG && __clang_major__ >= 19
        case 202400L: return "c2y";
#endif //"c2y"
        default:      return NULL;
    }
    PCB_Unreachable;
}

const char* PCB_GetCppStandardStr(long standard) {
    switch(standard) {
        case 199711L: return "c++98";
        case 201103L: return "c++11"; //there was no C++03 macro definition
        case 201402L: return "c++14";
        case 201703L: return "c++17";
        case 202002L: return "c++20";
        case 202302L: return "c++23";
        default:      return NULL;
    }
    PCB_Unreachable;
}

long PCB_GetCStandardInt(const char* standard) {
    if(standard == NULL) return 0;
    const char* cursor = standard + 1;
    if(standard[0] != 'c') {
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
        if(PCB_strncmp(standard, "iso9899:", 8) != 0) return 0;
        cursor = standard + 8;
#else
        return 0;
#endif //GCC/Clang recognize "-std=iso9899:*", not the case with C++ for some reason
    }
    long v = 0;
    while(*cursor) {
        if(*cursor >= '0' && *cursor <= '9') {
            v = v * 10 + (*cursor - '0'); ++cursor;
        } else return 0; //invalid character
    }
    switch(v) {
        //C89 doesn't have __STDC_VERSION__, but it's a valid
        //"-std=", hence a special value of 1
        case 89:
        case 90:   return 1L; //C89 = ANSI C <=> C90 = ISO C90
        //when `standard` starts with "c"
        case 99:   return 199901L;
        case 11:   return 201112L;
        case 17:   return 201710L;
        case 23:   return 202311L;
        //when `standard` starts with "iso9899:", only for GCC/Clang
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
        case 1990: return 1L;
        case 1999: return 199901L;
        case 2011: return 201112L;
        case 2017:
        case 2018: return 201710L;
#if PCB_COMPILER_GCC
        case 2024: return 202311L;
#endif //for some reason Clang doesn't have "-std=iso9899:2024"
#endif //GCC/Clang recognize "-std=iso9899:*"
        case 199409: return 199409L;
        default: return 0;
    }
    PCB_Unreachable;
}

long PCB_GetCppStandardInt(const char* standard) {
    if(standard == NULL) return 0;
    size_t len = PCB_strlen(standard);
    //no "c++" at the start
    if(len < 3 || PCB_strncmp(standard, "c++", 3) != 0) return 0;
    const char* cursor = standard + 3;
    long v = 0;
    while(*cursor) {
        if(*cursor >= '0' && *cursor <= '9') {
            v = v * 10 + (*cursor - '0'); ++cursor;
        } else return 0; //invalid character
    }
    switch(v) {
        case 98: return 199711L;
        case 11: return 201103L;
        case 14: return 201402L;
        case 17: return 201703L;
        case 20: return 202002L;
        case 23: return 202302L;
        default: return 0;
    }
    PCB_Unreachable;
}

static int PCB__build_file(PCB_BuildContext* context) {
    PCB_log(
        PCB_LOGLEVEL_INFO,
        "In: %s, out: %s",
        context->currentSourcePath.data, context->currentBuildPath.data
    );

    return 0;
}

//TODO: document return values
static int PCB__build_directory(PCB_BuildContext* context) {
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
#endif //PCB_IMPLEMENTATION_BUILD

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
