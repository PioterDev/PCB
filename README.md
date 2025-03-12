# PCB - Pioter's C Builder
## What is PCB?
PCB is a header-only library for exploring the idea of
a "no build" build system.
#### Hold on. "no build" build system?
Yes. The idea of such thing is that you should not need anything other than a compiler to build your project.
This also means that you don't have to learn another
programming/scripting language just to get an executable.

## Features
PCB contains functionality not only related to building a project, but also for use as a general library.

Currently implemented features:
- identification of the target platform via PCB_PLATFORM_* macros (see section 1.1)
- identification of the compiler used via PCB_COMPILER_* macros (see section 1.2)
- nice-to-have-but-compiler-specific functionality like marking functions as deprecated, force inlining and others (see section 1.3)
- macros for dynamic arrays of any type via PCB_Vec_* (see section 1.6.1)
- logging with <span style="color:red">c</span><span style="color:orange">o</span><span style="color:yellow">l</span><span style="color:green">o</span><span style="color:blue">r</span><span style="color:violet">s</span> (see section 2.1)
- platform-independent mkdir (more filesystem functionalities sometime in the future) (see section 2.2)
- dynamic strings (for now only ASCII is supported, UTF-8 support is *planned*) (see section 2.3)
- platform-independent running shell commands (not fully implemented) (see section 2.4)

PCB's functionality for building a project is disabled by default. To enable it, simply
`#define PCB_BUILD_CAPABILITY` before `#include`'ing it. This will enable certain macros, variables and functions. See sections:
- actually don't, nothing is implemented yet

Certain functions have extended documentation. See Appendix 1 for details.

The changelog is present in Appendix 2.