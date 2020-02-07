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

/* emxomf does not understand stabs+, so for frontends we have to
   switch to standard stabs if -Zomf is used. We also do many other
   argv preprocessing here. */

void
emx_driver_init (unsigned int *decoded_options_count,
		    struct cl_decoded_option **decoded_options)
{
    unsigned int i;

    /* Preload compiler if specified by GCCLOAD for faster subsequent runs */
    _emxload_env ("GCCLOAD");
    /* Compilers don't fork (thanks God!) so we can use >32MB RAM */ 
    /* bird: this doesn't matter really as the compiler now should use high */
    /*       memory. But we'll leave it here in case a DLL gets broken. */
    _uflags (_UF_SBRK_MODEL, _UF_SBRK_ARBITRARY);

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
