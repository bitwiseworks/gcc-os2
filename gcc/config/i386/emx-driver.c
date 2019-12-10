/* Additional functions for the GCC driver on emx/os2
   Copyright (C) 2006, 2007, 2008, 2010 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "opt-suggestions.h"
#include "gcc.h"
#include "opts.h"

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

void
emx_driver_init (unsigned int *decoded_options_count,
		    struct cl_decoded_option **decoded_options)
{
    int new_argc, max_argc;
    const char **new_argv;
    _emxload_env ("GCCLOAD");
#if 0
    _envargs (&argc, (char ***)&argv, "GCCOPT");
    _response (&argc, (char ***)&argv);
    _wildcard (&argc, (char ***)&argv);
#endif
    unsigned int i;

  for (i = 1; i < *decoded_options_count; i++)
    {
      if ((*decoded_options)[i].errors & CL_ERR_MISSING_ARG)
	continue;
      switch ((*decoded_options)[i].opt_index)
	{
	case OPT_Zlinker:
	  ++*decoded_options_count;
	  *decoded_options = XRESIZEVEC (struct cl_decoded_option,
					 *decoded_options,
					 *decoded_options_count);
	  memmove (*decoded_options + i + 2,
		   *decoded_options + i + 1,
		   ((*decoded_options_count - i - 2)
		    * sizeof (struct cl_decoded_option)));
	  generate_option (OPT_Xlinker,
			   (*decoded_options)[i].canonical_option[1], 1,
			   CL_DRIVER, &(*decoded_options)[i+1]);
	  generate_option (OPT_Xlinker,
			   "-O", 1,
			   CL_DRIVER, &(*decoded_options)[i]);
	  break;

	default:
	  break;
	}
    }
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
