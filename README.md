# PCB

> [!WARNING]
> This project is in version 0.X, meaning that the API is not yet fully stable and breaking changes are still likely.
> Use at your own risk!

PCB is a single-header library written in C99 for improved development ergonomics.
It was originally meant for exploration of the idea of a "no build" build system,
largely influenced by [nob.h](https://github.com/tsoding/nob.h), but has since
expanded to include many more low-level utilities not directly adjacent to building.

PCB maintains **full** compatibility with all versions of C and C++ starting from C99 and C++11, respectively.
You can include *and compile* this library as either C or C++ in any of those versions.

PCB is highly portable: it only dependens on POSIX.1 (POSIX-compatible systems only),
WinAPI (Windows only), libc (optional) and a C99/C++11 compiler.

> [!NOTE]
> PCB does not currently support automatic search for MSVC. You need to provide its required environment yourself during bootstrapping and run the program at least once.

## Features
### Existing
- compiler detection done **correctly** (`#ifdef __GNUC__` *does not* imply GCC, for example)
- nicer platform & architecture detection
- support for a wide range of compiler extensions that are usable portably
- support for generic dynamic arrays, slices and views (immutable slices)
- extensive string subsystem
- filesystem API that works on both POSIX and Windows and doesn't make you go insane
- arena allocators
  - supports realloc
  - supports LIFO alloc/free
  - with mark/restore and chaining
  - detects leaks/use-after-frees/double-frees
  - integrated with ASan
- **logic for building C/C++ projects**

### Work In Progress
- logic for building C/C++ projects
- subprocess management (currently breaks with Unicode on Windows, of course it does)
- polymorphic allocators
- concurrent pool allocator
- logging on Windows

    It's annoying that this is even here. You see, Windows' C runtime completely
    breaks down on Unicode when using narrow `printf` functions, but because
    *you* as the user don't want to deal with this nonsense and `wprintf`
    functions *also* don't work with UTF-8 (unless in a locale that is not supported
    on older systems), *we* will do the heavy lifting.

    tl;dr implementation of printf

### Planned
- pkg-config parser (to remove reliance on the `pkg-config` program)
- minimal (or not) C preprocessor for source file dependency management

    Maybe, *maybe*, also support for C++20 modules. The problem is that parsing
    C++ *in any capacity* is already a success due to its grammar being
    undecidable, let alone doing it *correctly*.
- dedicated *binary* file format for storing the above
- command line parsing sublibrary
- support for other generic data structures like linked lists & hash tables
- cross-platform filesystem monitoring (think inotify(7))
- *basic* multithreading

### Won't implement/Not-a-goal
- non-basic multitasking

    Already WIP as a separate library.
    This also means that any kind of job system beyond simple command scheduling
    is *not* going to be implemented. Do that yourself.

- support for file formats/protocols not strictly required for building

    Something like an JSON parser or WebSockets already are already implemented
    as separate libraries. Writing it again is wasted effort.

# Quick start
Clone this repository or download the raw `PCB.h` file.
In some file, which in the same directory as `PCB.h` (suppose `b.c`):
```c
#define PCB_IMPLEMENTATION
#include "PCB.h"

int main(int argc, char *argv[]) {
    PCB_REBUILD_THIS_SHIT(argc, argv);

    const char *prog = PCB_SHIFT(argc, argv);
    bool force_rebuild = false;
    bool static_lib = false;
    while(argc > 0) { //quick & dirty argument parsing
        const char *flag = PCB_SHIFT(argc, argv);
        if(!strcmp(flag, "-f")) force_rebuild = true;
        else if(!strcmp(flag, "--static")) static_lib = true;
        else {
            PCB_log(PCB_LOGLEVEL_ERROR, "Unknown option '%s'", flag);
            PCB_log(PCB_LOGLEVEL_INFO, "Usage: %s [-f] [--static]", prog);
            return 1;
        }
    }

    PCB_BuildContext context = PCB_ZEROED; //always zero-initialize!
    PCB_BuildContext_init(
        &context,
        PCB_BUILDOPTION_DEFAULT_COMPILER | //uses compiler used when compiling the library
        PCB_BUILDOPTION_DEFAULT_WARNINGS |
        PCB_BUILDOPTION_OPTIMIZE |
        // PCB_BUILDOPTION_NODEBUG | //on by default
        0 //trailing 0 is so you can quickly comment out options
    );
    PCB_CStrings_append(&context.sources, "PCB.h");
    PCB_CStrings_append(&context.includes, ".");
    //Some compilers need 2 separate flags, some need just one.
    PCB_CStringPair fp = PCB_build_flag_treat_inputs_as_c(context.compiler.kind);
    //If the compiler is unrecognized by PCB, both are empty strings.
    if(*fp.key)   PCB_CStrings_append(&context.otherCompilerFlags, fp.key);
    if(*fp.value) PCB_CStrings_append(&context.otherCompilerFlags, fp.value);
    PCB_CStringPair impl_macro = {"PCB_IMPLEMENTATION", NULL};
    PCB_CStringPairs_append(&context.preprocessorFlags.defines, impl_macro);
#if PCB_PLATFORM_WINDOWS
    if(!static_lib) {
        //Windows requires __declspec(dllexport) for DLL symbols when compiling,
        //which is enabled via this macro.
        //Make sure to define `PCB_DYN` when using PCB as a DLL, otherwise
        //__declspec(dllimport) won't be used.
        PCB_CStringPair build_dyn_macro = {"PCB_BUILD_DYN", NULL};
        PCB_CStringPairs_append(&context.preprocessorFlags.defines, build_dyn_macro);
    }
#endif //This isn't required outside of Windows
    PCB_BuildContext_flags(&context).buildType = static_lib
        ? PCB_BUILDTYPE_STATICLIB : PCB_BUILDTYPE_DYNAMICLIB;
    PCB_BuildContext_flags(&context).alwaysBuild = force_rebuild;
    context.outputPath = "PCB";
    PCB_Status result = PCB_build_fromContext(&context);
    if(!PCB_ISOK(result)) {
        PCB_log(PCB_LOGLEVEL_ERROR, "Failed to build %s!", context.outputPath);
        goto defer;
    }
    //Here would be any other logic. Omitted for brevity.
defer:
    PCB_BuildContext_destroy(&context);
    return PCB_ISOK(result) ? 0 : 1;
}
```
Compile `b.c` using a compiler of your choice, for example:
```console
cc -o b b.c
cl /std:c11 /Fe:b.exe b.c # needs appropriate environments variables
```
and run it:
```console
./b
b
```
You should see PCB built as a dynamic library (`libPCB.so` on non-Mac POSIX platforms,
`libPCB.dylib` on MacOS, `PCB.dll` on Windows).
You can modify this code and rerun the program -- it will be automatically rebuilt.

# Why use PCB?
Why would you use it over other build systems like CMake or Autoconf?
Why would you write your build logic in a native programming language when scripts do the job?
Why would you use this unproven method for building your project?

While yes, CMake and others do work, but they are not *pleasant* to interact with.
If you break something while using these, can you quickly identify what is the problem every single time? Probably not.
This is because these tools try to hide the problem behind impenetrable abstraction layers, and when these layers inevitably fail, it blows up in your face.

In contrast, build systems as libraries are, well, libraries, compiled as part of the project.
Because of this, you can utilize the exact same development environment as for everything else.
When (*not* if!) something breaks, instead of guessing and/or scavenging the Internet for answers, you can use a debugger.

Writing build logic as code also has 3 significant upsides: maintainability, scalability and portability.

> What? How is C/C++ code more maintainable than a script?

Well, it isn't...*until it is*.
Scripts early on bring an illusion of being better suited for this task because it's easier to write them...initially.
Naturally, as the project grows, that script also grows.
At some point, however, this either hits a wall of portability,
the scripting environment not supporting some needed feature or the script becoming
disorganized/messy/otherwise hard to reason about, possibly because of the former.

C/C++ code, however, only needs to be concerned about existence of the standard library
on the host (which, let's be honest, *will* be present on every desktop),
system-specific APIs and a C/C++ compiler, i.e. the bare minimum set of dependencies.
Wait, aren't these the only dependencies of PCB?

What it doesn't need to be concerned about is existence of some specific command,
behavior of which can be implemented in the code itself or by bringing in a library
that implements it.
It's also not concerned about a specific version of the runtime environment (looking at you, CMake).
All libc, WinAPI and POSIX maintain strong backwards and forwards compatibility,
which means that build logic dependent on them will work far into the future with little additional maintenance.

C/C++ code also naturally scales better than any scripting language, both in terms of
managing complexity, performance and them not unexpectedly breaking because a variable was empty or something.
Just the fact that there's a type system checked at compile time makes a huge difference.

Lastly, using a language that supports a C FFI means that you're not confined to features provided by the script's runtime.
You can use other libraries for the build process.

Maybe your build process requires compressing assets? You can use zlib.
Maybe your build process involves making web requests to bring in dependencies? You can use curl.
Maybe your project is graphics-related and you want to have a fancy GUI for the build step for...some reason?
You can reuse rendering code from the main project and load it via runtime linking after it's compiled.
Or maybe you have the needs of Google and need a distributed system for building? You can do that as well.

That is to say, build logic written in native code can scale to arbitrary complexity,
unlike script-based systems which were never designed for that.

And no, it's not an "unproven" method. There are multiple other projects that decided on using
the same programming language for both business logic and building that business logic.
These include Zig and Jai, though both are tightly integrated with the language and/or its toolchain.
Nevertheless, the fact that multiple programmers working independently
implemented a similar approach should tell you something.
Do note that PCB's build system was developed without the knowledge of how
the aforementioned systems operate internally
(and as of writing this I still have no idea how they work lol).

# Why not use PCB?
There are, however, certain cases where using PCB (or other build-system-as-libraries) doesn't make sense.

If your project relies heavily on tooling provided by CMake or others, like managing project dependencies,
rewriting it into native code will prove difficult or even impossible while keeping feature parity.

You might also conclude that PCB lacks some non-trivial (i.e. not something you want to write yourself)
feature that stops you from using it over existing tooling, and that is fine.
However, you are kindly advised against concluding that such state of affairs will be forever.
Unlike existing toolchains, which have accumulated legacy and/or dug themselves into a hole by
being fundamentally limited in what they can do and can't easily improve,
this project and others like it can improve.

Or maybe you just like writing large scripts like a masochist. Who knows.

# Contributing
Contributions *made by humans* that meaningfully improve the project are welcome.

**Patches that are low quality, generated by an LLM without appropriate review ("vibe coded"),
made to stroke your own ego or that don't meaningfully improve the project are not allowed.
Do not waste maintainers' time.**
