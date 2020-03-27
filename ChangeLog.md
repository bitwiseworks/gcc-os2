History of changes for GCC for OS/2
===================================

This file contains OS/2-specific changes to GCC that are not part of upstream
and its ChangeLog.

9.2.0 OS/2 Beta 2 (2020-03-27)
------------------------------

-   OS/2: Fix upstream regression in response file processing [#14].

-   OS/2: Define wint_t to be signed int [#16].

-   Build against new LIBCn with full wide character supoprt to enable wchar_t
    specializations in C++ including supoprt for wide char I/O (wcout etc.),
    wregex etc. [#16].

9.2.0 OS/2 Beta 1 (2020-01-14)
------------------------------

-   Update source code to version 9.2.0.

-   OS/2: libstdc++: Force DLL name base to be stdcpp instead of stdc++.

-   OS/2: Add ctype_base::blank required by newer libstdc++.

-   OS/2: Better align optlink/system cconv to new GCC code.

-   OS/2: libquadmath: Fake missing putwc.

-   OS/2: Synchronize EMX code with recent Windows NT (Cygwin) changes.

-   OS/2: Make -mstackrealign effect default on SSE targets.

-   OS/2: Enable mmap in fixincludes.

-   OS/2: configure: Make --enable-decimal-float default on EMX.

-   OS/2: Implement OS/2-specific locale(ctype) support.

-   OS/2: Add C++11 thread support.

-   OS/2: fixincludes: Do not pass write_fd to the child.

-   OS/2: Install missing headers, especially C11 headers.

-   OS/2: Add missing built-in integer macros.

-   OS/2: libgcc: Add a custom list of exports for kLIBC (__chkstk_ms so far).

4.9.2 OS/2 Release 1 (2015-01-30)
---------------------------------

-   Update source code to version 4.9.2.

-   Use default compiler for building libgcc, fix def file name.

-   OS/2: Enable frame pointer by default in source due to configure bug.

-   Use standard toolchain to build gcc runtime also under OS/2.

-   Don't emit DWARF2 unwind tables on OS/2 by default.

4.7.3 OS/2 Release 1 (2013-11-27)
---------------------------------

-   Update source code to version 4.7.3.

4.4.6 OS/2 Release 17 (2013-07-25)
----------------------------------

-   Fix Release 16 regression that would break output file name computation for
    the `gcc test.c -lpthread` case.

4.4.6 OS/2 Release 16 (2013-07-23)
----------------------------------

-   Fix treatment of a non-option argument as an output file. This makes cases
    like `gcc -lpthread test.c` produce the correct executable name.

-   Enlarge stack size to 8MiB and use high memory in gccXXX.dll.

-   Implement -pipe mode support for the OS/2 (EMX) target.

-   Suppress `Internal error: Interrupt` message caused by Ctrl-C/Ctrl-Break.

-   Fix regression causing build failures in some C++ code due to mismatching
    internal representations of the correct member declaration and definition.
