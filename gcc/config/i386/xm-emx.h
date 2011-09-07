/* Configuration for GNU C-compiler for hosting on OS/2.
   Using InnoTek GCC and InnoTek LIBC.
   Copyright (C) 2000(?)-2003 Andrew Zabolotny
   Copyright (C) 2003 InnoTek Systemberatung GmbH
   Copyright (C) 2003-2004 Knut St. Osmundsen

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


/* Handle drive separator in path names */
#define HAVE_DOS_BASED_FILE_SYSTEM

/* Directories in path environment variables are separated by semicolons. */
#undef PATH_SEPARATOR
#define PATH_SEPARATOR          ';'

/* OS/2 executables ends with '.exe' */
#undef HOST_EXECUTABLE_SUFFIX
#define HOST_EXECUTABLE_SUFFIX  ".exe"

/* Pathname components are separated by '/' or '\\'. */
#undef DIR_SEPARATOR
#define DIR_SEPARATOR           '/'
#undef DIR_SEPARATOR_2
#define DIR_SEPARATOR_2         '\\'

/* Tell GCC about OS/2's bit bucket. */
#define HOST_BIT_BUCKET         "NUL"

/* No block device special files on OS/2 */
#ifndef S_ISBLK
#define S_ISBLK(x)              0
#endif


/******************************************************************************
 *                             - P - A - T - H - S -
 ******************************************************************************/

/* (The /gcc/ prefix is rewritten to the real path when initalizing GCC.) */
#ifndef CROSS_COMPILE
#undef LOCAL_INCLUDE_DIR
#undef CROSS_INCLUDE_DIR
#undef TOOL_INCLUDE_DIR
#undef STANDARD_INCLUDE_DIR
#define STANDARD_INCLUDE_DIR            "/gcc/include/"
#define STANDARD_INCLUDE_COMPONENT      "GCC"
#define STANDARD_INCLUDE_SYSROOT        0
#undef LOCAL_INCLUDE_DIR
#undef PREFIX_INCLUDE_DIR
#undef TOOL_INCLUDE_DIR
#undef SYSTEM_INCLUDE_DIR
#endif /* not CROSS_COMPILE */

/* for prefix.c */
#define UPDATE_PATH_HOST_CANONICALIZE(path)                                    \
{                                                                              \
  char prefix_buffer [260 + 1], *c;                                            \
  _abspath (prefix_buffer, path, sizeof (prefix_buffer));                      \
  free (path);                                                                 \
  /* Way too often gcc generates double slashes, it looks bad */               \
  for (c = prefix_buffer; *c; c++)                                             \
    if ((c [0] == '/') && (c [1] == '/'))                                      \
      memmove (c, c + 1, strlen (c));                                          \
  path = xstrdup (prefix_buffer);                                              \
}


#if 0 /* move to emx.h? */
/******************************************************************************
 *                             - I - N - I - T -
 ******************************************************************************/

#define EMX_INITIALIZE_ENVIRONMENT(taildirs)                                   \
  /* Set env.vars GCC_ROOT, G++_ROOT and BINUTILS_ROOT.  */                    \
  {                                                                            \
  int fGCC = !getenv ("GCC_ROOT");                                             \
  int fGPP = !getenv ("G++_ROOT");                                             \
  int fBIN = !getenv ("BINUTILS_ROOT");                                        \
  if (fGCC || fGPP || fBIN)                                                    \
    {                                                                          \
      char root_path [260];                                                    \
      if (!_execname (root_path, sizeof (root_path)))                          \
      {                                                                        \
        int i;                                                                 \
        for (i = 0; i < taildirs; i++)                                         \
          {                                                                    \
            char *name = _getname (root_path);                                 \
            if (name > root_path)                                              \
              {                                                                \
                if (i < taildirs - 1)                                          \
                  name--;                                                      \
                if (*name != ':')                                              \
                  *name = '\0';                                                \
              }                                                                \
          }                                                                    \
        if (fGCC)                                                              \
          setenv ("GCC_ROOT", root_path, 1);                                   \
        if (fGPP)                                                              \
          setenv ("G++_ROOT", root_path, 1);                                   \
        if (fGCC)                                                              \
          setenv ("BINUTILS_ROOT", root_path, 1);                              \
      }                                                                        \
    }                                                                          \
  }

/* emxomf does not understand stabs+, so for frontends we have to
   switch to standard stabs if -Zomf is used. We also do many other
   argv preprocessing here. */
#define GCC_DRIVER_HOST_INITIALIZATION                                         \
  {                                                                            \
    int i, j, new_argc, max_argc;                                              \
    const char **new_argv;                                                     \
    _emxload_env ("GCCLOAD");                                                  \
    _envargs (&argc, (char ***)&argv, "GCCOPT");                               \
    _response (&argc, (char ***)&argv);                                        \
    _wildcard (&argc, (char ***)&argv);                                        \
                                                                               \
    /* Copy argv into a new location and modify it while copying */            \
    new_argv = (const char **)malloc ((max_argc = argc) * sizeof (char *));    \
    new_argv [0] = argv [0];                                                   \
    for (i = 1, new_argc = 1; i < argc; i++)                                   \
      {                                                                        \
        int         arg_count = 1;                                             \
        const char *arg [4];                                                   \
        arg [0] = argv [i];                                                    \
        if (!strcmp (argv [i], "-Zcrtdll"))                                    \
          arg [0] = "-Zcrtdll=c_dll";                                          \
        else if (!strcmp (argv [i], "-Zlinker"))                               \
          {                                                                    \
            if (i + 1 >= argc)                                                 \
                fatal ("argument to `-Zlinker' is missing");                   \
            arg [0] = "-Xlinker";                                              \
            arg [1] = "-O";                                                    \
            arg [2] = "-Xlinker";                                              \
            arg [3] = argv [++i];                                              \
            arg_count = 4;                                                     \
          }                                                                    \
        if (new_argc + arg_count > max_argc)                                   \
            new_argv = (const char **)realloc (new_argv,                       \
                (new_argc + arg_count) * sizeof (char *));                     \
        for (j = 0; j < arg_count; j++)                                        \
            new_argv [new_argc++] = arg [j];                                   \
      }                                                                        \
    argv = new_argv; argc = new_argc;                                          \
    EMX_INITIALIZE_ENVIRONMENT(2)                                              \
  }

/* Do some OS/2-specific work upon initialization of all compilers */
#define COMPILER_HOST_INITIALIZATION                                           \
  {                                                                            \
    /* Preload compiler if specified by GCCLOAD for faster subsequent runs */  \
    _emxload_env ("GCCLOAD");                                                  \
    /* Compilers don't fork (thanks God!) so we can use >32MB RAM */           \
    /* bird: this doesn't matter really as the compiler now should use high */ \
    /*       memory. But we'll leave it here in case a DLL gets broken. */     \
    _uflags (_UF_SBRK_MODEL, _UF_SBRK_ARBITRARY);                              \
    EMX_INITIALIZE_ENVIRONMENT(5)                                              \
  }
#endif



/******************************************************************************
 *                             - C - L - H  -
 ******************************************************************************/

/* Command-line help facility support -
   avoid using termcap since its not too reliable on OS/2 */
#ifdef IN_CLH

#include <io.h>

static int emx_monochrome = 0;  /* 1 for monochrome output */
static int emx_textattr = 0;    /* current text attributes */

static int
emx_getwidth (void)
{
    int wh[2];
    char *term;

    term = getenv ("CLH_TERM");
    if (!term)
        term = getenv ("TERM");
    if (term && stricmp (term, "mono") == 0)
        emx_monochrome = 1;

    _scrsize (wh);
    return (wh[0]);
}

static const char *ansi_seq [] =
{
/*  color               monochrome */
    "\033[0;36m",       "\033[0m",      /* */
    "\033[1;36m",       "\033[1m",      /* header */
    "\033[1;37m",       "\033[7m",      /* option */
    "\033[1;37m",       "\033[7m",      /* option + header */
    "\033[1;37m",       "\033[1m",      /* bold */
    "\033[1;37m",       "\033[1m",      /* bold + header */
    "\033[1;37m",       "\033[7m",      /* bold + option */
    "\033[1;37m",       "\033[7m",      /* bold + option + header */
    "\033[1;32m",       "\033[1m",      /* italic */
    "\033[1;32m",       "\033[1m",      /* italic + header */
    "\033[1;32m",       "\033[1m",      /* italic + option */
    "\033[1;32m",       "\033[1m",      /* italic + option + header */
    "\033[1;32m",       "\033[1m",      /* italic + bold */
    "\033[1;32m",       "\033[1m",      /* italic + bold + header */
    "\033[1;32m",       "\033[1m",      /* italic + bold + option */
    "\033[1;32m",       "\033[1m",      /* italic + bold + option + header */
    "\033[1;33m",       "\033[4m",      /* underline */
    "\033[1;33m",       "\033[4m",      /* underline + header */
    "\033[1;37m",       "\033[4m",      /* underline + option */
    "\033[1;37m",       "\033[4m",      /* underline + option + header */
    "\033[1;37m",       "\033[4m",      /* underline + bold */
    "\033[1;37m",       "\033[4m",      /* underline + bold + header */
    "\033[1;37m",       "\033[4m",      /* underline + bold + option */
    "\033[1;37m",       "\033[4m",      /* underline + bold + option + header */
    "\033[1;33m",       "\033[4m",      /* underline + italic */
    "\033[1;37m",       "\033[4m",      /* underline + italic + header */
    "\033[1;37m",       "\033[4m",      /* underline + italic + option */
    "\033[1;37m",       "\033[4m",      /* underline + italic + option + header */
    "\033[1;37m",       "\033[4m",      /* underline + italic + bold */
    "\033[1;37m",       "\033[4m",      /* underline + italic + bold + header */
    "\033[1;37m",       "\033[4m",      /* underline + italic + bold + option */
    "\033[1;37m",       "\033[4m",      /* underline + italic + bold + option + header */
};

#define TERM_OUT(s) \
    write (1, s, strlen (s));

static int
emx_setattr (int flags, int flagmask)
{
    emx_textattr = (emx_textattr & ~flagmask) | flags;
    TERM_OUT (ansi_seq [emx_textattr * 2 + emx_monochrome]);
    return (0);
}

#define CLH_GET_WIDTH                   emx_getwidth ()
#define CLH_GET_FILE_WIDTH              emx_getwidth ()
#define CLH_SET_ATTR(flags, flagmask)   emx_setattr (flags, flagmask)
#define CLH_FINISH                      TERM_OUT (ansi_seq [emx_monochrome])

#endif /* IN_CLH */
