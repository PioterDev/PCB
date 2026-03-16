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
#define PCB_VERSION_MINOR 6
#endif //PCB_VERSION_MINOR

#ifndef PCB_VERSION_PATCH
#define PCB_VERSION_PATCH 12
#endif //PCB_VERSION_PATCH

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
//Section 1.1: Identify the target platform

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
#define _XOPEN_SOURCE 700
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



//Section 1.3: Identify the target architecture

#ifndef PCB_ARCH
//https://stackoverflow.com/questions/152016/detecting-cpu-architecture-compile-time
//https://sourceforge.net/p/predef/wiki/Architectures/
#if PCB_COMPILER_MSVC
//https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros
#if defined(_M_X64) || defined(_M_AMD64)
#define PCB_ARCH_x86_64 1
#define PCB_ARCH_x64 1
#define PCB_ARCH "x64"
#elif defined(_M_IX86)
#define PCB_ARCH_i386 1
#define PCB_ARCH_x86 1
#define PCB_ARCH "x86"
#elif defined(_M_ARM64)
#define PCB_ARCH_AArch64 1
#define PCB_ARCH_ARM64 1
#define PCB_ARCH "ARM64"
#elif defined(_M_ARM)
#define PCB_ARCH_ARM _M_ARM
#define PCB_ARCH "ARMv?"
#elif defined(_M_ALPHA)
#define PCB_ARCH_Alpha 1
#define PCB_ARCH "Alpha"
#elif defined(_M_PPC)
#define PCB_ARCH_PowerPC 1
#define PCB_ARCH "PowerPC"
#endif //architectures
#elif PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#if defined(__x86_64__) || defined(__x86_64) || \
    defined(__amd64__)  || defined(__amd64)
#define PCB_ARCH_x86_64 1
#define PCB_ARCH_x64 1
#define PCB_ARCH "x86_64"
#elif defined(i386) || defined(__i386) || defined(__i386__)
#define PCB_ARCH_i386 1
#define PCB_ARCH_x86 1
#define PCB_ARCH "i386"
#elif defined(__aarch64__)
#define PCB_ARCH_AArch64 1
#define PCB_ARCH_ARM64 1
#define PCB_ARCH "AArch64"
#elif defined(__arm__)
#if defined(__ARM_ARCH_2__)
#define PCB_ARCH_ARM 2
#define PCB_ARCH_ARMv2 1
#define PCB_ARCH "ARMv2"
#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
#define PCB_ARCH_ARM 3
#define PCB_ARCH_ARMv3 1
#define PCB_ARCH "ARMv3"
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
#define PCB_ARCH_ARM 4
#define PCB_ARCH_ARMv4T 1
#define PCB_ARCH "ARMv4T"
#elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
#define PCB_ARCH_ARM 5
#define PCB_ARCH_ARMv5 1
#define PCB_ARCH "ARMv5"
#elif defined(__ARM_ARCH_5T__) || defined(__ARM_ARCH_5TE__) ||
      defined(__ARM_ARCH_5TEJ__)
#define PCB_ARCH_ARM 5
#define PCB_ARCH_ARMv5T 1
#define PCB_ARCH "ARMv5T"
#elif defined(__ARM_ARCH_6T2__)
#define PCB_ARCH_ARM 6
#define PCB_ARCH_ARMv6T2 1
#define PCB_ARCH "ARMv6T2"
#elif defined(__ARM_ARCH_6__)  || defined(__ARM_ARCH_6J__) || \
      defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || \
      defined(__ARM_ARCH_6ZK__)
#define PCB_ARCH_ARM 6
#define PCB_ARCH_ARMv6 1
#define PCB_ARCH "ARMv6"
#elif defined(__ARM_ARCH_7__)  || defined(__ARM_ARCH_7A__) || \
      defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || \
      defined(__ARM_ARCH_7S__)
#define PCB_ARCH_ARM 7
#if defined(__ARM_ARCH_7A__)
#define PCB_ARCH_ARMv7A 1
#define PCB_ARCH "ARMv7A"
#elif defined(__ARM_ARCH_7R__)
#define PCB_ARCH_ARMv7R 1
#define PCB_ARCH "ARMv7R"
#elif defined(__ARM_ARCH_7M__)
#define PCB_ARCH_ARMv7M 1
#define PCB_ARCH "ARMv7M"
#elif defined(__ARM_ARCH_7S__)
#define PCB_ARCH_ARMv7S 1
#define PCB_ARCH "ARMv7S"
#endif //ARMv7
#endif //different ARM ISAs
#elif defined(__alpha__)
//TODO: Versions
#define PCB_ARCH_Alpha 1
#define PCB_ARCH "Alpha"
#elif defined(__m68k__)
//TODO: Versions
#define PCB_ARCH_M68k 1
#define PCB_ARCH "Motorola 68k"
#elif defined(__mips__) || defined(mips)
//TODO: Versions
#define PCB_ARCH_MIPS 1
#define PCB_ARCH "MIPS"
//What in bloody hell is this?! Are you OK, GNU people?
#elif defined(__powerpc)   || defined(__powerpc__) || defined(__powerpc64__) || \
      defined(__POWERPC__) || defined(__ppc__)     || defined(__ppc64__) || \
      defined(__PPC__)     || defined(__PPC64__)   || \
      defined(_ARCH_PPC)   || defined(_ARCH_PPC64)
//TODO: Versions
#define PCB_ARCH_PowerPC 1
#define PCB_ARCH "PowerPC"
#elif defined(__sparc__)
//TODO: Versions
#define PCB_ARCH_SPARC 1
#define PCB_ARCH "SPARC"
#endif //architectures
#else //TODO: other compilers
#endif //compiler

#ifndef PCB_ARCH_x86_64
#define PCB_ARCH_x86_64 0
#endif //PCB_ARCH_x86_64
#ifndef PCB_ARCH_x64
#define PCB_ARCH_x64 0
#endif //PCB_ARCH_x64
#ifndef PCB_ARCH_i386
#define PCB_ARCH_i386 0
#endif //PCB_ARCH_i386
#ifndef PCB_ARCH_x86
#define PCB_ARCH_x86 0
#endif //PCB_ARCH_x86
#ifndef PCB_ARCH_ARM64
#define PCB_ARCH_ARM64 0
#endif //PCB_ARCH_ARM64
#ifndef PCB_ARCH_AArch64
#define PCB_ARCH_AArch64 0
#endif //PCB_ARCH_AArch64
#ifndef PCB_ARCH_ARM
#define PCB_ARCH_ARM 0
#endif //PCB_ARCH_ARM
#ifndef PCB_ARCH_ARMv2
#define PCB_ARCH_ARMv2 0
#endif //PCB_ARCH_ARMv2
#ifndef PCB_ARCH_ARMv3
#define PCB_ARCH_ARMv3 0
#endif //PCB_ARCH_ARMv3
#ifndef PCB_ARCH_ARMv4T
#define PCB_ARCH_ARMv4T 0
#endif //PCB_ARCH_ARMv4T
#ifndef PCB_ARCH_ARMv5
#define PCB_ARCH_ARMv5 0
#endif //PCB_ARCH_ARMv5
#ifndef PCB_ARCH_ARMv5T
#define PCB_ARCH_ARMv5T 0
#endif //PCB_ARCH_ARMv5T
#ifndef PCB_ARCH_ARMv6T2
#define PCB_ARCH_ARMv6T2 0
#endif //PCB_ARCH_ARMv6T2
#ifndef PCB_ARCH_ARMv6
#define PCB_ARCH_ARMv6 0
#endif //PCB_ARCH_ARMv6
#ifndef PCB_ARCH_ARMv7A
#define PCB_ARCH_ARMv7A 0
#endif //PCB_ARCH_ARMv7A
#ifndef PCB_ARCH_ARMv7R
#define PCB_ARCH_ARMv7R 0
#endif //PCB_ARCH_ARMv7R
#ifndef PCB_ARCH_ARMv7M
#define PCB_ARCH_ARMv7M 0
#endif //PCB_ARCH_ARMv7M
#ifndef PCB_ARCH_ARMv7S
#define PCB_ARCH_ARMv7S 0
#endif //PCB_ARCH_ARMv7S
#ifndef PCB_ARCH_Alpha
#define PCB_ARCH_Alpha 0
#endif //PCB_ARCH_Alpha
#ifndef PCB_ARCH_M68k
#define PCB_ARCH_M68k 0
#endif //PCB_ARCH_M68k
#ifndef PCB_ARCH_MIPS
#define PCB_ARCH_MIPS 0
#endif //PCB_ARCH_MIPS
#ifndef PCB_ARCH_PowerPC
#define PCB_ARCH_PowerPC 0
#endif //PCB_ARCH_PowerPC
#ifndef PCB_ARCH_SPARC
#define PCB_ARCH_SPARC 0
#endif //PCB_ARCH_SPARC

#ifndef PCB_ARCH
#define PCB_ARCH "Unknown"
#endif //PCB_ARCH

#endif //PCB_ARCH



//Section 1.4: Define useful, but often compiler-specific macros

#ifndef PCB_DO_PRAGMA
#if PCB_COMPILER_MSVC
//https://learn.microsoft.com/en-us/cpp/preprocessor/pragma-directives-and-the-pragma-keyword
//MSVC, could you stop being an annoying b*tch FOR FIVE MINUTES!?
#define PCB_DO_PRAGMA(x) __pragma(x)
#else
#define PCB_DO_PRAGMA(x) _Pragma(#x)
#endif //MSVC...
#endif //PCB_DO_PRAGMA

#ifndef PCB_EmitWarning
//https://stackoverflow.com/questions/471935/user-warnings-on-msvc-and-gcc
//https://releases.llvm.org/3.3/tools/clang/docs/UsersManual.html
#if PCB_COMPILER_GCC >= 40800 || PCB_COMPILER_CLANG >= 30300
#define PCB_EmitWarning(w) PCB_DO_PRAGMA(GCC warning w)
#elif PCB_COMPILER_GCC >= 40407 || PCB_COMPILER_MSVC >= 1500
#define PCB_EmitWarning(w) PCB_DO_PRAGMA(message w)
#else
#ifdef PCB_ALLOW_UNKNOWN_PRAGMAS_FOR_USER_WARNINGS
#define PCB_EmitWarning(w) PCB_DO_PRAGMA(user_warn w)
#else
#define PCB_EmitWarning(w)
#endif //opt-in, otherwise won't compile if some idiot treats every single warning as error
#endif //compilers
#endif //PCB_EmitWarning

//Save current diagnostic state.
#ifndef PCB_PushDiagnostics
#if PCB_COMPILER_GCC >= 40600 || PCB_COMPILER_CLANG >= 30100
#define PCB_PushDiagnostics() PCB_DO_PRAGMA(GCC diagnostic push)
#elif PCB_COMPILER_MSVC
#define PCB_PushDiagnostics() PCB_DO_PRAGMA(warning(push))
#else
#define PCB_PushDiagnostics()
#endif //compilers
#endif //PCB_PushDiagnostics

//Disable the specified diagnostic.
//MSVC has a completely different diagnostic naming (of course it does),
//so you still need an additional layer of #ifs for compiler detection.
#ifndef PCB_IgnoreDiagnostic
#if PCB_COMPILER_GCC >= 40600 || PCB_COMPILER_CLANG >= 30100
#define PCB_IgnoreDiagnostic(w) PCB_DO_PRAGMA(GCC diagnostic ignored w)
#elif PCB_COMPILER_MSVC
#define PCB_IgnoreDiagnostic(w) PCB_DO_PRAGMA(warning(disable: w))
#else
#define PCB_IgnoreDiagnostic(w)
#endif //compilers
#endif //PCB_IgnoreDiagnostic

//Elevate the specified warning to an error.
//Same deal as with `PCB_IgnoreDiagnostic`.
#ifndef PCB_ElevateWarning
#if PCB_COMPILER_GCC >= 40600 || PCB_COMPILER_CLANG >= 30100
#define PCB_ElevateWarning(w) PCB_DO_PRAGMA(GCC diagnostic error w)
#elif PCB_COMPILER_MSVC
#define PCB_ElevateWarning(w) PCB_DO_PRAGMA(warning(error: w))
#else
#define PCB_ElevateWarning(w)
#endif //compilers
#endif //PCB_ElevateWarning

//Here would be `GCC diagnostic warning ...`, but MSVC doesn't have an equivalent (of course it doesn't).

//Restore previous diagnostic state.
#ifndef PCB_PopDiagnostics
#if PCB_COMPILER_GCC >= 40600 || PCB_COMPILER_CLANG >= 30100
#define PCB_PopDiagnostics() PCB_DO_PRAGMA(GCC diagnostic pop)
#elif PCB_COMPILER_MSVC
#define PCB_PopDiagnostics() PCB_DO_PRAGMA(warning(pop))
#else
#define PCB_PopDiagnostics()
#endif //compilers
#endif //PCB_PopDiagnostics


#ifndef PCB_Nodiscard
#if (defined(__cplusplus) && __cplusplus+0 >= 201703L) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 202311L)
#define PCB_Nodiscard [[nodiscard]]
#elif PCB_COMPILER_GCC >= 30406 || PCB_COMPILER_CLANG >= 40000
#define PCB_Nodiscard __attribute__((warn_unused_result))
#else
#define PCB_Nodiscard
#endif //(C++17+ || C23+) || (GCC >= 3.4.6 || Clang >= 4)
#endif //PCB_Nodiscard

//`reason` must be a string literal.
#ifndef PCB_NodiscardReason
#if (defined(__cplusplus) && __cplusplus+0 >= 201703L) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 202311L)
#define PCB_NodiscardReason(reason) [[nodiscard(reason)]]
#elif PCB_COMPILER_GCC >= 30406 || PCB_COMPILER_CLANG >= 40000
#define PCB_NodiscardReason(reason) __attribute__((warn_unused_result))
#else
#define PCB_NodiscardReason(reason)
#endif //(C++17+ || C23+) || (GCC >= 3.4.6 || Clang >= 4)
#endif //PCB_NodiscardReason

#ifndef PCB_Deprecated
#if (defined(__cplusplus) && __cplusplus+0 >= 201402L) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 202311L)
#define PCB_Deprecated [[deprecated]]
#elif PCB_COMPILER_GCC >= 30101 || PCB_COMPILER_CLANG >= 30100
#define PCB_Deprecated __attribute__((deprecated))
#elif PCB_COMPILER_MSVC
#define PCB_Deprecated __declspec(deprecated)
#else
#define PCB_Deprecated PCB_EmitWarning("Deprecated")
#endif //(C++14+ || C23+) || (GCC >= 3.1.1 || Clang >= 3.1) || MSVC
#endif //PCB_Deprecated

//`reason` must be a string literal.
#ifndef PCB_DeprecatedReason
#if (defined(__cplusplus) && __cplusplus+0 >= 201402L) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 202311L)
#define PCB_DeprecatedReason(reason) [[deprecated(reason)]]
#elif PCB_COMPILER_GCC >= 30101 || PCB_COMPILER_CLANG >= 30100
#define PCB_DeprecatedReason(reason) __attribute__((deprecated(reason)))
#elif PCB_COMPILER_MSVC
#define PCB_DeprecatedReason(reason) __declspec(deprecated(reason))
#else
#define PCB_DeprecatedReason(reason) PCB_EmitWarning("Deprecated:" reason)
#endif //(C++14+ || C23+) || (GCC >= 3.1.1 || Clang >= 3.1) || MSVC
#endif //PCB_DeprecatedReason

#ifndef PCB_Noreturn
#if (defined(__cplusplus) && __cplusplus+0 >= 201103L) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 202311L)
#define PCB_Noreturn [[noreturn]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 201112L
#define PCB_Noreturn _Noreturn
#elif PCB_COMPILER_GCC >= 29503
#define PCB_Noreturn __attribute__((noreturn))
#elif PCB_COMPILER_CLANG >= 30100
#define PCB_Noreturn _Noreturn
#elif PCB_COMPILER_MSVC
#define PCB_Noreturn __declspec(noreturn)
#else
PCB_EmitWarning("PCB_Noreturn does not mark function as one that doesn't return")
#define PCB_Noreturn
#endif //(C++11+ || C23+) || (C11+) || compilers
#endif //PCB_Noreturn

#ifndef PCB_Unused
#if (defined(__cplusplus) && __cplusplus+0 >= 201703L) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 202311L)
#define PCB_Unused [[maybe_unused]]
#elif PCB_COMPILER_GCC >= 29503 || PCB_COMPILER_CLANG >= 40000
#define PCB_Unused __attribute__((unused))
#else
#define PCB_Unused
#endif //(C++17+ || C23+) || compilers
       //GCC 2.95.3 || Clang 4.0.0 docs are the oldest versions mentioning this attribute.
#endif //PCB_Unused

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
PCB_EmitWarning("This function will not run before main because the compiler used does not support it") \
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



//Section 1.5: Import libc, unless this macro is defined as 0
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
#if defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 201112L && PCB_HAS_INCLUDE(<uchar.h>)
#include <uchar.h>
#define PCB_HAS_UCHAR_H
#endif //C11's uchar
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

//Section 1.6: Define functions/macros that the library uses from libc.
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

//Section 1.7: Define other useful macros
//Section 1.7.1: General purpose macros
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

#ifndef PCB_defer_varl
/**
 * @brief Set the `var` variable to `val` and jump to `label`.
 */
#define PCB_defer_varl(var, val, label) do { var = val; goto label; } while(0)
#endif //PCB_defer_varl

#ifndef PCB_defer_l
/**
 * @brief Set the `result` variable to `val` and jump to `label`.
 * Parameterized version of `PCB_defer_varl` for `label`.
 */
#define PCB_defer_l(val, label) PCB_defer_varl(result, val, label)
#endif //PCB_defer_l

#ifndef PCB_return_defer_var
/**
 * @brief Set the `var` variable to `val` and jump to `defer`.
 * Parameterized version of `PCB_return_defer` for `var`.
 * See `PCB_defer_varl` for a more generic version.
 */
#define PCB_return_defer_var(var, val) PCB_defer_varl(var, val, defer)
#endif //PCB_return_defer_var

#ifndef PCB_return_defer
/**
 * @brief Set the `result` variable to `val` and jump to `defer`.
 * Used for resource cleanup at function exit.
 *
 * This macro uses `result` as the return variable and `defer` as the final
 * cleanup label by convention.
 * See `PCB_return_defer_var` for a more generic version.
 *
 * You can safely #define this macro in your code to whatever is best suited
 * for you prior to #include'ing the library. The library uses a private
 * version.
 */
#define PCB_return_defer(val) PCB_return_defer_var(result, val)
#endif //PCB_return_defer

//Macro used for ensuring a particular storage type for enums.
#ifndef PCB_Enum
#if defined(__cpluslus) || (defined(__STDC_VERSION__) && __STDC_VERSION__+0 >= 202311L)
#define PCB_Enum(name, type) \
    enum name : type; typedef enum name name; enum name : type
#else
#define PCB_Enum(name, type) \
    typedef type name; enum
#endif
#endif //PCB_Enum

//Section 1.7.2: template<*> struct vector in C let's goooo

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
 * Arguments are reversed compared to, for example, C++'s range-based for loop,
 * to allow skipping `underlyingType` if it can be inferred with `PCB_Typeof`.
 * If you prefer the standard order, you can #define a macro that reverses
 * arguments before passing them here.
 *
 * An example usage is as follows:
 * ```c
 * typedef struct { int* data; size_t length; size_t capacity; } Ints;
 * ...
 * Ints v = PCB_ZEROED;
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
 * `i` is of type `size_t`. For now it's not possible to change that.
 * Adding elements is not allowed as it may invalidate the iterator.
 *
 * Arguments are reversed compared to, for example, C++'s std::views::enumerate,
 * to allow skipping `elemType` if it can be inferred with `PCB_Typeof`.
 * If you prefer the standard order, you can #define a macro that reverses
 * arguments before passing them here.
 *
 * An example usage is as follows:
 * ```c
 * typedef struct {
 *     const char **data;
 *     size_t length, capacity;
 * } CStrings;
 * ...
 * CStrings cstrs = PCB_ZEROED;
 * ...
 * PCB_Vec_enumerate(&cstrs, i, it, iter, const char*) {
 *     printf("%4lu | %s\n", iter.i, *iter.it);
 * }
 * ```
 */
#define PCB_Vec_enumerate(vec, i, it, enumPair, elemType)               \
for(                                                                    \
    struct { size_t i; elemType *it; } enumPair = { 0, (vec)->data };   \
    enumPair.i < (vec)->length; enumPair.i++, enumPair.it++             \
)
#endif //PCB_Typeof?
#endif //PCB_Vec_enumerate

//Section 1.7.3: Macros for C++ compatibility
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


//Section 1.7.4: Macros for views, slices
#ifndef PCB_View_Vec_unchecked
/**
 * @brief Constructs a view on vector `vec` in the range of
 * [`start`, `end`). If `vec->data == NULL` or `start > end` or
 * `end > vec->length`, the behavior is undefined.
 */
#define PCB_View_Vec_unchecked(vec, start, end) \
    { (vec)->data + (start), (size_t)((end) - (start)) }
#endif //PCB_View_Vec_unchecked

#ifndef PCB_View_Arr_unchecked
/**
 * @brief Constructs a view on array (!) `arr` in the range of [`start`, `end`).
 * Used similarly to `PCB_View_Vec_unchecked`, except it's only valid for arrays,
 * like `int arr[16];`.
 * If `start > end`, the behavior is undefined.
 */
#define PCB_View_Arr_unchecked(arr, start, end) \
    { &(arr)[start], (size_t)((end) - (start)) }
#endif //PCB_View_Arr_unchecked

#ifndef PCB_View_Ptr_unchecked
/**
 * @brief Constructs a view on a pointer `ptr` in the range of [`start`, `end`).
 * If `ptr == NULL || start > end` or `end` goes out of bounds,
 * the behavior is undefined.
 */
#define PCB_View_Ptr_unchecked(ptr, start, end) \
    { (ptr) + (start), (size_t)((end) - (start)) }
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
     (size_t)((end) - (start)))                                         \
}
//For those wondering about casts to ssize_t: it's to suppress warnings about
//"unsigned comparison with 0 is always <true/false>" when passing literals
//to `start`/`end`.
//Same deal with casts to size_t, but this time to suppress
//"narrowing conversion", again for literals.
//There is no other way around it.
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
#define PCB_View_Arr(arr, start, end) {                 \
     &(arr)[start],                                     \
    (PCB_assert((ssize_t)(start) <= (ssize_t)(end)),    \
     PCB_assert((ssize_t)(end)   <= PCB_ARRAY_LEN(arr)),\
     (size_t)((end) - (start)))                         \
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
     (size_t)((end) - (start)))                                 \
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



//Section 1.7.5: Other macros
#ifndef PCB_VA_forEach_until
#define PCB_VA_forEach_until(args, argType, end, name)      \
for(                                                        \
    argType name = va_arg((args), argType);                 \
    name != (end);                                          \
    name = va_arg((args), argType)                          \
)
#endif //PCB_VA_forEach_until

//for-each for arrays. See `PCB_Vec_forEach_it` for details.
#ifndef PCB_Arr_forEach_it
#ifdef PCB_Typeof
#define PCB_Arr_forEach_it(arr, itName, ...)        \
for(                                                \
    PCB_Typeof(&(arr)[0]) itName = &(arr)[0];       \
    itName != &(arr)[PCB_ARRAY_LEN(arr)]; itName++  \
)
#else
#define PCB_Arr_forEach_it(arr, itName, underlyingType) \
for(                                                    \
    underlyingType *itName = &(arr)[0];                 \
    itName != &(arr)[PCB_ARRAY_LEN(arr)]; itName++      \
)
#endif //PCB_Typeof?
#endif //PCB_Arr_forEach_it

//Enumeration for arrays. See `PCB_Vec_enumerate` for details.
#ifndef PCB_Arr_enumerate
#ifdef PCB_Typeof
#define PCB_Arr_enumerate(arr, i, it, enumPair, type)                           \
for(                                                                            \
    struct { size_t i; PCB_Typeof(&(arr)[0]) it; } enumPair = { 0, &(arr)[0] }; \
    enumPair.i < PCB_ARRAY_LEN(arr); enumPair.i++, enumPair.it++                \
)
#else
#define PCB_Arr_enumerate(arr, i, it, enumPair, type)               \
for(                                                                \
    struct { size_t i; type *it; } enumPair = { 0, &(arr)[0] };     \
    enumPair.i < PCB_ARRAY_LEN(arr); enumPair.i++, enumPair.it++    \
)
#endif //PCB_Typeof?
#endif //PCB_Arr_enumerate

//Section 1.8: Import platform-specific header files
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

#ifdef __SANITIZE_ADDRESS__
#include <sanitizer/asan_interface.h>
#endif //ASan



//Section 2: declarations of library structures, enums, functions, etc.
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
    PCB_LOGLEVEL_NONE,  PCB_LOGLEVEL_NONE_NL,  //without the prefix
    PCB_LOGLEVEL_TRACE, PCB_LOGLEVEL_TRACE_NL,
    PCB_LOGLEVEL_DEBUG, PCB_LOGLEVEL_DEBUG_NL,
    PCB_LOGLEVEL_INFO,  PCB_LOGLEVEL_INFO_NL,
    PCB_LOGLEVEL_WARN,  PCB_LOGLEVEL_WARN_NL,
    PCB_LOGLEVEL_ERROR, PCB_LOGLEVEL_ERROR_NL,
    PCB_LOGLEVEL_FATAL, PCB_LOGLEVEL_FATAL_NL
    //With '\n'         Without '\n'
} PCB_LogLevel;

/**
 * @brief "ANSI Escape seQuences Available" (don't Question).
 */
PCB_Enum(PCB_Aeqa, uint8_t) {
    PCB_AEQA_DUNNO = 0,  //yet to be checked
    PCB_AEQA_YES,        //available
    PCB_AEQA_NO,         //unavailable
    PCB_AEQA_MAYBE,      //maybe available, it's up to the user of this enum to decide
    PCB_AEQA_BAD_HANDLE, //the underlying file handle is invalid, most likely closed
    PCB_AEQA_OTHER_ERR   //an error has occured while checking for availability
};


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

#ifndef PCB_FS_DIR_DELIM
#if PCB_PLATFORM_WINDOWS
#define PCB_FS_DIR_DELIM '\\'
#elif PCB_PLATFORM_POSIX
#define PCB_FS_DIR_DELIM '/'
#else
#define PCB_FS_DIR_DELIM 0x15 //stub
#endif //platforms
#endif //PCB_FS_DIR_DELIM

//NOTE: Zero-initialization of this structure is non-portable.
//ALWAYS use `PCB_File_init`.
typedef struct {
#if PCB_PLATFORM_WINDOWS
    HANDLE handle;
#elif PCB_PLATFORM_POSIX
    int handle;
#else
    int handle; //stub
#endif //platform
} PCB_File;

/**
 * @brief Initializer for `PCB_File`.
 * MUST be used instead of standard zero-initialization, otherwise
 * the behavior is incorrect.
 */
#ifndef PCB_File_init
#if PCB_PLATFORM_WINDOWS
#define PCB_File_init() PCB_CLITERAL(PCB_File){ INVALID_HANDLE_VALUE }
#elif PCB_PLATFORM_POSIX
#define PCB_File_init() PCB_CLITERAL(PCB_File){ -1 }
#else
#define PCB_File_init() PCB_ZEROED_T(PCB_File)
#endif //platforms
#endif //PCB_File_init

#ifndef PCB_File_isValid
#if PCB_PLATFORM_WINDOWS
#define PCB_File_isValid(f) ((f).handle != INVALID_HANDLE_VALUE)
#elif PCB_PLATFORM_POSIX
#define PCB_File_isValid(f) ((f).handle >= 0)
#else
#define PCB_File_isValid(f) false //stub
#endif //platforms
#endif //PCB_File_isValid



/**
 * @brief A dynamic array of characters - a string.
 * Has a concrete implementation unlike other dynamic arrays.
 * `PCB_String_*` functions ensure it ends with trailing null byte for
 * compatibility with null-terminated strings.
 * The trailing null byte is not included in its length.
 * Inserting null bytes in the middle is currently disallowed.
 * This behavior may change in future releases.
 */
typedef struct {
    char* data;
    size_t length; //in bytes, ignores variable length encoding of UTF-8
    size_t capacity;
} PCB_String;

/**
 * @brief A non-owning view at a portion of some string.
 * Likely does not end with a null byte, keep that in mind when passing
 * `data` to a function expecting a null-terminated string.
 */
typedef struct {
    const char* data;
    size_t length;
} PCB_StringView;

typedef struct {
    char* data;
    size_t length;
} PCB_StringSlice;

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



/**
 * Definitions of character types that work in C99+ and C++11+.
 * As you can see, it is rather convoluted.
 *
 * This is because C++11+ defines `char(16|32)_t` as distinct types to their
 * integer counterparts, but not `char8_t`...not until C++20...if it even
 * defines them, which we need to check with a FTM.
 *
 * In C, however, all 3 are `typedef`'d to integer types...if uchar.h is
 * available (which it's not for some platforms).
 *
 * These provide a uniform interface in all versions of both C and C++ and
 * allow passing appropiate string literals (for example u"foo") without casts.
 */
#if defined(__cplusplus)
#if defined(__cpp_char8_t) && __cpp_char8_t+0 >= 201811L
typedef char8_t PCB_char8;
#else
typedef unsigned char PCB_char8;
#endif //__cpp_char8_t FTM
#elif defined(__STDC_VERSION__)
//TODO: MinGW doesn't provide char8_t in C23. Is this a bug?
#if __STDC_VERSION__+0 >= 202311L && defined(PCB_HAS_UCHAR_H)
typedef char8_t PCB_char8;
#else
typedef unsigned char PCB_char8;
#endif //C23 && <uchar.h>
#endif //language

#if defined(__cplusplus)
#if defined(__cpp_unicode_characters) && __cpp_unicode_characters+0 >= 200704L
typedef char16_t PCB_char16;
#else
typedef uint_least16_t PCB_char16;
#endif //__cpp_unicode_characters FTM
#elif defined(__STDC_VERSION__)
#if __STDC_VERSION__+0 >= 201112L && defined(PCB_HAS_UCHAR_H)
typedef char16_t PCB_char16;
#else
typedef uint_least16_t PCB_char16;
#endif //C11?
#endif //language

#if defined(__cplusplus)
#if defined(__cpp_unicode_characters) && __cpp_unicode_characters+0 >= 200704L
typedef char32_t PCB_char32;
#else
typedef uint_least32_t PCB_char32;
#endif //__cpp_unicode_characters FTM
#elif defined(__STDC_VERSION__)
#if __STDC_VERSION__+0 >= 201112L && defined(PCB_HAS_UCHAR_H)
typedef char32_t PCB_char32;
#else
typedef uint_least32_t PCB_char32;
#endif //C11?
#endif //language

typedef struct { wchar_t*    data; size_t length; size_t capacity; } PCB_WString;
typedef struct { PCB_char8*  data; size_t length; size_t capacity; } PCB_U8String;
typedef struct { PCB_char16* data; size_t length; size_t capacity; } PCB_U16String;
typedef struct { PCB_char32* data; size_t length; size_t capacity; } PCB_U32String;

typedef struct { const wchar_t*    data; size_t length; } PCB_WStringView;
typedef struct { const PCB_char8*  data; size_t length; } PCB_U8StringView;
typedef struct { const PCB_char16* data; size_t length; } PCB_U16StringView;
typedef struct { const PCB_char32* data; size_t length; } PCB_U32StringView;

typedef struct { wchar_t*    data; size_t length; } PCB_WStringSlice;
typedef struct { PCB_char8*  data; size_t length; } PCB_U8StringSlice;
typedef struct { PCB_char16* data; size_t length; } PCB_U16StringSlice;
typedef struct { PCB_char32* data; size_t length; } PCB_U32StringSlice;

typedef struct { PCB_WString*   data; size_t length; size_t capacity; } PCB_WStrings;
typedef struct { PCB_U8String*  data; size_t length; size_t capacity; } PCB_U8Strings;
typedef struct { PCB_U16String* data; size_t length; size_t capacity; } PCB_U16Strings;
typedef struct { PCB_U32String* data; size_t length; size_t capacity; } PCB_U32Strings;

typedef struct { PCB_WStringView*   data; size_t length; size_t capacity; } PCB_WStringViews;
typedef struct { PCB_U8StringView*  data; size_t length; size_t capacity; } PCB_U8StringViews;
typedef struct { PCB_U16StringView* data; size_t length; size_t capacity; } PCB_U16StringViews;
typedef struct { PCB_U32StringView* data; size_t length; size_t capacity; } PCB_U32StringViews;

typedef struct { const wchar_t**    data; size_t length; size_t capacity; } PCB_WCStrings;
typedef struct { const PCB_char8**  data; size_t length; size_t capacity; } PCB_U8CStrings;
typedef struct { const PCB_char16** data; size_t length; size_t capacity; } PCB_U16CStrings;
typedef struct { const PCB_char32** data; size_t length; size_t capacity; } PCB_U32CStrings;

typedef struct { const wchar_t*    const* data; size_t length; } PCB_WCStringsView;
typedef struct { const PCB_char8*  const* data; size_t length; } PCB_U8CStringsView;
typedef struct { const PCB_char16* const* data; size_t length; } PCB_U16CStringsView;
typedef struct { const PCB_char32* const* data; size_t length; } PCB_U32CStringsView;

/**
 * Unicode codepoint.
 * `code` stores the actual scalar value, while `length` is the number of code
 * units to skip to get the next scalar value (using terminology from
 * Unicode ch03§3.9 Unicode Encoding Forms), depending on the encoding used.
 * For example, when decoding the UTF-8 string "ඞ" at byte 0, the structure
 * contains `code = 0xD9E, length = 3`.
 * Functions that return this structure define meaning of its fields on error.
 */
typedef struct {
    int32_t code;
    uint32_t length;
} PCB_Codepoint;



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
typedef struct PCB_Arena_Mark PCB_Arena_Mark;

PCB_Enum(PCB_Arena_Flags, uint32_t) {
    /**
     * @brief Store additional metadata before each allocation.
     *
     * Normally, when freeing, only the last allocation within a node can be
     * actually deallocated.
     *
     * Suppose A() allocates on the arena and calls B().
     * B() then allocates on the arena and calls C().
     * C() also allocates on the arena, frees what it allocated and returns.
     * B() then frees its allocation and returns.
     * A() then frees its allocation.
     *
     * When this flag is not set, memory allocated in A and B will not be deallocated.
     * This is because the arena only tracks the last allocation.
     *
     * With this flag enabled, memory allocated in A and B will become deallocatable
     * with such allocation pattern, thanks to additional metadata for
     * each allocation.
     *
     * This only works with individual allocations in strict LIFO order.
     * If you can't ensure this, it is recommended to use `mark`/`restore`
     * functionality of `PCB_Arena_Mark` instead.
     *
     * In conjuction with the above, the additional metadata enables
     * realloc, which fail without it.
     *
     * This flag can only be changed when the arena is empty. Otherwise,
     * if cleared, subsequent frees/mark restores will leak memory;
     * if set, frees/mark restores will go OOB and trigger UB.
     */
    PCB_ARENA_FLAG_ALLOC_META = 1 << 0
};

typedef struct {
    size_t size; //of the allocation
    //Additional padding of the last allocation, not including this structure.
    //A MSB of 1 marks allocations that have explicit alignment.
    size_t pad;
} PCB_Arena_Alloc_Meta;

/**
 * @brief A prefix of `PCB_Arena` for allocator metadata.
 */
typedef struct {
    size_t length;
    size_t capacity;
    PCB_Arena* next;
    PCB_Arena_Flags flags;
    PCB_Arena_Alloc_Meta last;
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
 * @brief Check whether `f` supports ANSI escape sequences.
 *
 * On Windows >=10, if `f` refers to a console, this function enables
 * virtual terminal processing, turning on support for ANSI escape sequences.
 * There is currently no way to work around this, though this behavior is most
 * likely desired anyway.
 */
PCBAPI PCB_Aeqa PCBCALL PCB_check_aeqa(PCB_File f);
/**
 * @brief Check whether stdout and stderr support ANSI escape sequences.
 * Internally calls `PCB_check_aeqa`.
 *
 * This function is called the first time `PCB_log` is called.
 */
PCBAPI void PCBCALL PCB_check_aeqa_std(PCB_Aeqa aeqa[2]);

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
 * @brief Get the basename (filename) of `path`.
 *
 * If `path` is an empty `PCB_StringView`,
 * the returned `PCB_StringView` points to statically allocated ".".
 * This behavior is equivalent to basename(3), except the buffer is not modified.
 */
PCBAPI PCB_StringView PCBCALL PCB_FS_Basename(PCB_StringView path);
/**
 * @brief Get the dirname (everything except filename and directory separator) of `path`.
 *
 * If `path` is an empty `PCB_StringView` or there is no separator,
 * the returned `PCB_StringView` points to statically allocated ".".
 * This behavior is equivalent to dirname(3), except the buffer is not modified.
 */
PCBAPI PCB_StringView PCBCALL PCB_FS_Dirname(PCB_StringView path);
/**
 * @brief Get the extension of `path`, without ".".
 * If there is none or it's empty ("./foo."), an empty `PCB_StringView` is returned.
 *
 * This function operates purely on strings. For example, if `path` refers to
 * a directory and there is a ".", for example "/etc/grub.d", this function
 * will pick up "d" as the extension. It is the caller's responsibility to
 * first check that `path` refers to a regular file!
 */
PCBAPI PCB_StringView PCBCALL PCB_FS_Extension(PCB_StringView path);
/**
 * @brief Get the extension of `path`, without "."
 * This function differs from `PCB_FS_Extension` in that it assumes `path` is
 * already a basename and is therefore faster. If it's not, the returned
 * `PCB_StringView` may be incorrect. Otherwise the behavior is identical.
 *
 * If you called `PCB_FS_Basename` already, it is better to use this function
 * for performance. If in doubt, use `PCB_FS_Extension`.
 */
PCBAPI PCB_StringView PCBCALL PCB_FS_Extension_base(PCB_StringView path);



/**
 * @brief Get the native handle to the process' standard input.
 */
PCBAPI PCB_File PCBCALL PCB_IO_get_stdin(void);
/**
 * @brief Get the native handle to the process' standard output.
 */
PCBAPI PCB_File PCBCALL PCB_IO_get_stdout(void);
/**
 * @brief Get the native handle to the process' standard error.
 */
PCBAPI PCB_File PCBCALL PCB_IO_get_stderr(void);



//Why exactly is there no standard strlen for these...?
PCBAPI size_t PCBCALL PCB_strlen_char8 (const PCB_char8*  str) PCB_Nonnull_Arg(1);
PCBAPI size_t PCBCALL PCB_strlen_char16(const PCB_char16* str) PCB_Nonnull_Arg(1);
PCBAPI size_t PCBCALL PCB_strlen_char32(const PCB_char32* str) PCB_Nonnull_Arg(1);

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
 * @brief Appends a Unicode `codepoint` to `str`.
 * @return whether the operation succeeded: fails on realloc failure or
 * if `codepoint` is invalid.
 */
PCBAPI bool PCBCALL PCB_String_append_codepoint(
    PCB_String* PCB_restrict str,
    int32_t codepoint
);
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
 * @brief Inserts a Unicode `codepoint` at position `position` into `str`.
 * Inserting 0 is not allowed.
 * @return whether the operation succeeded: fails on realloc failure or
 * if `codepoint` is invalid.
 */
PCBAPI bool PCBCALL PCB_String_insert_codepoint(
    PCB_String* PCB_restrict str,
    int32_t codepoint,
    size_t position
);
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
 * @brief Removes leading whitespace characters from `str`.
 */
PCBAPI void PCBCALL PCB_String_trim_left(
    PCB_String* PCB_restrict str
);
/**
 * @brief Removes trailing whitespace characters from `str`.
 */
PCBAPI void PCBCALL PCB_String_trim_right(
    PCB_String* PCB_restrict str
);
/**
 * @brief Creates a new `PCB_String` from `sv`.
 * @return an initialized `PCB_String` structure or a zeroed out one on failure.
 */
PCBAPI PCB_String PCBCALL PCB_String_from_StringView(PCB_StringView sv);
/**
 * @brief Creates a new `PCB_String` from `cstrs` joined with `delimiter.
 * @return an initialized `PCB_String` structure or a zeroed out one on failure
 */
PCBAPI PCB_String PCBCALL PCB_String_from_CStrings(
    const PCB_CStrings* PCB_restrict cstrs,
    const char* PCB_restrict delimiter
);
/**
 * @brief Removes leading and trailing whitespace characters from `str`.
 */
PCB_maybe_inline void PCBCALL PCB_String_trim(
    PCB_String* PCB_restrict str
);

/**
 * @brief Find `n`th occurence of a substring `sub` in `sv`.
 * `n` cannot be 0.
 * @return non-empty `PCB_StringView` on success or an empty one on invalid
 * arguments or if `sub` was not found.
 */
PCBAPI PCB_StringView PCBCALL PCB_StringView_substr_n(
    PCB_StringView sv,
    const PCB_StringView sub,
    size_t n
);
/**
 * @brief Find `n`th occurence of a substring `sub` in `sv`, searched from the end.
 * `n` cannot be 0.
 * @return non-empty `PCB_StringView` on success or an empty one on invalid
 * arguments or if `sub` was not found.
 */
PCBAPI PCB_StringView PCBCALL PCB_StringView_rsubstr_n(
    PCB_StringView sv,
    const PCB_StringView sub,
    size_t n
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
/**
 * @brief Skip past bytes from `s` at the start of `sv`.
 * @return `sv` moved past `s` or an empty `PCB_StringView` if `sv` didn't
 * start with `s`.
 */
PCBAPI PCB_StringView PCBCALL PCB_StringView_skipPast(
    PCB_StringView sv,
    PCB_StringView s
);
/**
 * @brief Skips past leading whitespace characters from `sv`.
 */
PCBAPI PCB_StringView PCBCALL PCB_StringView_trim_left(PCB_StringView sv);
/**
 * @brief Drops trailing whitespace characters from `sv`.
 */
PCBAPI PCB_StringView PCBCALL PCB_StringView_trim_right(PCB_StringView sv);

/**
 * @brief Get the length of the Unicode codepoint in `sv` from byte (!)
 * at index `index`.
 *
 * ⚠️WARNING⚠️: THIS FUNCTION ONLY CHECKS THE FIRST BYTE; DO NOT USE IT TO
 * VERIFY THAT THE UNDERLYING SEQUENCE IS VALID,
 * USE `PCB_StringView_GetCodepoint` INSTEAD.
 *
 * @return value in range [1, 4*] or 0 if `sv.data[index]` is not a valid
 * first byte in UTF-8 encoding.
 * 0 is also returned if `sv` is empty or `index` goes out of bounds.
 * This is not the case with `PCB_StringView_GetCodepointLength_unchecked`.
 *
 * See RFC 2279/3629 for info about UTF-8 encoding.
 *
 * * - the full theoretical range of [1, 6] is available by #defining
 * `PCB_UTF8_FULL_RANGE`. It is disabled by default in compliance with RFC 3629
 * and is only available if `PCB_UNICODE_CONFORMANT` is not #defined.
 */
PCBAPI uint8_t PCBCALL PCB_StringView_GetCodepointLength(
    PCB_StringView sv,
    size_t index
);
/**
 * @brief Get the length of the Unicode codepoint in `sv` from byte (!)
 * at index `index`.
 *
 * This function is unsafe; only use it when certain that `sv` and `index`
 * are correct. Otherwise the behavior is undefined. You've been warned.
 *
 * ⚠️WARNING⚠️: THIS FUNCTION ONLY CHECKS THE FIRST BYTE; DO NOT USE IT TO
 * VERIFY THAT THE UNDERLYING SEQUENCE IS VALID,
 * USE `PCB_StringView_GetCodepoint_unchecked` INSTEAD.
 *
 * @return same as `PCB_StringView_GetCodepointLength`.
 */
PCBAPI uint8_t PCBCALL PCB_StringView_GetCodepointLength_unchecked(
    PCB_StringView sv,
    size_t index
);
/**
 * @brief Get the Unicode codepoint in `sv` from byte (!) at index `index`.
 * @return `PCB_Codepoint` structure with:
 * - `code` field in range [0, 0x10FFFF*] on success,
 *   -1  if `sv.data[index]` is a continuation byte,
 *   -2  if the sequence is unfinished,
 *   -3  if octets in the sequence would produce a codepoint out of range,
 *   -4  if a non-continuation byte was encountered while decoding a multi-octet codepoint,
 *   -5  if a surrogate was decoded (never returned under `PCB_UNICODE_CONFORMANT`),
 *   -6  if `sv.data[index]` is either `0xC0` or `0xC1`,
 *   -16 if `sv` is empty,
 *   -17 if `index` goes out of bounds,
 *   -18 if `sv.data + sv.length` or `sv.data + index + <decoded UTF-8 length>`
 *   would cause a pointer arithmetic overflow;
 * - `length` field describing how many bytes to skip until the next codepoint
 *   or 0 if `code <= -16` (on an error unrelated to UTF-8 decoding).
 *
 * See RFC 2279/3629 for info about UTF-8 encoding.
 *
 * * - the full theoretical range of [0, 0x7FFF FFFF] is available by #defining
 * `PCB_UTF8_FULL_RANGE`. It is disabled by default in compliance with RFC 3629
 * and is only available if `PCB_UNICODE_CONFORMANT` is not #defined.
 */
PCBAPI PCB_Codepoint PCBCALL PCB_StringView_GetCodepoint(
    PCB_StringView sv,
    size_t index
);
/**
 * @brief Get the Unicode codepoint in `sv` from byte (!) at index `index`.
 *
 * This function is unsafe; only use it when absolutely certain that `sv` and `input`
 * are correct. Otherwise the behavior is undefined. You've been warned.
 * @return same as `PCB_StringView_GetCodepoint`, except the `length` field
 * is only 0 if `sv.data + index + <decoded UTF-8 length>` would cause an overflow.
 */
PCBAPI PCB_Codepoint PCBCALL PCB_StringView_GetCodepoint_unchecked(
    PCB_StringView sv,
    size_t index
);


PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_from_String(
    const PCB_String* PCB_restrict str
);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_from_cstr(
    const char* PCB_restrict str
);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_from_parts(
    const char* PCB_restrict ptr,
    size_t length
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
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_substr   (PCB_StringView    sv,  PCB_StringView    sub);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_subcstr  (PCB_StringView    sv,  const char*       sub);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_subcstr_n(PCB_StringView    sv,  const char*       sub, size_t n);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_substr       (const PCB_String* str, const PCB_String* sub);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_subcstr      (const PCB_String* str, const char*       sub);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_substr_n     (const PCB_String* str, const PCB_String* sub, size_t n);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_subcstr_n    (const PCB_String* str, const char*       sub, size_t n);
/* Similarly for PCB_StringView_rsubstr. */
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_rsubstr   (PCB_StringView    sv,  PCB_StringView    sub);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_rsubcstr  (PCB_StringView    sv,  const char*       sub);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_rsubcstr_n(PCB_StringView    sv,  const char*       sub, size_t n);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_rsubstr       (const PCB_String* str, const PCB_String* sub);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_rsubcstr      (const PCB_String* str, const char*       sub);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_rsubstr_n     (const PCB_String* str, const PCB_String* sub, size_t n);
PCB_maybe_inline PCB_StringView PCBCALL PCB_String_rsubcstr_n    (const PCB_String* str, const char*       sub, size_t n);
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

//@sa `PCB_StringView_skipPast`.
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_skipPast_cstr(PCB_StringView sv, const char* s);
/**
 * @brief Skip past the `n`th occurence of a `sub`string in `sv`.
 * @return whether stuff was skipped.
 *
 * This is a convenience wrapper for `PCB_StringView_substr_n` + pointer arithmetic.
 * You can infer what other functions here do.
 */
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_skipPast_sub_n    (PCB_StringView sv, PCB_StringView sub, size_t n);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_skipPast_sub      (PCB_StringView sv, PCB_StringView sub);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_skipPast_subcstr  (PCB_StringView sv, const char*    sub);
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_skipPast_subcstr_n(PCB_StringView sv, const char*    sub, size_t n);

//Convenience aliases for consistency.
static PCB_ForceInline PCB_StringView PCB_StringView_skipPast_anyCharFrom       (PCB_StringView sv, PCB_StringView accept)           { return PCB_StringView_findCharFrom       (sv, accept); }
static PCB_ForceInline PCB_StringView PCB_StringView_skipPast_anyCharFrom_n     (PCB_StringView sv, PCB_StringView accept, size_t n) { return PCB_StringView_findCharFrom_n     (sv, accept, n); }
static PCB_ForceInline PCB_StringView PCB_StringView_skipPast_anyCharFrom_cstr  (PCB_StringView sv, const char*    accept)           { return PCB_StringView_findCharFrom_cstr  (sv, accept); }
static PCB_ForceInline PCB_StringView PCB_StringView_skipPast_anyCharFrom_cstr_n(PCB_StringView sv, const char*    accept, size_t n) { return PCB_StringView_findCharFrom_cstr_n(sv, accept, n); }

static PCB_ForceInline PCB_StringView PCB_StringView_skipPast_anyCharNotFrom       (PCB_StringView sv, PCB_StringView accept)           { return PCB_StringView_findCharNotFrom       (sv, accept); }
static PCB_ForceInline PCB_StringView PCB_StringView_skipPast_anyCharNotFrom_n     (PCB_StringView sv, PCB_StringView accept, size_t n) { return PCB_StringView_findCharNotFrom_n     (sv, accept, n); }
static PCB_ForceInline PCB_StringView PCB_StringView_skipPast_anyCharNotFrom_cstr  (PCB_StringView sv, const char*    accept)           { return PCB_StringView_findCharNotFrom_cstr  (sv, accept); }
static PCB_ForceInline PCB_StringView PCB_StringView_skipPast_anyCharNotFrom_cstr_n(PCB_StringView sv, const char*    accept, size_t n) { return PCB_StringView_findCharNotFrom_cstr_n(sv, accept, n); }

PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_skipPast_whitespace(PCB_StringView sv);

/**
 * @brief Shrinks `sv` to not include leading and trailing whitespace characters.
 */
PCB_maybe_inline PCB_StringView PCBCALL PCB_StringView_trim(PCB_StringView sv);


/**
 * `PCB_String` is only 1 of 5 string types exported. The interface for
 * the remaining 4 is almost identical.
 * Refer to `PCB_String_*` variants for behavior.
 */
PCBAPI void    PCBCALL PCB_WString_destroy(PCB_WString* PCB_restrict str);
PCBAPI bool    PCBCALL PCB_WString_reserve(PCB_WString* PCB_restrict str, const size_t howMany);
//----------------------------------------------------------------------------
PCBAPI void    PCBCALL PCB_U8String_destroy(PCB_U8String* PCB_restrict str);
PCBAPI bool    PCBCALL PCB_U8String_reserve(PCB_U8String* PCB_restrict str, const size_t howMany);

PCBAPI void    PCBCALL PCB_U16String_destroy(PCB_U16String* PCB_restrict str);
PCBAPI bool    PCBCALL PCB_U16String_reserve(PCB_U16String* PCB_restrict str, const size_t howMany);

PCBAPI void    PCBCALL PCB_U32String_destroy(PCB_U32String* PCB_restrict str);
PCBAPI bool    PCBCALL PCB_U32String_reserve(PCB_U32String* PCB_restrict str, const size_t howMany);

/**
 * @brief Check if `codepoint` is a valid Unicode character.
 */
PCBAPI bool PCBCALL PCB_IsValidUnicode(int32_t codepoint);
/**
 * @brief Get the UTF-8 length of the Unicode `codepoint`.
 *
 * This function is unsafe; only use it when certain that `codepoint` is a
 * valid Unicode character. Otherwise the behavior is undefined.
 * You've been warned.
 * @return value in range [1, 4*].
 *
 * * - the full theoretical range of [1, 6] is available by #defining
 * `PCB_UTF8_FULL_RANGE`. It is disabled by default in compliance with RFC 3629.
 */
PCBAPI uint8_t PCBCALL PCB_GetUTF8Length_unchecked(int32_t codepoint);
/**
 * @brief Get the UTF-8 length of the Unicode `codepoint`.
 * @return value in range [1, 4*] or 0 if `codepoint` is invalid.
 *
 * * - the full theoretical range of [1, 6] is available by #defining
 * `PCB_UTF8_FULL_RANGE`. It is disabled by default in compliance with RFC 3629.
 */
PCBAPI uint8_t PCBCALL PCB_GetUTF8Length(int32_t codepoint);
/**
 * @brief Store UTF-8-encoded `codepoint` in `buf`.
 *
 * This function is unsafe; only use it when certain that `buf` can hold
 * `codepoint` and that `codepoint` is valid. Otherwise the behavior is undefined.
 * You've been warned.
 * @return position after the encoded codepoint
 */
PCBAPI char* PCBCALL PCB_StoreUTF8Codepoint(
    char* buf,
    int32_t codepoint
);



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
 * @brief Creates a new arena allocator with `size` bytes as initial capacity
 * and default configuration.
 * @return a valid pointer to the arena
 * or NULL if `size == 0` or the allocation failed.
 * @sa PCB_Arena_destroy
 */
PCBAPI PCB_Arena* PCBCALL PCB_Arena_init(size_t size);
/**
 * @brief Creates a new arena allocator with `size` bytes as initial capacity.
 * @return see PCB_Arena_init.
 * @sa PCB_Arena_Flags
 *
 * NOTE: This function is subject to change in the near future.
 */
PCBAPI PCB_Arena* PCBCALL PCB_Arena_init_ex(size_t size, PCB_Arena_Flags flags);
/**
 * @brief Initialize a `PCB_Arena` in a chunk of memory pointed to by `mem`
 * and size `memsize` with default configuration.
 * @return a valid pointer to the arena or NULL if `memsize` is insufficient to
 * hold the arena.
 * @sa PCB_Arena_destroy
 */
PCBAPI PCB_Arena* PCBCALL PCB_Arena_init_in(void* mem, size_t memsize);
/**
 * @brief Initialize a `PCB_Arena` in a chunk of memory pointed to by `mem`
 * and size `memsize`.
 * @return see PCB_Arena_init_in.
 * @sa PCB_Arena_Flags
 *
 * NOTE: This function is subject to change in the near future.
 */
PCBAPI PCB_Arena* PCBCALL PCB_Arena_init_in_ex(
    void* mem,
    size_t memsize,
    PCB_Arena_Flags flags
);
/**
 * @brief Allocates `size` bytes in `arena`.
 *
 * The actual number of bytes allocated will be rounded up to `sizeof(void*)`.
 *
 * @return a valid pointer to the allocated buffer aligned to `sizeof(void*)`
 * or NULL if `size == 0` or if allocation failed
 */
PCBAPI void* PCBCALL PCB_Arena_alloc(PCB_Arena* arena, size_t size);
/**
 * @brief Same as `PCB_Arena_alloc`, but clears (zeroes)
 * the buffer before return.
 */
PCBAPI void* PCBCALL PCB_Arena_zalloc(PCB_Arena* arena, size_t size);
/**
 * @brief Allocates `size` bytes in `arena` with at least `alignment` alignment.
 *
 * The actual number of bytes allocated will be rounded up to `sizeof(void*)`.
 *
 * `alignment` MUST be a power of 2 and a multiple of `sizeof(void*)` (1).
 *
 * @return a valid pointer to the allocated buffer aligned to
 * `min(sizeof(void*), alignment)` or NULL if any of the following occurs:
 *
 * - `size == 0 || alignment == 0`,
 *
 * - `alignment` is invalid,
 *
 * - allocation failed.
 *
 * @sa (1) posix_memalign(3)
 */
PCBAPI void* PCBCALL PCB_Arena_aligned_alloc(
    PCB_Arena* arena,
    size_t size,
    size_t alignment
);
/**
 * @brief Same as `PCB_Arena_aligned_alloc`, but zeroes (clears)
 * the buffer before return.
 */
PCBAPI void* PCBCALL PCB_Arena_aligned_zalloc(
    PCB_Arena* arena,
    size_t size,
    size_t alignment
);
/**
 * @brief Allocates the entire contiguous memory block left unallocated in `arena`.
 * If `arena` has a next node, that node is not considered for allocation.
 *
 * A pointer to the allocated memory is stored in `*ptr`
 * and the number of bytes allocated is stored in `*size`.
 * If there is no space left, `*ptr` is set to NULL and `*size` is set to 0.
 *
 * If `ptr == NULL`, memory is not allocated and `*size` is set to the number
 * of bytes that _would_ be allocated, i.e. queries `arena` for how much space
 * is left inside it.
 *
 * @return false if `arena == NULL || size == NULL` (subject to `PCB_SAFETY_CHECKS`),
 * true otherwise.
 */
PCBAPI bool PCBCALL PCB_Arena_alloc_whole(
    PCB_Arena* arena,
    void** ptr,
    size_t* size
);
/**
 * @brief Returns a pointer to the next arena in the internal linked list
 * or NULL if `arena == NULL` or `arena` doesn't have a next node.
 */
PCBAPI PCB_Arena* PCBCALL PCB_Arena_next(PCB_Arena* arena);
/**
 * @brief Get the number of bytes already allocated in `arena`.
 */
PCBAPI size_t PCBCALL PCB_Arena_allocated(PCB_Arena* arena);
/**
 * @brief Get the number of bytes that can be allocated in `arena`.
 */
PCBAPI size_t PCBCALL PCB_Arena_allocatable(PCB_Arena* arena);
/**
 * @brief Get `arena`'s current capacity.
 */
PCBAPI size_t PCBCALL PCB_Arena_capacity(PCB_Arena* arena);
/**
 * @brief Get the number of bytes already allocated in `arena`,
 * including its successor nodes.
 */
PCBAPI size_t PCBCALL PCB_Arena_allocated_all(PCB_Arena* arena);
/**
 * @brief Get the number of bytes that can be allocated in `arena`,
 * including its successor nodes.
 */
PCBAPI size_t PCBCALL PCB_Arena_allocatable_all(PCB_Arena* arena);
/**
 * @brief Get `arena`'s current capacity,
 * including its successor nodes.
 */
PCBAPI size_t PCBCALL PCB_Arena_capacity_all(PCB_Arena* arena);
/**
 * @brief Get `arena`'s flags. Only looks at the head of the internal linked list.
 * @sa PCB_Arena_next
 */
PCBAPI PCB_Arena_Flags PCBCALL PCB_Arena_flags(PCB_Arena* arena);
/**
 * @brief Enables storing additional metadata before each allocation.
 * This can only be done if `arena` is empty, including all of its successors.
 * @return whether successfully `enable`d (`!enable` -> disabled).
 * @sa PCB_Arena_reset
 * @sa PCB_ARENA_FLAG_ALLOC_META
 */
PCBAPI bool PCBCALL PCB_Arena_enable_allocMeta(
    PCB_Arena* arena,
    bool enable
);
/**
 * @brief Create a mark for `arena`.
 *
 * A mark stores information about how much was allocated when this function
 * was called. It is used in conjunction with `PCB_Arena_restore(_to)` to implement
 * a stack allocator. Think of it as `asm("push rbp")`.
 *
 * `mark` is allocated in `arena` and cannot be used in `PCB_Arena_restore(_to)`
 * with a different arena.
 * @return pointer to the mark or NULL if allocation failed.
 * @sa PCB_Arena_alloc
 */
PCBAPI PCB_Arena_Mark* PCBCALL PCB_Arena_mark(PCB_Arena* arena);
/**
 * @brief Restore the state of `arena` from `mark`.
 * `mark` becomes invalid after this function returns `true`.
 * You CANNOT use the same mark multiple times!!
 * @return `true` on success or `false` if `mark == NULL` or `arena`
 * holds less than what is recorded in `mark` (this can happen if you messed
 * up the LIFO order of `PCB_Arena_restore`) or if `mark` was not
 * allocated in `arena`.
 */
PCBAPI bool PCBCALL PCB_Arena_restore(PCB_Arena* arena, PCB_Arena_Mark* mark);
/**
 * @brief Restore the state of `arena` from `mark`.
 * Contrary to `PCB_Arena_restore`, `mark` is preserved and can be used later.
 * This is useful in memory-hungry loops.
 * @return same as `PCB_Arena_restore`
 */
PCBAPI bool PCBCALL PCB_Arena_restore_to(PCB_Arena* arena, PCB_Arena_Mark* mark);
#ifndef PCB_Arena_scope
/**
 * @brief Create an allocation scope for `arena`.
 * The lifetime of objects allocated in `arena` in this scope ends when leaving it.
 *
 * It is defined as a `for` loop that runs exactly once, so you can leave
 * the scope prematurely by `continue`ing (⚠️do NOT however `break`⚠️, otherwise
 * objects won't be deallocated!!!).
 * A helper macro `PCB_Arena_scope_leave` is #defined to reduce the chance to
 * mess this up if needed.
 */
#define PCB_Arena_scope(arena)                                      \
for(                                                                \
    PCB_Arena_Mark* PCB_MANGLE(m) = PCB_Arena_mark(arena);          \
    PCB_MANGLE(m) != NULL;                                          \
    PCB_Arena_restore(arena, PCB_MANGLE(m)), PCB_MANGLE(m) = NULL   \
)
#define PCB_Arena_scope_leave() continue
#endif //PCB_Arena_scope
/**
 * @brief Free `ptr`, which was previously allocated with
 * `PCB_Arena_(c)alloc` or `PCB_Arena_aligned_(c)alloc`, from `arena`.
 * This is a no-op if `ptr` was not the last thing allocated in `arena` or
 * one of its successors or `arena` does not store allocation metadata.
 * The memory is simply leaked.
 * @sa PCB_Arena_enable_allocMeta
 */
PCBAPI void PCBCALL PCB_Arena_free(PCB_Arena* arena, void* ptr);
/**
 * @brief Change the size of an allocation pointed to by `ptr` to `size` bytes.
 * The following list of ifs should be followed from top to bottom.
 *
 * If `ptr == NULL`, the behavior is equivalent to calling `PCB_Arena_alloc`.
 * If `size == 0 && ptr != NULL`, a diagnostic is issued and NULL is returned.
 * ISO C does not define behavior for standard realloc for such inputs.
 * If `arena` does not store allocation metadata, the function fails with NULL.
 * If `size` is equal to the original size, `ptr` is returned without any modifications.
 * If `ptr` is *not* the last thing allocated in `arena` or one of its successors, then:
 * - if `size` is smaller than the original size, `ptr` is returned without
 *   any modifications and the difference is leaked;
 * - otherwise a new buffer of at least size `size` bytes is allocated, data
 *   from `ptr` is copied into the new buffer and `ptr` is leaked.
 * If `size` is greater than the original size and the old buffer cannot be
 * expanded in-place, a new buffer of at least `size` bytes is allocated, data
 * from `ptr` is copied into the new buffer and `ptr` is freed.
 *
 * In the happy path, the allocation is simply expanded/shrunk in-place.
 * In such case `ptr` is returned.
 *
 * After this function returns, `ptr` is considered invalid, unless it itself
 * is returned. It is not recommended to blindly reassign `ptr` to the value
 * returned.
 *
 * If `ptr` was created using `PCB_Arena_aligned_(c)alloc` and a new buffer
 * is created, the new buffer is aligned to at least the original alignment.
 * If `ptr` is not a pointer originally returned by one of the alloc functions,
 * the behavior is undefined.
 *
 * @return NULL for reasons described above or if allocating a new node
 * failed, `ptr` in some described cases, a new pointer otherwise.
 *
 * @note Passing `ptr` which does not belong to `arena` or any of its
 * successors is tolerated; currently NULL is returned.
 * Do not, however, rely on this behavior.
 * Future revisions may issue a diagnostic, abort, or do something else entirely.
 *
 * @sa PCB_Arena_alloc
 * @sa PCB_Arena_aligned_alloc
 * @sa PCB_Arena_free
 * @sa PCB_Arena_enable_allocMeta
 * @sa PCB_ARENA_FLAG_ALLOC_META
 * @sa https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3621.txt
 */
PCBAPI void* PCBCALL PCB_Arena_realloc(
    PCB_Arena* arena,
    void* ptr,
    size_t size
);
/**
 * @brief Resets `arena` as if nothing was allocated.
 */
PCBAPI void PCBCALL PCB_Arena_reset(PCB_Arena* arena);
/**
 * @brief Destroys `arena`, i.e. frees blocks contained within it.
 * After this call, `arena` becomes a dangling pointer!
 *
 * Do not call this function if `arena` was created with `PCB_Arena_init_in`.
 * In that case the caller is responsible for managing the backing store.
 * You still need to call this function on `PCB_Arena_next(arena)`.
 */
PCBAPI void PCBCALL PCB_Arena_destroy(PCB_Arena* arena);
/**
 * @brief `sprintf`s a new string in `arena`, variadic version.
 * @return pointer to the allocated string or NULL on error.
 * @sa PCB_Arena_alloc
 *
 * This function is only available if the library was compiled with stdio.h present.
 * Otherwise it always returns NULL.
 */
PCBAPI char* PCBCALL PCB_Arena_asprintf(
    PCB_Arena* arena,
    const char* fmt,
    ...
) PCB_Printf_Format(2, 3);
/**
 * @brief `sprintf`s a new string in `arena`, argument version.
 * @return pointer to the allocated string or NULL on error.
 * @sa PCB_Arena_alloc
 *
 * This function is only available if the library was compiled with stdio.h present.
 * Otherwise it always returns NULL.
 */
PCBAPI char* PCBCALL PCB_Arena_vasprintf(
    PCB_Arena* arena,
    const char* fmt,
    va_list args
);
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



//Section 3: Implementation

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

#ifndef PCB_IMPLEMENTATION_IO
#define PCB_IMPLEMENTATION_IO
#endif //PCB_IMPLEMENTATION_IO

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

#ifndef PCB_IMPLEMENTATION_LIBC_FALLBACKS
#define PCB_IMPLEMENTATION_LIBC_FALLBACKS
#endif //PCB_IMPLEMENTATION_LIBC_FALLBACKS

#endif //PCB_IMPLEMENTATION

#if defined(PCB_IMPLEMENTATION_LOG) || defined(PCB_IMPLEMENTATION_ERR) || \
    defined(PCB_IMPLEMENTATION_FS)  || defined(PCB_IMPLEMENTATION_IO) || \
    defined(PCB_IMPLEMENTATION_STRING) || \
    defined(PCB_IMPLEMENTATION_PROCESS) || defined(PCB_IMPLEMENTATION_ARENA) || \
    defined(PCB_IMPLEMENTATION_BUILD)
#define PCB_IMPLEMENTATION_ANY
#endif //Any implementation (except libc fallbacks) requested



#ifdef PCB_IMPLEMENTATION_ANY
/* ---------------------------------------------------------------- */
/* ------------------------ Private macros ------------------------ */
/* ---------------------------------------------------------------- */
#define PCB__defer_l(val, label) PCB_defer_varl(result, val, label)
#define PCB__return_defer(val)   PCB_defer_varl(result, val, defer)

#ifdef PCB_DEBUG_SELF
#define PCB__logTrace(...) PCB_logTrace(__VA_ARGS__)
#else
#define PCB__logTrace(...)
#endif //PCB_DEBUG_SELF

#if defined(PCB_DEBUG_SELF) && PCB_DEBUG_SELF+0
#define PCB__logDebug(...) PCB_logDebug(__VA_ARGS__)
#else
#define PCB__logDebug(...)
#endif //PCB_DEBUG_SELF

#endif //PCB_IMPLEMENTATION_ANY

#ifdef PCB_IMPLEMENTATION_LIBC_FALLBACKS
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
#endif //PCB_IMPLEMENTATION_LIBC_FALLBACKS


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

//Section 3.1: Logging, messages, error handling
#ifdef PCB_IMPLEMENTATION_LOG
PCB_Aeqa PCB_check_aeqa(PCB_File f) {
    if(!PCB_File_isValid(f)) return PCB_AEQA_BAD_HANDLE;
#if PCB_PLATFORM_WINDOWS
    //As of 2025-02-15, Wine doesn't support virtual terminal processing.
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (ntdll != NULL && GetProcAddress(ntdll, "wine_get_version") != NULL)
        return PCB_AEQA_NO;
    DWORD dw = GetFileType(f.handle);
    if(dw != FILE_TYPE_CHAR) { //TODO: similar checks as in POSIX version
        return PCB_AEQA_NO;
    }
    if(!GetConsoleMode(f.handle, &dw))
        return PCB_AEQA_OTHER_ERR;
    if(!SetConsoleMode(f.handle, dw | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        return PCB_AEQA_OTHER_ERR;
    return PCB_AEQA_YES;
#elif PCB_PLATFORM_POSIX
    struct stat s = PCB_ZEROED;
    if(fstat(f.handle, &s) < 0) switch(errno) {
      case EBADF: return PCB_AEQA_BAD_HANDLE;
      default:    return PCB_AEQA_OTHER_ERR;
    }
    if(S_ISREG(s.st_mode)) return PCB_AEQA_NO;
    if(S_ISDIR(s.st_mode)) return PCB_AEQA_NO;
    if(S_ISBLK(s.st_mode)) return PCB_AEQA_NO;
    //This should be impossible. After all, how in hell are you going to write(2) to a symlink?!
    if(S_ISLNK(s.st_mode)) return PCB_AEQA_NO;
    //A niche case where a child process' output is piped to the parent
    //where it's processed similarly to a terminal, but it's not a terminal.
    //This is rare, but possible, so we mark it as "possibly supported".
    if(S_ISFIFO(s.st_mode)) return PCB_AEQA_MAYBE;
    if(S_ISSOCK(s.st_mode)) return PCB_AEQA_MAYBE;
    if(!isatty(f.handle)) return PCB_AEQA_NO;
    //Assume that, if this refers to a terminal, then "supported".
    //May or may not actually be the case.
    //TODO: We should somehow distinguish physical serial output devices
    //like an Arduino from a terminal.
    return PCB_AEQA_YES;
#else
    //Assume no.
    //This disregards the infinite possibilities of embedded devices, but it's
    //simply not possible to determine support for all hardware/software.
    //Do this yourself if you care.
    return PCB_AEQA_NO;
#endif
}

void PCB_check_aeqa_std(PCB_Aeqa aeqa[2]) {
    aeqa[0] = PCB_check_aeqa(PCB_IO_get_stdout());
    aeqa[1] = PCB_check_aeqa(PCB_IO_get_stderr());
}

static const char* PCB__log_label(PCB_LogLevel level, PCB_Aeqa aeqa[2]) {
    switch(level) {
      case PCB_LOGLEVEL_NONE:
      case PCB_LOGLEVEL_NONE_NL:
        break;
      case PCB_LOGLEVEL_TRACE:
      case PCB_LOGLEVEL_TRACE_NL:
        if(aeqa[0] == PCB_AEQA_YES) return "[\033[38;5;238mTrace\033[0m] ";
        return "[Trace] ";
      case PCB_LOGLEVEL_DEBUG:
      case PCB_LOGLEVEL_DEBUG_NL:
        if(aeqa[0] == PCB_AEQA_YES) return "[\033[38;5;51mDebug\033[0m] ";
        return "[Debug] ";
      case PCB_LOGLEVEL_INFO:
      case PCB_LOGLEVEL_INFO_NL:
        return "[Info]  ";
      case PCB_LOGLEVEL_WARN:
      case PCB_LOGLEVEL_WARN_NL:
        if(aeqa[0] == PCB_AEQA_YES) return "[\033[38;5;214mWarn\033[0m]  ";
        return "[Warn]  ";
      case PCB_LOGLEVEL_ERROR:
      case PCB_LOGLEVEL_ERROR_NL:
        if(aeqa[1] == PCB_AEQA_YES) return "[\033[38;5;9mError\033[0m] ";
        return "[Error] ";
      case PCB_LOGLEVEL_FATAL:
      case PCB_LOGLEVEL_FATAL_NL:
        if(aeqa[1] == PCB_AEQA_YES) return "[\033[1m\033[38;5;1mFatal\033[0m] ";
        return "[Fatal] ";
    }
    return "";
}

void PCB_log(PCB_LogLevel level, const char* fmt, ...) {
    //TODO: Should probably be global.
    static PCB_Aeqa aeqa[2];
    if(aeqa[0] == PCB_AEQA_DUNNO || aeqa[1] == PCB_AEQA_DUNNO)
        PCB_check_aeqa_std(aeqa);

    const char* label = PCB__log_label(level, aeqa);

    va_list args;
    va_start(args, fmt);
    switch(level) {
      case PCB_LOGLEVEL_NONE:  case PCB_LOGLEVEL_NONE_NL:
      case PCB_LOGLEVEL_TRACE: case PCB_LOGLEVEL_TRACE_NL:
      case PCB_LOGLEVEL_DEBUG: case PCB_LOGLEVEL_DEBUG_NL:
      case PCB_LOGLEVEL_INFO:  case PCB_LOGLEVEL_INFO_NL:
      case PCB_LOGLEVEL_WARN:  case PCB_LOGLEVEL_WARN_NL:
        PCB_fprintf(PCB_stdout, "%s", label);
        PCB_vfprintf(PCB_stdout, fmt, args); break;
      case PCB_LOGLEVEL_ERROR: case PCB_LOGLEVEL_ERROR_NL:
      case PCB_LOGLEVEL_FATAL: case PCB_LOGLEVEL_FATAL_NL:
        PCB_fprintf(PCB_stderr, "%s", label);
        PCB_vfprintf(PCB_stderr, fmt, args); break;
    }
    va_end(args);
    switch(level) {
      case PCB_LOGLEVEL_NONE:  case PCB_LOGLEVEL_TRACE:
      case PCB_LOGLEVEL_DEBUG: case PCB_LOGLEVEL_INFO:
      case PCB_LOGLEVEL_WARN:
        PCB_fprintf(PCB_stdout, "\n"); break;
      case PCB_LOGLEVEL_ERROR: case PCB_LOGLEVEL_FATAL:
        PCB_fprintf(PCB_stderr, "\n"); break;
      case PCB_LOGLEVEL_NONE_NL:  case PCB_LOGLEVEL_TRACE_NL:
      case PCB_LOGLEVEL_DEBUG_NL: case PCB_LOGLEVEL_INFO_NL:
      case PCB_LOGLEVEL_WARN_NL:
#ifdef PCB_DEBUG
        PCB_fflush(PCB_stdout);
#endif  //explicitly flush if debugging is enabled
        //fallthrough since stderr isn't buffered
      case PCB_LOGLEVEL_ERROR_NL: case PCB_LOGLEVEL_FATAL_NL:
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


//Section 3.2: Platform-independent (sort of) filesystem functions
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

//skip until not separator
static PCB_StringView PCB__FS_SUNS(PCB_StringView path) {
    while(path.length > 0) { //skip duplicated separators
        if(path.data[path.length-1] == PCB_FS_DIR_DELIM) goto cont;
#if PCB_PLATFORM_WINDOWS
        if(path.data[path.length-1] == '/') goto cont;
#endif
        break;
    cont:
        --path.length;
    }
    return path;
}

PCB_StringView PCB_FS_Basename(PCB_StringView path) {
    if(PCB_String_isEmpty(&path)) return PCB_StringView_from_cstr(".");
    const char sep[2] = { PCB_FS_DIR_DELIM, '\0' };
#if PCB_PLATFORM_WINDOWS
    const char sep_alt[2] = "/";
#endif
    PCB_StringView dirsep = PCB_StringView_rsubcstr(path, sep);
#if PCB_PLATFORM_WINDOWS
    //CMD allows for paths with '/'. Dunno if it internally replaces them with '\'.
    //We'll assume Windows is chill with '/' as a directory separator.
    PCB_StringView dirsep_alt = PCB_StringView_rsubcstr(path, sep_alt);
    if(PCB_String_isEmpty(&dirsep)) dirsep = dirsep_alt;
    else if(!PCB_String_isEmpty(&dirsep_alt) && dirsep_alt.data > dirsep.data)
        dirsep = dirsep_alt; //use rightmost separator
#endif
    if(PCB_String_isEmpty(&dirsep)) return path;
    PCB_StringView base = {
        dirsep.data + 1,
        path.length - (size_t)(dirsep.data - path.data) - 1
    };
    if(base.length > 0) return base;
    path = PCB__FS_SUNS(path);
    //`path` was just separators
    //return the leftmost one, it shouldn't matter which one is actually returned
    if(path.length == 0) { path.length = 1; return path; }
    //Find *previous* separator, i.e. apply identical logic a 2nd time and not more.
    //A loop here would imply that we could potentially do so more than twice, but
    //we don't. DO NOT factor it out into a loop.
    dirsep = PCB_StringView_rsubcstr(path, sep);
#if PCB_PLATFORM_WINDOWS
    dirsep_alt = PCB_StringView_rsubcstr(path, sep_alt);
    if(PCB_String_isEmpty(&dirsep)) dirsep = dirsep_alt;
    else if(!PCB_String_isEmpty(&dirsep_alt) && dirsep_alt.data > dirsep.data)
        dirsep = dirsep_alt;
#endif
    if(PCB_String_isEmpty(&dirsep)) return path;
    return PCB_CLITERAL(PCB_StringView){
        dirsep.data + 1,
        path.length - (size_t)(dirsep.data - path.data) - 1
    };
}

PCB_StringView PCB_FS_Dirname(PCB_StringView path) {
    if(PCB_String_isEmpty(&path)) return PCB_StringView_from_cstr(".");
    char sep[2]=  { PCB_FS_DIR_DELIM, '\0' };
#if PCB_PLATFORM_WINDOWS
    char sep_alt[2] = "/";
#endif
    PCB_StringView dirsep = PCB_StringView_rsubcstr(path, sep);
#if PCB_PLATFORM_WINDOWS
    PCB_StringView dirsep_alt = PCB_StringView_rsubcstr(path, sep_alt);
    if(PCB_String_isEmpty(&dirsep)) dirsep = dirsep_alt;
    //use rightmost separator
    else if(!PCB_String_isEmpty(&dirsep_alt) && dirsep_alt.data > dirsep.data)
        dirsep = dirsep_alt;
#endif
    if(PCB_String_isEmpty(&dirsep)) return PCB_StringView_from_cstr(".");
    path.length = (size_t)(dirsep.data - path.data);
    path = PCB__FS_SUNS(path);
    if(path.length == 0) path.length = 1; //`path` was just separators
    return path;
}

PCB_StringView PCB_FS_Extension(PCB_StringView path) {
    path = PCB_FS_Basename(path);
    PCB_assert(!PCB_String_isEmpty(&path));
    return PCB_FS_Extension_base(path);
}

PCB_StringView PCB_FS_Extension_base(PCB_StringView path) {
    PCB_StringView last_dot = PCB_StringView_rsubcstr(path, ".");
    if(PCB_String_isEmpty(&last_dot)) return last_dot;
    PCB_StringView ext = {
        last_dot.data + 1,
        path.length - (size_t)(last_dot.data - path.data) - 1
    };
    //It's better to crash than have OOB reads, as seen in Mongobleed.
    if(ext.length == 0) ext.data = NULL;
    return ext;
}
#endif //PCB_IMPLEMENTATION_FS



//Section 3.3: Input/output utilities
#ifdef PCB_IMPLEMENTATION_IO
PCB_File PCB_IO_get_stdin(void) {
    PCB_File f = PCB_File_init();
#if PCB_PLATFORM_WINDOWS
    f.handle = GetStdHandle(STD_OUTPUT_HANDLE);
#elif PCB_PLATFORM_POSIX
    f.handle = STDIN_FILENO;
#endif //platforms
    return f;
}

PCB_File PCB_IO_get_stdout(void) {
    PCB_File f = PCB_File_init();
#if PCB_PLATFORM_WINDOWS
    f.handle = GetStdHandle(STD_OUTPUT_HANDLE);
#elif PCB_PLATFORM_POSIX
    f.handle = STDOUT_FILENO;
#endif //platforms
    return f;
}

PCB_File PCB_IO_get_stderr(void) {
    PCB_File f = PCB_File_init();
#if PCB_PLATFORM_WINDOWS
    f.handle = GetStdHandle(STD_ERROR_HANDLE);
#elif PCB_PLATFORM_POSIX
    f.handle = STDERR_FILENO;
#endif //platforms
    return f;
}
#endif //PCB_IMPLEMENTATION_IO



//Section 3.4: Strings, string views, vectors of strings...
#ifdef PCB_IMPLEMENTATION_STRING
size_t PCB_strlen_char8 (const PCB_char8*  str) { const PCB_char8*  s = str; while(*s++) {} return (size_t)(s - str); }
size_t PCB_strlen_char16(const PCB_char16* str) { const PCB_char16* s = str; while(*s++) {} return (size_t)(s - str); }
size_t PCB_strlen_char32(const PCB_char32* str) { const PCB_char32* s = str; while(*s++) {} return (size_t)(s - str); }

//poor man's C++ templates
#define PCB__Str_destroy(Type) \
void Type##_destroy(Type* PCB_restrict str) { \
    PCB_CHECK_SELF(str,); \
    PCB_Vec_destroy(str); \
}
PCB__Str_destroy(PCB_String)    //PCB_String_destroy()
PCB__Str_destroy(PCB_WString)   //PCB_WString_destroy()
PCB__Str_destroy(PCB_U8String)  //PCB_U8String_destroy()
PCB__Str_destroy(PCB_U16String) //PCB_U16String_destroy()
PCB__Str_destroy(PCB_U32String) //PCB_U32String_destroy()
#undef PCB__Str_destroy

#define PCB__Str_reserve(Type, charType) \
bool Type##_reserve(Type* PCB_restrict str, const size_t howMany) { \
    PCB_CHECK_SELF(str, false); \
    const size_t newSize = str->length + howMany + 1; /*'\0'*/ \
    if(newSize <= str->capacity) return true; \
    if(newSize > (SIZE_MAX)/2 / sizeof(*str->data)) return false; \
    size_t newCapacity = str->capacity == 0 ? PCB_VEC_INITIAL_CAPACITY : str->capacity; \
    while(newSize > newCapacity) newCapacity *= 2; \
    if(newCapacity > (SIZE_MAX/2) / sizeof(*str->data)) return false; \
    charType* newData = (charType*)PCB_realloc(str->data, newCapacity * sizeof(*str->data)); \
    if(newData == NULL) return false; \
    str->data = newData; str->capacity = newCapacity; \
    return true; \
}
PCB__Str_reserve(PCB_String,    char)       //PCB_String_reserve()
PCB__Str_reserve(PCB_WString,   wchar_t)    //PCB_WString_reserve()
PCB__Str_reserve(PCB_U8String,  PCB_char8)  //PCB_U8String_reserve()
PCB__Str_reserve(PCB_U16String, PCB_char16) //PCB_U16String_reserve()
PCB__Str_reserve(PCB_U32String, PCB_char32) //PCB_U32String_reserve()
#undef PCB__Str_reserve

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

bool PCB_String_append_codepoint(
    PCB_String* PCB_restrict str, int32_t codepoint
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(!PCB_IsValidUnicode(codepoint), false);
    uint8_t l = PCB_GetUTF8Length_unchecked(codepoint);
    if(!PCB_String_reserve(str, l)) return false;
    PCB_StoreUTF8Codepoint(str->data + str->length, codepoint);
    str->data[str->length += l] = '\0';
    return true;
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

bool PCB_String_insert_codepoint(
    PCB_String* PCB_restrict str, int32_t codepoint, size_t position
) {
    PCB_CHECK_SELF(str, false);
    PCB_CHECK(position > str->length, false);
    PCB_CHECK(!PCB_IsValidUnicode(codepoint), false);
    uint8_t l = PCB_GetUTF8Length_unchecked(codepoint);

    if(!PCB_String_reserve(str, l)) return false;
    PCB_memmove(
        str->data + position + l,
        str->data + position,
        str->length - position
    );

    PCB_StoreUTF8Codepoint(str->data + position, codepoint);
    str->data[str->length += l] = '\0';
    return true;
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

void PCB_String_trim_left(PCB_String* PCB_restrict str) {
    PCB_CHECK_SELF(str,);
    if(PCB_String_isEmpty(str)) return;
    const char* cursor = str->data;
    while(PCB_isspace(*cursor)) ++cursor;
    size_t firstNonWsp = (size_t)(cursor - str->data);
    if(firstNonWsp == 0) return; //redundant memmove
    PCB_memmove(
        str->data,
        str->data + firstNonWsp,
        str->length - firstNonWsp + 1 //'\0'
    );
    str->length -= firstNonWsp;
}

void PCB_String_trim_right(PCB_String* PCB_restrict str) {
    PCB_CHECK_SELF(str,);
    if(PCB_String_isEmpty(str)) return;
    const char* cursor = str->data + str->length - 1;
    while((cursor - str->data) >= 0 && PCB_isspace(*cursor)) --cursor;
    size_t newLen = (size_t)(cursor + 1 - str->data);
    str->data[str->length = newLen] = '\0';
}

PCB_String PCB_String_from_StringView(PCB_StringView sv) {
    PCB_String s = PCB_ZEROED;
    PCB_String_append_sv(&s, sv); //it this fails, `s` will stil be zeroed
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

PCB_StringView PCB_StringView_trim_left(PCB_StringView sv) {
    if(PCB_String_isEmpty(&sv)) return PCB_ZEROED_T(PCB_StringView);
    while(sv.length > 0 && PCB_isspace(*sv.data)) { ++sv.data; --sv.length; }
    return sv;
}

PCB_StringView PCB_StringView_trim_right(PCB_StringView sv) {
    if(PCB_String_isEmpty(&sv)) return PCB_ZEROED_T(PCB_StringView);
    while(sv.length > 0 && PCB_isspace(sv.data[sv.length-1])) { --sv.length; }
    return sv;
}

PCB_StringView PCB_StringView_substr_n(
    PCB_StringView sv, const PCB_StringView sub, size_t n
) {
    PCB_CHECK(n == 0, PCB_ZEROED_T(PCB_StringView));
    if(PCB_String_isEmpty(&sv))  return PCB_ZEROED_T(PCB_StringView);
    if(PCB_String_isEmpty(&sub)) return PCB_ZEROED_T(PCB_StringView);
    PCB_StringView s = sub;
    //TODO: "premature optimization is the root of all evil",
    //this loop would benefit greatly from vectorization
    while(n > 0) {
        while(sv.length > 0 && sv.data[0] != s.data[0]) {
            sv.data++; sv.length--;
        }
        if(sv.length == 0) return PCB_ZEROED_T(PCB_StringView);
        while(s.length > 0 && sv.length > 0 && sv.data[0] == s.data[0]) {
            sv.data++; sv.length--;
            s.data++;  s.length--;
        }
        if(s.length == 0) n -= 1;
        else if(sv.length == 0) return PCB_ZEROED_T(PCB_StringView);
        s = sub; //search again
    }
    sv.data -= sub.length;
    sv.length = sub.length;
    return sv;
}

PCB_StringView PCB_StringView_rsubstr_n(
    PCB_StringView sv, const PCB_StringView sub, size_t n
) {
    PCB_CHECK(n == 0, PCB_ZEROED_T(PCB_StringView));
    if(PCB_String_isEmpty(&sv))  return PCB_ZEROED_T(PCB_StringView);
    if(PCB_String_isEmpty(&sub)) return PCB_ZEROED_T(PCB_StringView);
    PCB_StringView s = sub;
    while(n > 0) {
        while(sv.length > 0 && sv.data[sv.length-1] != s.data[s.length-1]) {
            --sv.length;
        }
        if(sv.length == 0) return PCB_ZEROED_T(PCB_StringView);
        while(s.length > 0 && sv.length > 0 && sv.data[sv.length-1] == s.data[s.length-1]) {
            --sv.length; --s.length;
        }
        if(s.length == 0) n -= 1;
        else if(sv.length == 0) return PCB_ZEROED_T(PCB_StringView);
        s = sub;
    }
    sv.data += sv.length;
    sv.length = sub.length;
    return sv;
}

PCB_StringViews PCB_StringView_split(
    PCB_StringView sv,
    PCB_StringView delim
) {
    PCB_StringViews views = PCB_ZEROED;
    PCB_CHECK(PCB_String_isEmpty(&delim), views);

    PCB_StringView cur = PCB_StringView_substr(sv, delim);
    while(cur.data != NULL && cur.length > 0) {
        size_t slice_len = (size_t)(&cur.data[0] - &sv.data[0]);
        PCB_Vec_append(&views, (PCB_CLITERAL(PCB_StringView){ sv.data, slice_len }));
        sv.length -= slice_len + cur.length;
        sv.data   += slice_len + cur.length;
        cur = PCB_StringView_substr(sv, delim);
    }
    if(sv.length > 0) PCB_Vec_append(&views, sv);
    return views;
}

PCB_Strings PCB_StringView_split_copy(
    PCB_StringView sv, PCB_StringView delim
) {
    PCB_Strings strs = PCB_ZEROED;
    PCB_CHECK(PCB_String_isEmpty(&delim), strs);

    PCB_StringView cur = PCB_StringView_substr(sv, delim);
    PCB_String str;
    while(cur.data != NULL && cur.length > 0) {
        str = PCB_ZEROED_T(PCB_String);
        size_t slice_len = (size_t)(&cur.data[0] - &sv.data[0]);
        PCB_String_append_sv(&str, PCB_CLITERAL(PCB_StringView){sv.data, slice_len});
        PCB_Vec_append(&strs, str);
        sv.length -= slice_len + cur.length;
        sv.data   += slice_len + cur.length;
        cur = PCB_StringView_substr(sv, delim);
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

PCB_StringView PCB_StringView_skipPast(PCB_StringView sv, PCB_StringView s) {
    if(PCB_String_isEmpty(&sv)) return PCB_ZEROED_T(PCB_StringView);
    if(PCB_String_isEmpty(&s))  return PCB_ZEROED_T(PCB_StringView);
    while(s.length > 0 && sv.length > 0) {
        if(*sv.data != *s.data) return PCB_ZEROED_T(PCB_StringView);
        ++sv.data; --sv.length;
        ++s.data;  --s.length;
    }
    if(s.length == 0) return sv;
    return PCB_ZEROED_T(PCB_StringView);
}

//PCB_CodepointLengthFromFirstCharacter_UTF8
//Pay close attention to return values before use.
static PCB_ForceInline uint8_t PCB__CPLFFC_UTF8(unsigned int ch) {
//NOTE: `ch` is NOT a Unicode codepoint, it is a zero-extended 1st byte of
//the input string
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
#define PCB__unlikely(cond) __builtin_expect(!!(cond), 0)
#else
#define PCB__unlikely(cond) (cond)
#endif
#if defined(PCB_UTF8_FULL_RANGE) && !defined(PCB_UNICODE_CONFORMANT)
    if(PCB__unlikely(ch > 0xFD)) return 255; //1111111-, invalid
#else
    if(PCB__unlikely(ch > 0xF4)) return 255; //111110xx (>U+10FFFF), 1111110x
#endif //PCB_UTF8_FULL_RANGE
    if(PCB__unlikely(ch == 0xC0 || ch == 0xC1)) return 254;
#undef PCB__unlikely
#if PCB_ARCH_x86_64
#if PCB_COMPILER_GCC || PCB_COMPILER_CLANG
    __asm__ __volatile__ goto (
        "mov{l} {$2147483649, %%eax | eax, 2147483649}\n\t"
        "cpuid\n\t"
        "test{l} {$32, %%ecx | ecx, 32}\n\t"
        "jz %l0"
        ::: "eax", "ecx", "edx", "cc" : no_lzcnt
    );
    __asm__ __volatile__(
        "not{l} %k0\n\t"
        "{sall $24, %k0 | shl %k0, 24}\n\t"
        "lzcnt{l} %k0, %k0\n\t"
        "cmp{l $1, %k0 | %k0, 1}\n\t"
        "jg ret%=\n\t"
        "xor{l $1, %k0 | %k0, 1}\n" /* 0->1 (ASCII), 1->0 (continuation byte) */
        "ret%=:\n\t"
        : "+r" (ch) :: "cc"
    );
    return (uint8_t)ch;
#elif PCB_COMPILER_MSVC
    //https://learn.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex
    int cpuinfo[4];
    __cpuid(cpuinfo, 0x80000001);
    if(!(cpuinfo[2] & (1 << 5))) goto no_lzcnt;
    ch = __lzcnt(~ch << 24);
    return (ch > 1 ? ch : ch ^ 1); //0->1 (ASCII), 1->0 (continuation byte)
#endif //compilers/x86_64
#endif //architectures
    if(0) goto no_lzcnt; //suppress "unused label" warnings
    no_lzcnt:
    if(ch <= 0x7F) return 1; /* ASCII */
    if(ch <= 0xBF) return 0; /* 10xxxxxx, continuation byte, invalid input */
    if(ch <= 0xDF) return 2; /* 110xxxxx */
    if(ch <= 0xEF) return 3; /* 1110xxxx */
#if defined(PCB_UTF8_FULL_RANGE) && !defined(PCB_UNICODE_CONFORMANT)
    if(ch <= 0xF7) return 4; /* 11110xxx */
    if(ch <= 0xFB) return 5; /* 111110xx */
    if(ch <= 0xFD) return 6; /* 1111110x */
#else
    if(ch <= 0xF4) return 4; /* 11110xxx (<=U+10FFFF)*/
#endif
    PCB_Unreachable;
}

uint8_t PCB_StringView_GetCodepointLength(PCB_StringView sv, size_t index) {
    PCB_CHECK(PCB_String_isEmpty(&sv), 0);
    PCB_CHECK(index >= sv.length, 0);
    return PCB_StringView_GetCodepointLength_unchecked(sv, index);
}

uint8_t PCB_StringView_GetCodepointLength_unchecked(
    PCB_StringView sv, size_t index
) {
    uint8_t len = PCB__CPLFFC_UTF8((unsigned char)sv.data[index]);
    if(len == 0 || len >= 254) return 0;
    return len;
}

#ifndef PCB_UNICODE_CONFORMANT
static const int32_t PCB__MINIMAL_CODEPOINT_UTF8[] = {
    0, 0x80, 0x800, 0x10000,
#ifdef PCB_UTF8_FULL_RANGE
    0x200000, 0x4000000,
#endif //PCB_UTF8_FULL_RANGE
};
#endif //!PCB_UNICODE_CONFORMANT

PCB_Codepoint PCB_StringView_GetCodepoint(PCB_StringView sv, size_t index) {
    PCB_CHECK(PCB_String_isEmpty(&sv), (PCB_CLITERAL(PCB_Codepoint){ -16, 0 }));
    PCB_CHECK(index >= sv.length, (PCB_CLITERAL(PCB_Codepoint){ -17, 0 }));
    PCB_CHECK((uintptr_t)sv.data > (uintptr_t)-1 - sv.length, (PCB_CLITERAL(PCB_Codepoint){ -18, 0 }));
    return PCB_StringView_GetCodepoint_unchecked(sv, index);
}

PCB_Codepoint PCB_StringView_GetCodepoint_unchecked(PCB_StringView sv, size_t index) {
#define PCB__ISCONT(byte) ((byte & 0xC0) == 0x80)
#define PCB__CP_ERR(code, bytesToSkip) PCB_CLITERAL(PCB_Codepoint){ code, bytesToSkip }
    const unsigned char* cursor = (const unsigned char*)(sv.data + index);
    const unsigned char* const end = (const unsigned char*)(sv.data + sv.length);
    uint8_t len = PCB__CPLFFC_UTF8(*cursor);
    if(len == 0) return PCB__CP_ERR(-1, 1);
    else if(len == 254) return PCB__CP_ERR(-6, 1);
    else if(len == 255) return PCB__CP_ERR(-3, 1);

    if((uintptr_t)cursor > (uintptr_t)-1 - len) return PCB__CP_ERR(-18, 0);
#ifndef PCB_UNICODE_CONFORMANT
    if(cursor + len > end) return PCB__CP_ERR(-2, 1);
#endif //!PCB_UNICODE_CONFORMANT
    if(len == 1) return PCB_CLITERAL(PCB_Codepoint){ *cursor, 1 }; //ASCII

    const uint32_t mask = (1u << (8u - len)) - 1;
    uint32_t codepoint = (uint32_t)*cursor & mask;

#ifdef PCB_UNICODE_CONFORMANT
    if(cursor + 1 == end) return PCB__CP_ERR(-2, 1);
    //https://www.unicode.org/versions/Unicode6.0.0/ch03.pdf, table 3-7
    switch(cursor[0]) {
      case 0xE0: if(cursor[1] < 0xA0) { return PCB__CP_ERR(-3, 1); } break;
      case 0xED: if(cursor[1] > 0x9F) { return PCB__CP_ERR(-3, 1); } break;
      case 0xF0: if(cursor[1] < 0x90) { return PCB__CP_ERR(-3, 1); } break;
      case 0xF4: if(cursor[1] > 0x8F) { return PCB__CP_ERR(-3, 1); } break;
      default: break;
    }
    ++cursor;

    switch(len) {
      case 4:
        if(!PCB__ISCONT(*cursor)) return PCB__CP_ERR(-4, 1);
        codepoint = (codepoint << 6) + (*cursor++ & 0x3F);
        //fallthrough
      case 3:
        if(!PCB__ISCONT(*cursor)) return PCB__CP_ERR(-4, len - 2u);
        if(cursor == end)         return PCB__CP_ERR(-2, len - 2u);
        codepoint = (codepoint << 6) + (*cursor++ & 0x3F);
        //fallthrough
      case 2:
        if(!PCB__ISCONT(*cursor)) return PCB__CP_ERR(-4, len - 1u);
        if(cursor == end)         return PCB__CP_ERR(-2, len - 1u);
        codepoint = (codepoint << 6) + (*cursor & 0x3F);
        return PCB_CLITERAL(PCB_Codepoint){ (int32_t)codepoint, len };
      default: PCB_Unreachable;
    }
#else
    ++cursor;
    for(uint8_t l = len - 1; l > 0; ++cursor, --l) {
        if(!PCB__ISCONT(*cursor)) return PCB__CP_ERR(-4, (uint32_t)(len - l));
        codepoint = (codepoint << 6) + (*cursor & 0x3F);
    }
    // const int32_t errval = -(int32_t)len - 2;
    if(0xD800 <= codepoint && codepoint <= 0xDFFF)
        return PCB__CP_ERR(-5, len); //surrogates
    if((int32_t)codepoint < PCB__MINIMAL_CODEPOINT_UTF8[len - 1])
        return PCB__CP_ERR(-3, len);
#ifndef PCB_UTF8_FULL_RANGE
    if(codepoint > 0x10FFFF) return PCB__CP_ERR(-3, len);
#endif //!PCB_UTF8_FULL_RANGE
    return PCB_CLITERAL(PCB_Codepoint){ (int32_t)codepoint, len };
#endif //PCB_UNICODE_CONFORMANT
#undef PCB__ISCONT
#undef PCB__CP_ERR
}

bool PCB_IsValidUnicode(int32_t codepoint) {
    if(0xD800 <= codepoint && codepoint <= 0xDFFF) return false; //surrogates

    //https://www.unicode.org/versions/corrigendum9.html
    // if(0xFDD0 <= codepoint && codepoint <= 0xFDEF) return false; //reserved
    // const int32_t ls2b = codepoint & 0xFFFF;
    // if(ls2b == 0xFFFE || ls2b == 0xFFFF) return false; //also reserved

    if(codepoint < 0x000000) return false;
#if !(defined(PCB_UTF8_FULL_RANGE) && !defined(PCB_UNICODE_CONFORMANT))
    if(codepoint > 0x10FFFF) return false;
#endif //!PCB_UTF8_FULL_RANGE
    return true;
}

uint8_t PCB_GetUTF8Length_unchecked(int32_t codepoint) {
    if(codepoint <= 0x000007F) return 1;
    if(codepoint <= 0x00007FF) return 2;
    if(codepoint <= 0x000FFFF) return 3;
#if defined(PCB_UTF8_FULL_RANGE) && !defined(PCB_UNICODE_CONFORMANT)
    if(codepoint <= 0x01FFFFF) return 4;
    if(codepoint <= 0x3FFFFFF) return 5;
    return 6;
#else
    return 4;
#endif //PCB_UTF8_FULL_RANGE
}

uint8_t PCB_GetUTF8Length(int32_t codepoint) {
    if(!PCB_IsValidUnicode(codepoint)) return 0;
    return PCB_GetUTF8Length_unchecked(codepoint);
}

//modified from https://gist.github.com/tylerneylon/9773800
char* PCB_StoreUTF8Codepoint(char* buf, int32_t codepoint) {
    if(codepoint <= 0x7F) {
        *(uint8_t*)buf = (uint8_t)((uint32_t)codepoint & 0x7F);
        return buf + 1;
    }
    uint8_t c[6] = PCB_ZEROED; int i = 0; uint8_t first_max = 0x1F;
    while(codepoint > first_max) {
        c[i++] = (uint8_t)((uint32_t)codepoint & 0x3F) | 0x80;
        codepoint >>= 6; first_max >>= 1;
    }
    c[i++] = ((uint8_t)((uint32_t)codepoint) & first_max) | (~first_max << 2);
    while(i > 0) *(uint8_t*)buf++ = c[--i];
    return buf;
}
#endif //PCB_IMPLEMENTATION_STRING

#if !defined(PCB_NO_INLINE_EXPORTS) || (defined(PCB_NO_INLINE_EXPORTS) && defined(PCB_IMPLEMENTATION_STRING))
PCB_maybe_inline void PCB_String_trim(PCB_String* PCB_restrict str) {
    PCB_String_trim_left (str);
    PCB_String_trim_right(str);
}

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

PCB_maybe_inline PCB_StringView PCB_StringView_from_parts(
    const char* PCB_restrict ptr, size_t length
) {
    PCB_CHECK(ptr == NULL, PCB_ZEROED_T(PCB_StringView));
    return PCB_View_Ptr_A_T(ptr, PCB_StringView, length);
}


PCB_maybe_inline PCB_StringView PCB_StringView_skipPast_cstr(
    PCB_StringView sv, const char* s
) { return PCB_StringView_skipPast(sv, PCB_StringView_from_cstr(s)); }


PCB_maybe_inline bool PCB_String_replace_range_cstr(
    PCB_String* PCB_restrict str,
    size_t start, size_t length,
    const char* PCB_restrict cstr
) { return PCB_String_replace_range(str, start, length, PCB_StringView_from_cstr(cstr)); }


PCB_maybe_inline PCB_StringView PCB_StringView_substr(
    PCB_StringView sv, PCB_StringView sub
) { return PCB_StringView_substr_n(sv, sub, 1); }

PCB_maybe_inline PCB_StringView PCB_StringView_subcstr(
    PCB_StringView sv, const char* sub
) { return PCB_StringView_substr(sv, PCB_StringView_from_cstr(sub)); }

PCB_maybe_inline PCB_StringView PCB_StringView_subcstr_n(
    PCB_StringView sv, const char* sub, size_t n
) { return PCB_StringView_substr_n(sv, PCB_StringView_from_cstr(sub), n); }

PCB_maybe_inline PCB_StringView PCB_String_substr(
    const PCB_String* str, const PCB_String* sub
) { return PCB_StringView_substr(PCB_StringView_from_String(str), PCB_StringView_from_String(sub)); }

PCB_maybe_inline PCB_StringView PCB_String_subcstr(
    const PCB_String* str, const char* sub
) { return PCB_StringView_subcstr(PCB_StringView_from_String(str), sub); }

PCB_maybe_inline PCB_StringView PCB_String_substr_n(
    const PCB_String* str, const PCB_String* sub, size_t n
) { return PCB_StringView_substr_n(PCB_StringView_from_String(str), PCB_StringView_from_String(sub), n); }

PCB_maybe_inline PCB_StringView PCB_String_subcstr_n(
    const PCB_String* str, const char* sub, size_t n
) { return PCB_StringView_subcstr_n(PCB_StringView_from_String(str), sub, n); }


PCB_maybe_inline PCB_StringView PCB_StringView_rsubstr(
    PCB_StringView sv, PCB_StringView sub
) { return PCB_StringView_rsubstr_n(sv, sub, 1); }

PCB_maybe_inline PCB_StringView PCB_StringView_rsubcstr(
    PCB_StringView sv, const char* sub
) { return PCB_StringView_rsubstr(sv, PCB_StringView_from_cstr(sub)); }

PCB_maybe_inline PCB_StringView PCB_StringView_rsubcstr_n(
    PCB_StringView sv, const char* sub, size_t n
) { return PCB_StringView_rsubstr_n(sv, PCB_StringView_from_cstr(sub), n); }

PCB_maybe_inline PCB_StringView PCB_String_rsubstr(
    const PCB_String* str, const PCB_String* sub
) { return PCB_StringView_rsubstr(PCB_StringView_from_String(str), PCB_StringView_from_String(sub)); }

PCB_maybe_inline PCB_StringView PCB_String_rsubcstr(
    const PCB_String* str, const char* sub
) { return PCB_StringView_rsubcstr(PCB_StringView_from_String(str), sub); }

PCB_maybe_inline PCB_StringView PCB_String_rsubstr_n(
    const PCB_String* str, const PCB_String* sub, size_t n
) { return PCB_StringView_rsubstr_n(PCB_StringView_from_String(str), PCB_StringView_from_String(sub), n); }

PCB_maybe_inline PCB_StringView PCB_String_rsubcstr_n(
    const PCB_String* str, const char* sub, size_t n
) { return PCB_StringView_rsubcstr_n(PCB_StringView_from_String(str), sub, n); }


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


PCB_maybe_inline PCB_StringView PCB_StringView_skipPast_sub_n(
    PCB_StringView sv, PCB_StringView sub, size_t n
) {
    PCB_StringView s = PCB_StringView_substr_n(sv, sub, n);
    if(PCB_String_isEmpty(&s)) return s;
    return PCB_StringView_from_parts(
        s.data + s.length,
        (size_t)((sv.data+sv.length)-(s.data+s.length))
    );
}

PCB_maybe_inline PCB_StringView PCB_StringView_skipPast_sub(
    PCB_StringView sv, PCB_StringView sub
) { return PCB_StringView_skipPast_sub_n(sv, sub, 1); }

PCB_maybe_inline PCB_StringView PCB_StringView_skipPast_subcstr(
    PCB_StringView sv, const char* sub
) { return PCB_StringView_skipPast_sub_n(sv, PCB_StringView_from_cstr(sub), 1); }

PCB_maybe_inline PCB_StringView PCB_StringView_skipPast_subcstr_n(
    PCB_StringView sv, const char* sub, size_t n
) { return PCB_StringView_skipPast_sub_n(sv, PCB_StringView_from_cstr(sub), n); }


PCB_maybe_inline PCB_StringView PCB_StringView_skipPast_whitespace(
    PCB_StringView sv
) { return PCB_StringView_skipPast_anyCharFrom_cstr(sv, " \t\n\r\v\f"); }

PCB_maybe_inline PCB_StringView PCB_StringView_trim(PCB_StringView sv) {
    return PCB_StringView_trim_right(PCB_StringView_trim_left(sv));
}
#endif //PCB_IMPLEMENTATION_STRING (inline)

//Section 3.5: Platform-independent (sort of) process functions.
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



//Section 3.6: other platform-independent stuff
#ifdef PCB_IMPLEMENTATION_ARENA
struct PCB_Arena_Mark {
    size_t length;
    size_t lengths[1];
};

#define PCB__Arena_forEach_node(cur, next)                      \
for(PCB_Arena_Prefix                                            \
        *cur  = (PCB_Arena_Prefix*)arena,                       \
        *next = (PCB_Arena_Prefix*)cur->next;                   \
    cur != NULL;                                                \
    cur  = next,                                                \
    next = next == NULL ? NULL : (PCB_Arena_Prefix*)next->next  \
)

static PCB_ForceInline char* PCB__Arena_start(PCB_Arena_Prefix* a) {
    return (char*)a + sizeof(*a);
}

static PCB_ForceInline void* PCB__Arena_cur(PCB_Arena_Prefix* a) {
    return PCB__Arena_start(a) + a->length*sizeof(void*);
}

static PCB_ForceInline char* PCB__Arena_end(PCB_Arena_Prefix* a) {
    return PCB__Arena_start(a) + a->capacity*sizeof(void*);
}

//Only safe if `a` comes from `PCB__Arena_ptrnode` and neither are NULL.
static PCB_ForceInline size_t PCB__Arena_offsetof(PCB_Arena_Prefix* a, void* ptr) {
    return (size_t)((char*)ptr - PCB__Arena_start(a))/sizeof(void*);
}

//size rounded up to a multiple of pointer size
static PCB_ForceInline size_t PCB__Arena_ceil(size_t size) {
    return (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
}

static inline size_t PCB__Arena_nodes(PCB_Arena_Prefix* current) {
    PCB_Arena_Prefix* next = (PCB_Arena_Prefix*)current->next;
    size_t i = 1;
    while(next != NULL) {
        ++i;
        current = next;
        next = (PCB_Arena_Prefix*)next->next;
    }
    return i;
}

//node that houses `mark`
static inline PCB_Arena_Prefix* PCB__Arena_marknode(
    PCB_Arena* arena, PCB_Arena_Mark* mark
) {
    const size_t marklen = sizeof(mark->length) + mark->length * sizeof(mark->lengths[0]);
    char* const m = (char*)mark;
    PCB__Arena_forEach_node(current, next) {
        char* start = PCB__Arena_start(current);
        char* end   = PCB__Arena_end(current);
        if(m >= start && m + marklen <= end) return current;
    }
    return NULL;
}

static inline void PCB__Arena_restore(PCB_Arena* arena, PCB_Arena_Mark* mark) {
    PCB_Arena_Prefix* cur  = (PCB_Arena_Prefix*)arena;
    PCB_Arena_Prefix* next = (PCB_Arena_Prefix*)cur->next;
    //NOTE: this code assumes the number of nodes in `arena` cannot be
    //`< mark->length`, which may not hold true if `arena` gains the capability
    //of dropping nodes
    for(size_t i = 0; i < mark->length; i++) {
        next = (PCB_Arena_Prefix*)cur->next;
        //Suppose the following history: ([x] -> buffer placed in node x)
        //alloc_1[2], mark[1], free_1, restore
        //In such case `cur->length` will be smaller than `mark->length`
        //on restore, so assigning old value would leak.
        if(cur->length > mark->lengths[i]) {
            cur->length = mark->lengths[i];
#ifdef __SANITIZE_ADDRESS__
            ASAN_POISON_MEMORY_REGION(
                PCB__Arena_cur(cur),
                (cur->capacity - cur->length)*sizeof(void*)
            );
#endif //ASan
        }
        cur = next;
    }
    //if new nodes were allocated after `mark`'s creation, they should be reset
    if(next != NULL) PCB_Arena_reset((PCB_Arena*)next);
}

static inline bool PCB__Arena_holds(PCB_Arena_Prefix* a, const void* ptr) {
    char* start = PCB__Arena_start(a);
    char* end   = PCB__Arena_end(a); //         `<= end` or `< end`?
    return (const char*)ptr >= start && (const char*)ptr <= end;
}

static inline PCB_Arena_Prefix* PCB__Arena_ptrnode(PCB_Arena* arena, const void* ptr) {
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    while(a != NULL) {
        if(PCB__Arena_holds(a, ptr)) return a;
        a = (PCB_Arena_Prefix*)a->next;
    }
    return NULL;
}

/**
 * Computes the padding required, given `start`, so that
 * at least `reserve` units are available immediately prior and
 * alignment equal to `alignment` is guaranteed.
 *
 * In terms of arenas, it computes the padding from `start` required for
 * an allocation aligned to at least `alignment*sizeof(void*)` bytes, which reserves
 * `reserve*sizeof(void*)` bytes immediately prior to the allocation for metadata.
 *
 * The result is correct and well-defined even if `start` and `reserve` are 0.
 */
static PCB_ForceInline size_t PCB__Arena_pad(
    uintptr_t start, size_t alignment, size_t reserve
) {
    return reserve + alignment - ((size_t)start+reserve-1)%alignment - 1;
}

static PCB_ForceInline size_t PCB__Arena_meta_size(PCB_Arena_Prefix* a) {
#ifdef __SANITIZE_ADDRESS__
    (void)a;
    return sizeof(PCB_Arena_Alloc_Meta)/sizeof(void*);
#else
    return a->flags & PCB_ARENA_FLAG_ALLOC_META
        ? sizeof(PCB_Arena_Alloc_Meta)/sizeof(void*)
        : 0;
#endif //always leave poisoned redzones between allocations under ASan
}

static inline void PCB__Arena_store_meta(
    PCB_Arena_Prefix* a, void* data, size_t size, size_t pad
) {
    PCB_Arena_Alloc_Meta* meta = &a->last;
    if(a->flags & PCB_ARENA_FLAG_ALLOC_META)
        meta = (PCB_Arena_Alloc_Meta*)data;
    meta->size = size;
    meta->pad  = pad;
}

static void* PCB__Arena_realloc_new(
    PCB_Arena* arena, void* ptr, size_t size, PCB_Arena_Alloc_Meta* meta
) {
    void* new_ptr = NULL;
    if(meta->pad & (SIZE_MAX ^ (SIZE_MAX>>1))) { //explicit alignment was requested
        size_t alignment = 1;
        while((!((uintptr_t)ptr & alignment))) alignment <<= 1;
        new_ptr = PCB_Arena_aligned_alloc(arena, size*sizeof(void*), alignment);
    } else {
        new_ptr = PCB_Arena_alloc(arena, size*sizeof(void*));
    }
    if(new_ptr == NULL) return NULL;
    PCB_memcpy(new_ptr, ptr, meta->size*sizeof(void*));
    return new_ptr;
}

static inline void PCB__Arena_do_free(
    PCB_Arena_Prefix* a, size_t length, PCB_Arena_Alloc_Meta* meta
) {
    a->length = length - (meta->pad & ~(SIZE_MAX ^ (SIZE_MAX>>1)));
#ifdef __SANITIZE_ADDRESS__
    ASAN_POISON_MEMORY_REGION(PCB__Arena_cur(a), (a->capacity - a->length) * sizeof(void*));
#endif //ASan
    if(a->flags & PCB_ARENA_FLAG_ALLOC_META) //NOTE: Poisoned by the caller
        a->length -= sizeof(*meta)/sizeof(void*);
    else
        a->last.size = a->last.pad = 0;
}

//TODO: would be better to somehow use ASan for these.
static void PCB__Arena_diagnose_df(const char* func, void* ptr) {
    PCB_log(PCB_LOGLEVEL_FATAL, "%s: double free detected for %p.", func, ptr);
    abort();
}

static void PCB__Arena_diagnose_uaf(const char* func, void* ptr) {
    PCB_log(PCB_LOGLEVEL_FATAL, "%s: use-after-free detected for %p.", func, ptr);
    abort();
}

static inline bool PCB__Arena_try_shrink_buf(
    PCB_Arena_Prefix* a, size_t size, PCB_Arena_Alloc_Meta* meta
) {
    if(size > meta->size) return false;
    size_t diff = meta->size - size;
    meta->size -= diff; a->length -= diff;
#ifdef __SANITIZE_ADDRESS__
    ASAN_POISON_MEMORY_REGION(PCB__Arena_cur(a), diff*sizeof(void*));
#endif //ASan
    return true;
}

static inline bool PCB__Arena_try_expand_buf(
    PCB_Arena_Prefix* a, size_t size, PCB_Arena_Alloc_Meta* meta
) {
    size_t diff = size - meta->size;
    if(a->length + diff > a->capacity) return false;
#ifdef __SANITIZE_ADDRESS__
    ASAN_UNPOISON_MEMORY_REGION(PCB__Arena_cur(a), diff*sizeof(void*));
#endif //ASan
    meta->size += diff; a->length += diff;
    return true;
}

PCB_Arena* PCB_Arena_init(size_t size) {
    return PCB_Arena_init_ex(size, 0);
}

PCB_Arena* PCB_Arena_init_ex(size_t size, PCB_Arena_Flags flags) {
    PCB_CHECK(size == 0, NULL);
    size_t capacity = 1;
    while(capacity < size) capacity *= 2;
    capacity += sizeof(PCB_Arena_Prefix);
    void* mem = PCB_realloc(NULL, capacity);
    if(mem == NULL) {
#if PCB_PLATFORM_WINDOWS
        SetLastError(0);
#endif
        return NULL;
    }
    return PCB_Arena_init_in_ex(mem, capacity, flags);
}

PCB_Arena* PCB_Arena_init_in(void* mem, size_t memsize) {
    return PCB_Arena_init_in_ex(mem, memsize, 0);
}

PCB_Arena* PCB_Arena_init_in_ex(void* mem, size_t memsize, PCB_Arena_Flags flags) {
    PCB_CHECK_NULL(mem, NULL);
    if(memsize <= sizeof(PCB_Arena_Prefix)) return NULL;
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)mem;
    a->length    = 0;
    a->capacity  = (memsize - sizeof(*a)) / sizeof(void*);
    a->next      = NULL;
    a->flags     = flags;
    a->last.size = a->last.pad = 0;
#ifdef __SANITIZE_ADDRESS__
    ASAN_POISON_MEMORY_REGION(PCB__Arena_start(a), a->capacity*sizeof(void*));
#endif //ASan
    return (PCB_Arena*)a;
}

void* PCB_Arena_alloc(PCB_Arena* arena, size_t size) {
    PCB_CHECK_SELF(arena, NULL);
    size = PCB__Arena_ceil(size);
    if(size == 0) return NULL;

    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    size /= sizeof(void*);
    const size_t meta = PCB__Arena_meta_size(a);
try_alloc:
    if(a->length + size + meta > a->capacity) {
        if(a->next != NULL)  {
            a = (PCB_Arena_Prefix*)a->next;
            goto try_alloc;
        }
        size_t capacity = a->capacity;
        while(capacity < size + meta) capacity *= 2;
        a->next = PCB_Arena_init_ex(capacity*sizeof(void*), a->flags);
        if(a->next == NULL) return NULL;
        a = (PCB_Arena_Prefix*)a->next;
    }
    void* data = PCB__Arena_cur(a);
#ifdef __SANITIZE_ADDRESS__
    ASAN_UNPOISON_MEMORY_REGION(data, (size + meta) * sizeof(void*));
#endif //ASan
    PCB__Arena_store_meta(a, data, size, 0);
#ifdef __SANITIZE_ADDRESS__
    ASAN_POISON_MEMORY_REGION(data, meta*sizeof(void*));
#endif //ASan
    a->length += size + meta;
    return (char*)data + meta*sizeof(void*);
}

void* PCB_Arena_zalloc(PCB_Arena* arena, size_t size) {
    void* mem = PCB_Arena_alloc(arena, size);
    if(mem != NULL) PCB_memset(mem, 0, size);
    return mem;
}

void* PCB_Arena_aligned_alloc(PCB_Arena* arena, size_t size, size_t alignment) {
    PCB_CHECK_SELF(arena, NULL);
    if(alignment == 0) return NULL;
    const bool pow2 = (alignment & (alignment - 1)) == 0;
    if(!pow2) return NULL;
    if(alignment % sizeof(void*) != 0) return NULL;
    size = PCB__Arena_ceil(size);
    if(size == 0) return NULL;

    alignment /= sizeof(void*); //normalize to
    size      /= sizeof(void*); //internal units
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    const size_t meta = PCB__Arena_meta_size(a);
    void* data; size_t pad;
try_alloc:
    data = PCB__Arena_cur(a);
    pad = PCB__Arena_pad((uintptr_t)data / sizeof(void*), alignment, meta);
    if(a->length + size + pad > a->capacity) {
        if(a->next != NULL)  {
            a = (PCB_Arena_Prefix*)a->next;
            goto try_alloc;
        }
        size_t capacity = a->capacity;
        while(capacity < size + meta + alignment) capacity *= 2;
        a->next = PCB_Arena_init_ex(capacity*sizeof(void*), a->flags);
        if(a->next == NULL) return NULL;
        a = (PCB_Arena_Prefix*)a->next;
        goto try_alloc;
    }
    data = (char*)data + (pad - meta) * sizeof(void*); //sets MSB
#ifdef __SANITIZE_ADDRESS__
    ASAN_UNPOISON_MEMORY_REGION(data, (size + meta) * sizeof(void*));
#endif //ASan
    PCB__Arena_store_meta(a, data, size, (pad - meta) | (SIZE_MAX ^ (SIZE_MAX>>1)));
#ifdef __SANITIZE_ADDRESS__
    ASAN_POISON_MEMORY_REGION(data, meta*sizeof(void*));
#endif //ASan
    a->length += size + pad;
    return (char*)data + meta*sizeof(void*);
}

void* PCB_Arena_aligned_zalloc(PCB_Arena* arena, size_t size, size_t alignment) {
    void* mem = PCB_Arena_aligned_alloc(arena, size, alignment);
    if(mem != NULL) PCB_memset(mem, 0, size);
    return mem;
}

bool PCB_Arena_alloc_whole(PCB_Arena* arena, void** ptr, size_t* size) {
    PCB_CHECK_SELF(arena, false);
    if(size == NULL) return false;
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;

    *size = 0;
    if(ptr != NULL) *ptr = NULL;
    const size_t meta = PCB__Arena_meta_size(a);
    const size_t end  = a->length + meta;
    if(end > a->capacity) return false;

    size_t s = a->capacity - end;
    *size = s * sizeof(void*);
    if(ptr != NULL && s > 0) {
        void* data = PCB__Arena_cur(a);
#ifdef __SANITIZE_ADDRESS__
        ASAN_UNPOISON_MEMORY_REGION(data, (s + meta) * sizeof(void*));
#endif //ASan
        PCB__Arena_store_meta(a, data, s, 0);
#ifdef __SANITIZE_ADDRESS__
        ASAN_POISON_MEMORY_REGION(data, meta*sizeof(void*));
#endif //ASan
        a->length = a->capacity;
        *ptr = (char*)data + meta*sizeof(void*);
    }
    return true;
}

PCB_Arena* PCB_Arena_next(PCB_Arena* arena) {
    PCB_CHECK_SELF(arena, NULL);
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    return a->next;
}

size_t PCB_Arena_allocated(PCB_Arena* arena) {
    PCB_CHECK_SELF(arena, (size_t)-1);
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    return a->length * sizeof(void*);
}

size_t PCB_Arena_allocatable(PCB_Arena* arena) {
    PCB_CHECK_SELF(arena, (size_t)-1);
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    return (a->capacity - a->length) * sizeof(void*);
}

size_t PCB_Arena_capacity(PCB_Arena* arena) {
    PCB_CHECK_SELF(arena, (size_t)-1);
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    return a->capacity * sizeof(void*);
}

//These are implemented separately because I don't trust the compiler to use tail recursion.
size_t PCB_Arena_allocated_all(PCB_Arena* arena) {
    PCB_CHECK_SELF(arena, (size_t)-1);
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    size_t allocated = 0;
    do {
        allocated += a->length * sizeof(void*);
        a = (PCB_Arena_Prefix*)a->next;
    } while(a != NULL);
    return allocated;
}

size_t PCB_Arena_allocatable_all(PCB_Arena* arena) {
    PCB_CHECK_SELF(arena, (size_t)-1);
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    size_t allocatable = 0;
    do {
        allocatable += (a->capacity - a->length) * sizeof(void*);
        a = (PCB_Arena_Prefix*)a->next;
    } while(a != NULL);
    return allocatable;
}

size_t PCB_Arena_capacity_all(PCB_Arena* arena) {
    PCB_CHECK_SELF(arena, (size_t)-1);
    PCB_Arena_Prefix* a = (PCB_Arena_Prefix*)arena;
    size_t capacity = 0;
    do {
        capacity += a->capacity * sizeof(void*);
        a = (PCB_Arena_Prefix*)a->next;
    } while(a != NULL);
    return capacity;
}

PCB_Arena_Flags PCB_Arena_flags(PCB_Arena* arena) {
    PCB_CHECK_SELF(arena, 0);
    return ((PCB_Arena_Prefix*)arena)->flags;
}

bool PCB_Arena_enable_allocMeta(PCB_Arena* arena, bool enable) {
    PCB_CHECK_SELF(arena, false);
    PCB__Arena_forEach_node(current, next) {
        if(current->length != 0) return false;
    }
    PCB__Arena_forEach_node(current, next) {
        if(enable)
            current->flags |=  PCB_ARENA_FLAG_ALLOC_META;
        else //how come does ~ change type to SIGNED int?!
            current->flags &= (PCB_Arena_Flags)~PCB_ARENA_FLAG_ALLOC_META;
    }
    return true;
}

PCB_Arena_Mark* PCB_Arena_mark(PCB_Arena* arena) {
    PCB_CHECK_SELF(arena, NULL);

    size_t i = PCB__Arena_nodes((PCB_Arena_Prefix*)arena);
    PCB_Arena_Mark* mark = (PCB_Arena_Mark*)PCB_Arena_alloc(
        arena, sizeof(mark->length) + i * sizeof(mark->lengths[0])
    );
    if(mark == NULL) return NULL;

    mark->length = i; i = 0;
    PCB__Arena_forEach_node(current, next) {
        mark->lengths[i++] = current->length;
    }
    return mark;
}

bool PCB_Arena_restore_to(PCB_Arena* arena, PCB_Arena_Mark* mark) {
    PCB_CHECK_SELF(arena, false);
    PCB_CHECK(mark == NULL, false);

    PCB_Arena_Prefix* marknode = PCB__Arena_marknode(arena, mark);
    if(marknode == NULL) return false; //likely user bug, maybe issue a diagnostic?
    PCB__Arena_restore(arena, mark);
    return true;
}

bool PCB_Arena_restore(PCB_Arena* arena, PCB_Arena_Mark* mark) {
    PCB_CHECK_SELF(arena, false);
    PCB_CHECK(mark ==  NULL, false);

    const size_t marklen = sizeof(mark->length) + mark->length * sizeof(*mark->lengths);
    PCB_Arena_Prefix* marknode = PCB__Arena_marknode(arena, mark);
    if(marknode == NULL) return false; //likely user bug, maybe issue a diagnostic?
    PCB__Arena_restore(arena, mark);
    //dealloc `mark` since we know it's the last thing that was allocated
    marknode->length -= marklen / sizeof(void*);
#ifdef __SANITIZE_ADDRESS__
    ASAN_POISON_MEMORY_REGION(PCB__Arena_cur(marknode), marklen);
#endif //ASan
    //dealloc metadata before `mark`, if any
    const size_t meta = PCB__Arena_meta_size(marknode);
    marknode->length -= meta;
#ifdef __SANITIZE_ADDRESS__
    ASAN_POISON_MEMORY_REGION(PCB__Arena_cur(marknode), meta*sizeof(void*));
#endif //ASan
    return true;
}

void PCB_Arena_free(PCB_Arena* arena, void* ptr) {
    PCB_CHECK_SELF(arena,);
    if(ptr == NULL) return;
    PCB_Arena_Prefix* a = PCB__Arena_ptrnode(arena, ptr);
    if(a == NULL) return; //likely user bug, maybe issue a diagnostic?

    size_t length = PCB__Arena_offsetof(a, ptr);
    if(length >= a->length) { PCB__Arena_diagnose_df(__func__, ptr); return; }
    PCB_Arena_Alloc_Meta* meta = &a->last;
    if(a->flags & PCB_ARENA_FLAG_ALLOC_META) {
        meta = (PCB_Arena_Alloc_Meta*)ptr - 1;
#ifdef __SANITIZE_ADDRESS__
        ASAN_UNPOISON_MEMORY_REGION(meta, sizeof(*meta));
#endif //ASan
    }
    //`ptr` was the last thing allocated, we can actually deallocate it.
    //Otherwise ignored.
    if(length + meta->size == a->length) PCB__Arena_do_free(a, length, meta);
#ifdef __SANITIZE_ADDRESS__
    if(a->flags & PCB_ARENA_FLAG_ALLOC_META)
        ASAN_POISON_MEMORY_REGION(meta, sizeof(*meta));
#endif //ASan
}

void* PCB_Arena_realloc(PCB_Arena* arena, void* ptr, size_t size) {
    if(ptr == NULL) return PCB_Arena_alloc(arena, size);
    PCB_CHECK_SELF(arena, NULL);

    size = PCB__Arena_ceil(size);
    if(size == 0) {
        PCB_log( //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3621.txt
            PCB_LOGLEVEL_WARN, //recommends issuing a diagnostic.
            "%s called with size 0 and non-null pointer. "
            "ISO C does not standardize behavior of realloc functions for "
            "this combination of inputs. "
            "Will return null without freeing. Don't rely on this behavior.",
            __func__
        ); return NULL;
    }
    PCB_Arena_Prefix* a = PCB__Arena_ptrnode(arena, ptr);
    if(a == NULL) return NULL; //likely a user bug, maybe issue a diagnostic?

    //We have no way of knowing how much data to copy to a new allocation
    //if it has to be moved. Bail.
    if(!(a->flags & PCB_ARENA_FLAG_ALLOC_META)) return NULL;

    size /= sizeof(void*);
    size_t length = PCB__Arena_offsetof(a, ptr);
    if(length >= a->length) { PCB__Arena_diagnose_uaf(__func__, ptr); return NULL; }
    PCB_Arena_Alloc_Meta* meta = (PCB_Arena_Alloc_Meta*)ptr - 1;
#ifdef __SANITIZE_ADDRESS__
    ASAN_UNPOISON_MEMORY_REGION(meta, sizeof(*meta));
#endif //ASan
    void* result;

    if(meta->size == size) PCB__return_defer(ptr);
    if(length + meta->size != a->length) { //Not the last thing allocated.
        //We can't shrink without introducing a hole, just leak the difference.
        if(meta->size > size) PCB__return_defer(ptr);
        //"Extend" by allocating new memory.
        //This leaks `ptr`, but nobody sane is going to use a linear
        //allocator with arbitrary lifetimes...right?
        PCB__return_defer(PCB__Arena_realloc_new(arena, ptr, size, meta));
    }
    if(PCB__Arena_try_shrink_buf(a, size, meta)) PCB__return_defer(ptr);
    if(PCB__Arena_try_expand_buf(a, size, meta)) PCB__return_defer(ptr);
    result = PCB__Arena_realloc_new(arena, ptr, size, meta);
    if(result == NULL) goto defer;
    PCB__Arena_do_free(a, length, meta);
defer:
#ifdef __SANITIZE_ADDRESS__
    ASAN_POISON_MEMORY_REGION(meta, sizeof(*meta));
#endif //ASan
    return result;
}

void PCB_Arena_reset(PCB_Arena* arena) {
    PCB_CHECK_SELF(arena, );
    PCB__Arena_forEach_node(current, next) {
        current->length = 0;
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
    PCB_CHECK_SELF(arena, NULL);
    if(fmt == NULL) return NULL;

    va_list args;
    va_start(args, fmt);
    char* str = PCB_Arena_vasprintf(arena, fmt, args);
    va_end(args);
    return str;
}

char* PCB_Arena_vasprintf(PCB_Arena* arena, const char* fmt, va_list ap) {
    PCB_CHECK_SELF(arena, NULL);
    if(fmt == NULL) return NULL;

    va_list args;
    va_copy(args, ap);
    const size_t req = (unsigned int)PCB_vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);

    char* text = (char*)PCB_Arena_alloc(arena, req);
    if(text == NULL) return NULL;
    va_copy(args, ap);
    const size_t printed = (unsigned int)PCB_vsnprintf(text, req, fmt, args);
    PCB_assert(printed + 1 == req);
    va_end(args);
    return text;
}
#else
char* PCB_Arena_asprintf(PCB_Arena* arena, const char* fmt, ...) {
    (void)arena; (void)fmt;
    return NULL;
}

char* PCB_Arena_vasprintf(PCB_Arena* arena, const char* fmt, va_list ap) {
    (void)arena; (void)fmt; (void)ap;
    return NULL;
}
#endif //PCB_HAS_STDIO_H?

char* PCB_Arena_strdup(PCB_Arena* arena, const char* str) {
    PCB_CHECK_SELF(arena, NULL);
    if(str == NULL) return NULL;
    size_t len = PCB_strlen(str) + 1; // '\0'
    char* text = (char*)PCB_Arena_alloc(arena, len);
    if(text == NULL) return NULL;
    PCB_memcpy(text, str, len);
    return text;
}

char* PCB_Arena_strndup(PCB_Arena* arena, const char* str, size_t n) {
    PCB_CHECK_SELF(arena, NULL);
    if(str == NULL) return NULL;
    size_t len = PCB_strnlen(str, n); //           '\0'
    char* text = (char*)PCB_Arena_alloc(arena, len + 1);
    if(text == NULL) return NULL;
    PCB_memcpy(text, str, len);
    text[len] = '\0'; //`str` may not end with '\0'
    return text;
}

#undef PCB__Arena_forEach_node
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

//Section 3.7: build capability
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
#if PCB_COMPILER_GCC < 140000 && PCB_COMPILER_GCC >= 90000
      case 202000L: return "c2x";
#endif //gcc's "c2x", deprecated in GCC14
      case 202311L: return "c23";
#if   PCB_COMPILER_GCC >= 150000
      case 202500L: return "c2y";
#elif PCB_COMPILER_CLANG >= 190000
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
