/* Operating system specific defines to be used when targeting GCC for
   hosting on OS/2, using InnoTek LIBC and tools.
   Copyright (C) 2000(?)-2003 Andrew Zabolotny
   Copyright (C) 2003 InnoTek Systemberatung GmbH
   Copyright (C) 2003-2004 Knut St. Osmundsen
   Modified for GCC 4.x by Paul Smedley 2008-2013

This file is part of the InnoTek port of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* Use #ifdef EMX for issues needed by EMX target.
   Use #ifdef __EMX__ for issues needed on a EMX host.
   Note that the cross-compiler is untested  */
#ifndef EMX
# define EMX
#endif

/* Directories in path environment variables are separated by semicolons. */
#undef PATH_SEPARATOR
#define PATH_SEPARATOR          ';'

/* We do want to add an underscore to the front of each user symbol. */
#define YES_UNDERSCORES

#define DLL_IMPORT_EXPORT_PREFIX '#'

/* Debug formats */
#define DEFAULT_GDB_EXTENSIONS          1
#define DBX_DEBUGGING_INFO              1
#define DWARF2_DEBUGGING_INFO           1
#define PREFERRED_DEBUGGING_TYPE    DBX_DEBUG

/* headers */
#include <stdio.h>              /* for FILE* */

/* Some additional system-dependent includes... */
#include <sys/emxload.h>
#include <sys/uflags.h>

/* Generate stack probes for allocations bigger than 4000. */
#define CHECK_STACK_LIMIT               4000

/* OS/2 executables ends with .exe, but it's only enforced sometimes... */
#define TARGET_EXECUTABLE_SUFFIX        ".exe"
#define NO_FORCE_EXEOBJ_SUFFIX

/* The system headers are C++-aware. */
#define NO_IMPLICIT_EXTERN_C

/* Don't provide default values for __CTOR_LIST__ and __DTOR_LIST__ in libgcc */
#define CTOR_LISTS_DEFINED_EXTERNALLY

/* We support weak symbols (in the limited way OS/2 supports them).
   This also enables #pragma weak handling.  */
#define SUPPORTS_WEAK                   1

/* We support weak style link-once semantics.  */
#define SUPPORTS_ONE_ONLY               1

/* Enabling aliasing. This is wanted especially for weak externals with
   defaults. (#483) */
#define SET_ASM_OP                      "\t.set\t"

/* Support for C++ templates.  */
#undef MAKE_DECL_ONE_ONLY
#define MAKE_DECL_ONE_ONLY(DECL)        (DECL_WEAK (DECL) = 1)

/* This is how we tell the assembler that a symbol is weak.  */
#define ASM_WEAKEN_LABEL(FILE, NAME)    \
  do                                    \
    {                                   \
      fputs ("\t.weak\t", (FILE));      \
      assemble_name ((FILE), (NAME));   \
      fputc ('\n', (FILE));             \
    }                                   \
  while (0)


/* Define this macro if in some cases global symbols from one translation
   unit may not be bound to undefined symbols in another translation unit
   without user intervention.  For instance, under Microsoft Windows
   symbols must be explicitly imported from shared libraries (DLLs).  */
#define MULTIPLE_SYMBOL_SPACES          1

/* We want DWARF2 frame unwind info for exception handling */
#define DWARF2_UNWIND_INFO              1

#define TARGET_ASM_INIT_SECTIONS i386_emx_init_sections

/* Terminate DWARF frame unwind info with a closing zero */
#define DWARF2_FRAMEINFO_NULL_TERMINATED 1

/* Define the type for size_t. */
#define SIZE_TYPE                       "unsigned int"
#define PTRDIFF_TYPE                    "int"

/* wchar_t is unsigned short. */
#define WCHAR_TYPE                      "short unsigned int"
#define WCHAR_TYPE_SIZE                 16

/* We want the _System attribute */
#define TARGET_SYSTEM_DECL_ATTRIBUTES

/* We want the _Optlink attribute */
#define TARGET_OPTLINK_DECL_ATTRIBUTES

/* #631: Hack (temporary, will be fixed properly 3.4.x it seems) for _Optlink and
   _System functions which returns structures. (breaks some tcpip stuff.) */
#define RETURN_IN_MEMORY_WITH_FNTYPE    emx_return_in_memory_with_fntype

/* For Win32 ABI compatibility. */
#define DEFAULT_PCC_STRUCT_RETURN       0

/* Win32 complier aligns internal doubles in structures on dword boundaries.
   We need to emulate this behaviour for easier porting. */
#define BIGGEST_FIELD_ALIGNMENT         64

/* If bit field type is int, don't let it cross an int,
   and give entire struct the alignment of an int.  */
/* Required on the 386 since it doesn't have bitfield insns.  */
#define PCC_BITFIELD_TYPE_MATTERS       1

/* Don't continue symbols.  I think all relevant tools can handle
   symbols of arbitrary length.  GAS does not correctly handle
   continued symbols: it sometimes emits another symbol before the
   continuation. */
#define DBX_CONTIN_LENGTH               0

/* If defined, a C expression whose value is a string containing the
   assembler operation to identify the following data as
   uninitialized global data.  If not defined, and neither
   `ASM_OUTPUT_BSS' nor `ASM_OUTPUT_ALIGNED_BSS' are defined,
   uninitialized global data will be output in the data section if
   `-fno-common' is passed, otherwise `ASM_OUTPUT_COMMON' will be
   used.  */
#undef BSS_SECTION_ASM_OP
#define BSS_SECTION_ASM_OP              "\t.bss"

/* A C statement (sans semicolon) to output to the stdio stream
   FILE the assembler definition of uninitialized global DECL named
   NAME whose size is SIZE bytes and alignment is ALIGN bytes.
   Try to use asm_output_aligned_bss to implement this macro.  */
#define ASM_OUTPUT_ALIGNED_BSS(FILE, DECL, NAME, SIZE, ALIGN) \
  asm_output_aligned_bss (FILE, DECL, NAME, SIZE, ALIGN)

/* This is how to output an assembler line that says to advance the
   location counter to a multiple of 2**LOG bytes.
   bird: Pad using int 3. */
#undef ASM_OUTPUT_ALIGN
#define ASM_OUTPUT_ALIGN(FILE,LOG) \
    if ((LOG)!=0) fprintf ((FILE), "\t.align %d,0xcc\n", LOG)

/* Output a reference to a label.
   We're doing all here since we must get the '*' vs. user_label_prefix
   bit right. */
#if 1
#undef ASM_OUTPUT_LABELREF
#define  ASM_OUTPUT_LABELREF(STREAM, NAME)	\
do {						\
  if (((NAME)[0] != FASTCALL_PREFIX) && ((NAME)[0] != '*'))		\
    fputs (user_label_prefix, (STREAM));	\
  fputs ((NAME), (STREAM));			\
} while (0)
#else
#undef ASM_OUTPUT_LABELREF
#define ASM_OUTPUT_LABELREF(FILE,NAME) \
  do { \
    const char *xname = (NAME); \
    if (*xname == '%') \
      xname += 2; \
    if (*xname == '*') \
      xname += 1; \
    if (*xname == '*') \
      xname += 1; \
    else \
      fputs (user_label_prefix, FILE); \
    fputs (xname, FILE); \
  } while (0)
#endif


/* Get tree.c to declare a target-specific specialization of
   merge_decl_attributes.  */
#define TARGET_DLLIMPORT_DECL_ATTRIBUTES 1

#undef SUBTARGET_ENCODE_SECTION_INFO
#define SUBTARGET_ENCODE_SECTION_INFO  i386_emx_encode_section_info
#undef TARGET_ENCODE_SECTION_INFO
#define TARGET_ENCODE_SECTION_INFO  i386_emx_encode_section_info

#if 0 /* useful for hacking declspec attributes */
#define EMX_DBG_LOC_DECL , const char *function,  const char *file, unsigned line
#define EMX_DBG_LOC_INARGS , __FUNCTION__, __FILE__, __LINE__
#define EMX_DBG_LOC_RARGS function, file, line
#else
#define EMX_DBG_LOC_DECL
#define EMX_DBG_LOC_INARGS
#define EMX_DBG_LOC_RARGS "", "", 0
#endif


/* Use `#' instead of `/' as assembler comments */
#undef ASM_COMMENT_START
#define ASM_COMMENT_START               "#"
#undef ASM_APP_ON
#define ASM_APP_ON                      "#APP\n"
#undef ASM_APP_OFF
#define ASM_APP_OFF                     "#NO_APP\n"

/* Output a common block.  */
#if 1
#undef ASM_OUTPUT_ALIGNED_DECL_COMMON
#define ASM_OUTPUT_ALIGNED_DECL_COMMON(STREAM, DECL, NAME, SIZE, ALIGN)	\
do {							\
      i386_emx_maybe_record_exported_symbol (DECL, NAME, 1 EMX_DBG_LOC_INARGS);	\
							\
      /* 16 is the best we can do (segment para). */    \
      const int xalign = (ALIGN) > 16 ? 16 : (ALIGN);   \
      fprintf ((STREAM), "\t.comm\t"); 			\
      assemble_name ((STREAM), (NAME));			\
      fprintf ((STREAM), ", %d\t%s %d\n",		\
	       (((SIZE) + xalign - 1) / xalign) * xalign, \
               ASM_COMMENT_START, (SIZE));	        \
} while (0)
#else
#undef ASM_OUTPUT_ALIGNED_DECL_COMMON
#define ASM_OUTPUT_ALIGNED_DECL_COMMON \
  i386_emx_asm_output_aligned_decl_common
#endif

/* Output the label for an initialized variable.  */
#undef ASM_DECLARE_OBJECT_NAME
#define ASM_DECLARE_OBJECT_NAME(STREAM, NAME, DECL)	\
do {							\
  i386_emx_maybe_record_exported_symbol (DECL, NAME, 1);	\
  ASM_OUTPUT_LABEL ((STREAM), (NAME));			\
} while (0)

/* Track exported functions... */
#undef ASM_DECLARE_FUNCTION_NAME
#define ASM_DECLARE_FUNCTION_NAME(FILE, NAME, DECL)			\
  do									\
    {									\
      i386_emx_maybe_record_exported_symbol (DECL, NAME, 0);		\
      ASM_OUTPUT_LABEL (FILE, NAME);					\
    }									\
  while (0)

/* Output function declarations at the end of the file.  */
#undef TARGET_ASM_FILE_END
#define TARGET_ASM_FILE_END i386_emx_file_end

#if 1
/* This macro gets just the user-specified name out of the string
   in a SYMBOL_REF. Discard trailing @[NUM] encoded by ENCODE_SECTION_INFO.
   This is used to generate unique section names from function names
   (if -ffunction-sections is given).  */
#undef  TARGET_STRIP_NAME_ENCODING
#define TARGET_STRIP_NAME_ENCODING      emx_strip_name_encoding_full
#endif
#define TARGET_VALID_DLLIMPORT_ATTRIBUTE_P i386_emx_valid_dllimport_attribute_p
#define TARGET_CXX_ADJUST_CLASS_AT_DEFINITION i386_emx_adjust_class_at_definition


/* External function declarations.  */

extern int emx_c_set_decl_assembler_name PARAMS ((tree, int));

extern int emx_return_in_memory_with_fntype PARAMS ((tree type, tree fntype));
extern const char * emx_strip_name_encoding_full PARAMS ((const char *));
extern void emx_eh_frame_section (void);
extern void emx_output_function_begin_epilogue (FILE *);

/* These functions are for handling dllexport and dllimport. */
extern void i386_emx_record_exported_symbol (const char *, int);
extern void i386_emx_file_end (void);
extern void i386_emx_encode_section_info PARAMS ((tree, rtx, int));


/******************************************************************************
 *                       - S - W - I - T - C - H - E - S -
 ******************************************************************************/

/* Define an additional switch -m(no-)probe for backward compatibility.
   Also define the -m(no-)epilogue switch. */
#define MASK_EPILOGUE                   0x40000000  /* Use FUNCTION_EPILOGUE */

/* Masks for subtarget switches used by other files.  */
#define MASK_NOP_FUN_DLLIMPORT          0x08000000  /* Ignore dllimport for functions */

#undef SUBTARGET_SWITCHES
#define SUBTARGET_SWITCHES \
  { "probe",        MASK_STACK_PROBE, N_("Do stack probing") }, \
  { "no-probe",    -MASK_STACK_PROBE, N_("Don't do stack probing") }, \
  { "epilogue",     MASK_EPILOGUE,    N_("Generate prologue/epilogue labels") }, \
  { "no-epilogue", -MASK_EPILOGUE,    N_("Don't generate prologue/epilogue labels") }, \
  { "nop-fun-dllimport", MASK_NOP_FUN_DLLIMPORT, N_("Ignore dllimport for functions") }, \
  { "no-nop-fun-dllimport", -MASK_NOP_FUN_DLLIMPORT, "" },

/* Use FUNCTION_EPILOGUE */
#define TARGET_EPILOGUE                 (target_flags & MASK_EPILOGUE)

/* Used in winnt.c.  */
#define TARGET_NOP_FUN_DLLIMPORT        (target_flags & MASK_NOP_FUN_DLLIMPORT)

/* By default, target has a 80387, uses IEEE compatible arithmetic,
   and returns float values in the 387 and needs stack probes */
#undef TARGET_SUBTARGET_DEFAULT
#define TARGET_SUBTARGET_DEFAULT \
   (MASK_80387 | MASK_IEEE_FP | MASK_FLOAT_RETURNS | MASK_STACK_PROBE)


/* Don't allow flag_pic to propagate since gas may produce invalid code
   otherwise.  */

#undef  SUBTARGET_OVERRIDE_OPTIONS
#define SUBTARGET_OVERRIDE_OPTIONS					\
do {									\
  if (flag_pic)								\
    {									\
      warning (0, "-f%s ignored for target (all code is position independent)",\
	       (flag_pic > 1) ? "PIC" : "pic");				\
      flag_pic = 0;							\
    }									\
} while (0)								\

/* Add a __POST$xxx label before epilogue if -mepilogue specified */
#undef TARGET_ASM_FUNCTION_BEGIN_EPILOGUE
#define TARGET_ASM_FUNCTION_BEGIN_EPILOGUE emx_output_function_begin_epilogue

/* Output assembler code to FILE to increment profiler label # LABELNO
   for profiling a function entry.  */

#undef FUNCTION_PROFILER
#define FUNCTION_PROFILER(FILE, LABELNO)                                \
{                                                                       \
    fprintf (FILE, "\tcall __mcount\n");                                \
}


/******************************************************************************
 *
 *
 *                            - S - P - E - C - S -
 *
 *
 ******************************************************************************/

#define TARGET_OS_CPP_BUILTINS()                                        \
  do                                                                    \
    {                                                                   \
        builtin_define ("_X86_=1");                                     \
        builtin_define ("__i386");                                      \
        builtin_define ("__i386__");                                    \
        builtin_define ("__32BIT__");                                   \
        builtin_define ("__OS2__");                                     \
        builtin_define ("__EMX__");                                     \
        builtin_define ("__MT__");                                      \
        builtin_assert ("system=unix");                                 \
        builtin_assert ("system=posix");                                \
        builtin_assert ("system=emx");                                  \
        builtin_define ("__stdcall=__attribute__((__stdcall__))");      \
	builtin_define ("__fastcall=__attribute__((__fastcall__))");	\
        builtin_define ("__cdecl=__attribute__((__cdecl__))");          \
        builtin_define ("_Optlink=__attribute__((__optlink__))");       \
        builtin_define ("_System=__attribute__((__system__))");         \
	builtin_define ("_Export=__attribute__((dllexport))");	        \
	builtin_define ("__declspec(x)=__attribute__((x))");		\
        if (!flag_iso)                                                  \
          {                                                             \
            builtin_define ("_stdcall=__attribute__((__stdcall__))");   \
	    builtin_define ("_fastcall=__attribute__((__fastcall__))");	\
            builtin_define ("_cdecl=__attribute__((__cdecl__))");       \
            builtin_define ("_Cdecl=__attribute__((__cdecl__))");       \
          }                                                             \
	builtin_define_std ("__KLIBC__=0");		\
	builtin_define_std ("__KLIBC_MINOR__=6");		\
	builtin_define_std ("__KLIBC_PATCHLEVEL__=3");		\
	builtin_define_std ("__KLIBC_VERSION__=0x00060002");		\
	builtin_define_std ("__INNOTEK_LIBC__=0x006");	\
	builtin_assert ("system=os2");	\
    }                                                                   \
  while (0)

#if 1
/*------------------------------------------------------------------------------
*   Current default specs which uses aout by default.
*-----------------------------------------------------------------------------*/

/* Use the stdc++ code linked into the libc dll when possible. */
#if 0 /* Use default values for these until this GCC is bundled with a libcxx build */
#define LIBSTDCXX                   "-lc_dll"
#endif
#define LIBSTDCXX_STATIC            "-lstdc++ -lgcc_eh"
#define LIBSTDCXX_PROFILE           LIBSTDCXX
#define LIBSTDCXX_PROFILE_STATIC    LIBSTDCXX_STATIC

/* Provide extra args to the C preprocessor and extra switch-translations.  */
#undef CPP_SPEC
#define CPP_SPEC                                                               \
  "%(cpp_cpu) "                                                                \
  "%{posix:-D_POSIX_SOURCE} "                                                  \
  "%{Zmt*:} %{Zlow-mem:} "                                                     \
  "%{pg:-D__GPROF__} "                                                         \
  "%{mepilogue:-D__EPILOGUE__} "                                               \
  "%{mprobe|mstack-arg-probe:-D__STACK_PROBE__} "

/* -s to -Zstrip and -Zomf must be passed down. */
#define ASM_SPEC "--traditional-format %{Zomf} %{s:-Zstrip}"

#define ASM_DEBUG_SPEC "%{g*:--gstabs}"


/* Here is the spec for running the linker, after compiling all files.  */

/* Provide extra args to the linker and extra switch-translations.  */
#define LINK_SPEC                                                              \
  "%{Zexe} %{Zstack*} %{Zlinker*} %{Zmap*} %{Zsym} %{Zdll} %{shared:-Zdll} %{static:-static}" \
  "%{!o*:-o %b%{Zdll|shared:.dll}%{!Zdll:%{!shared:%{!Zexe:.exe}}}} "          \
  "%{static:%{Zcrtdll*:%e-static and -Zcrtdll are incompatible}}"              \
  "%{Zomf:%{Zaout:%e-Zomf and -Zaout are incompatible}}"                       \
  "%{Zdll:%{Zexe:%e-Zdll and -Zexe are incompatible}}"                         \
  "%{shared:%{Zexe:%e-shared and -Zexe are incompatible}}"                     \
  "%{Zsmall-conv:%{Zcrtdll*:%e-Zsmall-conv and -Zcrtdll are incompatible}}"

/* Override how and when libgcc.a is included (%G).  */
#define LIBGCC_SPEC                                                            \
  "%{static|static-libgcc:%{shared-libgcc:%e-static-libgcc and -shared-libgcc are incompatible}}"\
  "%{shared-libgcc:-lgcc_so_d}"                                                \
  "%{!shared-libgcc:%{!static-libgcc:%{!static*:-lgcc_so_d}}}"                 \
  "%{!shared-libgcc:%{static-libgcc|static:-lgcc_eh -lgcc}}"

/* We have a shared libgcc, but don't need this extra handling. */
#undef ENABLE_SHARED_LIBGCC

/* Override the default libraries (%L).  */
#define LIB_SPEC                                                               \
  "-lc_alias "                                                                 \
  "%{!static:-lc_dll}"                                                         \
  "%{static:-lc%{p|pg:_p}_s %{!Zdll:%{!shared:-lc_app%{pg:_p}}}} "             \
  "-los2%{p|pg:_p}"

/* Override the default crt0 files.
   bird: we might consider kicking gcrt & mcrt stuff now... */
#define STARTFILE_SPEC                                                         \
  "%{Zno-high-mem:}%{Zfork:}%{Zunix:}"                                         \
  "%{Zdll|shared:dll0%{Zhigh-mem:hi}%{Zno-fork:nofork}%{Zno-unix:noux}%O%{Zomf:bj}%s}" \
  "%{!Zdll:%{!shared:%{pg:gcrt0%O%{Zomf:bj}%s}"                                          \
          "%{!pg:%{pn:gcrt0%O%{Zomf:bj}%s}"                                    \
                "%{!pn:%{p:mcrt0%O%{Zomf:bj}%s}"                               \
                      "%{!p:crt0%{Zhigh-mem:hi}%{Zno-fork:nofork}%{Zno-unix:noux}%{Zargs-wild:wild}%{Zargs-resp:resp}%O%{Zomf:bj}%s}}}}" \
  " %{Zbin-files:binmode%O%{Zomf:bj}%s}"                                       \
  " %{Zsmall-conv:smallcnv%O%{Zomf:bj}%s}}"

/* the -pthread flag is not recognized.  */
#undef GOMP_SELF_SPECS
#define GOMP_SELF_SPECS ""

#if 0
/* Prefer symbols in %L (-lc) over %G (libcAB.dll includes gcc3XY.dll).
   For the GNU linker we need to repeat everything as it doesn't
   restart the library search as the IBM and M$ linkers do. */
#define LINK_GCC_C_SEQUENCE_SPEC        "%L %G %L %{!Zomf:%G %L %G %L %G %L}"
#else
#define LINK_GCC_C_SEQUENCE_SPEC        "%G %L %{!Zomf:%G %L %G %L %G %L}"
#endif

#undef ENDFILE_SPEC
#define ENDFILE_SPEC                    "%{Zomf:-lend}"

/* Override the default linker program (collect2).  */
#define LINKER_NAME                     "%{Zomf:emxomf}ld.exe"



#else
/*------------------------------------------------------------------------------
*   As the above specs but defaults to OMF.
*   Note: this is not updated lately.
*-----------------------------------------------------------------------------*/

/* Predefine symbols. */
#undef CPP_PREDEFINES
#define CPP_PREDEFINES ""

/* Provide extra args to the C preprocessor and extra switch-translations.  */
#undef CPP_SPEC
#define CPP_SPEC                                                               \
  "%(cpp_cpu) "                                                                \
  "%{posix:-D_POSIX_SOURCE} "                                                  \
  "%{Zmt*:} %{Zlow-mem:} "                                                     \
  "%{pg:-D__GPROF__} "                                                         \
  "%{mepilogue:-D__EPILOGUE__} "                                               \
  "%{mprobe|mstack-arg-probe:-D__STACK_PROBE__} "

/* -s to -Zstrip and -Zomf must be passed down. */
#define ASM_SPEC "--traditional-format %{!Zaout:-Zomf} %{s:-Zstrip}"

#define ASM_DEBUG_SPEC "%{g*:--gstabs}"


/* Here is the spec for running the linker, after compiling all files.  */

/* Provide extra args to the linker and extra switch-translations.  */
#define LINK_SPEC                                                              \
  "%{Zexe} %{Zstack*} %{Zmap*} %{Zsym} %{!Zaout:%{Zdll}} "                     \
  "%{!o*:-o %b%{Zdll:.dll}%{!Zdll:%{!Zexe:.exe}}} "                            \
  "%{static:%{Zcrtdll*:%e-static and -Zcrtdll are incompatible}}"              \
  "%{Zomf:%{Zaout:%e-Zomf and -Zaout are incompatible}}"                       \
  "%{Zdll:%{Zexe:%e-Zdll and -Zexe are incompatible}}"                         \
  "%{Zsmall-conv:%{Zcrtdll*:%e-Zsmall-conv and -Zcrtdll are incompatible}}"

/* Override how and when libgcc.a is included (%G).  */
#define LIBGCC_SPEC                                                            \
  "%{static|static-libgcc:%{shared-libgcc:%e-static-libgcc and -shared-libgcc are incompatible}}"\
  "%{shared-libgcc:-lgcc%v1%v2%v3}"                                            \
  "%{!shared-libgcc:%{!static-libgcc:%{!static:-lgcc%v1%v2%v3}}}"              \
  "%{!shared-libgcc:%{static-libgcc|static:-lgcc_eh -lgcc}}"

/* We have a shared libgcc, but don't need this extra handling. */
#undef ENABLE_SHARED_LIBGCC

/* Override the default libraries (%L).  */
#define LIB_SPEC                                                               \
  "-lc_alias "                                                                 \
  "%{!static:-lc_dll}"                                                         \
  "%{static:-lc%{p|pg:_p}_s %{!Zdll:-lc_app%{pg:_p}}} -los2%{p|pg:_p}"

/* Override the default crt0 files.  */
#define STARTFILE_SPEC                                                         \
  "%{Zno-high-mem:}%{Zfork:}%{Zunix:}"                                         \
  "%{Zdll:dll0%{Zhigh-mem:hi}%{Zno-fork:nofork}%{Zno-unix:noux}%O%{!Zaout:bj}%s}"   \
  "%{!Zdll:%{pg:gcrt0%O%{!Zaout:bj}%s}"                                        \
          "%{!pg:%{pn:gcrt0%O%{!Zaout:bj}%s}%{!pn:%{p:mcrt0%O%{!Zaout:bj}%s}%{!p:crt0%{Zhigh-mem:hi}%{Zno-fork:nofork}%{Zno-unix:noux}%{Zargs-wild:wild}%{Zargs-resp:resp}%O%{!Zaout:bj}%s}}}"\
         " %{Zbin-files:binmode%O%{!Zaout:bj}%s} %{Zsmall-conv:smallcnv%O%{!Zaout:bj}%s}}"

/* Prefer symbols in %L (-lc) over %G (libcAB.dll includes gcc3XY.dll).
   For the GNU linker we need to repeate everything as it doesn't
   restart the library search as the IBM and M$ linkers do. */
#define LINK_GCC_C_SEQUENCE_SPEC "%L %G %L %{Zaout:%G %L %G %L %G %L}"

#define ENDFILE_SPEC "%{!Zaout:-lend}"

/* Override the default linker program (collect2).  */
#define LINKER_NAME "%{!Zaout:emxomf}ld.exe"

#endif

/******************************************************************************
 *                             - I - N - I - T -
 ******************************************************************************/
extern void emx_driver_init (unsigned int *,struct cl_decoded_option **);
#define GCC_DRIVER_HOST_INITIALIZATION \
        emx_driver_init (&decoded_options_count, &decoded_options)





/* The following will be defined only when compiling libgcc2
 * to avoid including large .h files when they are not needed.
 * Including them is neccessary to avoid 'implicit declatation'
 * warnings, and also for __write.
 */
#ifdef IN_LIBGCC2
/* define "write" and some other external functions for libgcc */
#define write __write

#include <malloc.h>
#include <alloca.h>
#include <time.h>
#include <string.h>

extern void __ctordtorInit (void);
extern void __ctordtorTerm (void);

/* Invoke static object constructors/destructors */
/* Also register/deregister frame unwind information */
#define DO_GLOBAL_CTORS_BODY __ctordtorInit ()
#define DO_GLOBAL_DTORS_BODY __ctordtorTerm ()

/* We call register_frame_table ourselves */
#define DONT_AUTOREGISTER_FRAME_INFO

#endif
