/* Utilities to execute a program in a subprocess (possibly linked by pipes
   with other subprocesses), and wait for it.  DJGPP specialization.
   Copyright (C) 1996-2019 Free Software Foundation, Inc.

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If not,
write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

#include "pex-common.h"

#include <stdio.h>
#include <errno.h>
#ifdef NEED_DECLARATION_ERRNO
extern int errno;
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <process.h>

/* Use ECHILD if available, otherwise use EINVAL.  */
#ifdef ECHILD
#define PWAIT_ERROR ECHILD
#else
#define PWAIT_ERROR EINVAL
#endif

static int pex_djgpp_open_read (struct pex_obj *, const char *, int);
static int pex_djgpp_open_write (struct pex_obj *, const char *, int, int);
static pid_t pex_djgpp_exec_child (struct pex_obj *, int, const char *,
				  char * const *, char * const *,
				  int, int, int, int,
				  const char **, int *);
static int pex_djgpp_close (struct pex_obj *, int);
static pid_t pex_djgpp_wait (struct pex_obj *, pid_t, int *, struct pex_time *,
			   int, const char **, int *);
#ifdef __EMX__
static int pex_djgpp_pipe (struct pex_obj *, int *, int);
static FILE *pex_djgpp_fdopenr (struct pex_obj *, int, int);
static FILE *pex_djgpp_fdopenw (struct pex_obj *, int, int);
static void pex_djgpp_cleanup (struct pex_obj *obj);
#endif

/* The list of functions we pass to the common routines.  */

const struct pex_funcs funcs =
{
  pex_djgpp_open_read,
  pex_djgpp_open_write,
  pex_djgpp_exec_child,
  pex_djgpp_close,
  pex_djgpp_wait,
#ifndef __EMX__
  NULL, /* pipe */
  NULL, /* fdopenr */
  NULL, /* fdopenw */
  NULL  /* cleanup */
#else
  pex_djgpp_pipe,
  pex_djgpp_fdopenr,
  pex_djgpp_fdopenw,
  pex_djgpp_cleanup
#endif
};

/* Return a newly initialized pex_obj structure.  */

struct pex_obj *
pex_init (int flags, const char *pname, const char *tempbase)
{
#ifndef __EMX__
  /* DJGPP does not support pipes.  */
  flags &= ~ PEX_USE_PIPES;
#endif
  return pex_init_common (flags, pname, tempbase, &funcs);
}

/* Open a file for reading.  */

static int
pex_djgpp_open_read (struct pex_obj *obj ATTRIBUTE_UNUSED,
		     const char *name, int binary)
{
  return open (name, O_RDONLY | (binary ? O_BINARY : O_TEXT));
}

/* Open a file for writing.  */

static int
pex_djgpp_open_write (struct pex_obj *obj ATTRIBUTE_UNUSED,
		      const char *name, int binary, int append)
{
  /* Note that we can't use O_EXCL here because gcc may have already
     created the temporary file via make_temp_file.  */
  if (append)
    return -1;
  return open (name,
	       (O_WRONLY | O_CREAT | O_TRUNC
		| (binary ? O_BINARY : O_TEXT)),
	       S_IRUSR | S_IWUSR);
}

/* Close a file.  */

static int
pex_djgpp_close (struct pex_obj *obj ATTRIBUTE_UNUSED, int fd)
{
  return close (fd);
}

/* Execute a child.  */

static pid_t
pex_djgpp_exec_child (struct pex_obj *obj, int flags, const char *executable,
		      char * const * argv, char * const * env,
                      int in, int out, int errdes,
		      int toclose ATTRIBUTE_UNUSED, const char **errmsg,
		      int *err)
{
  int org_in, org_out, org_errdes;
  int status;
  int *statuses;

  org_in = -1;
  org_out = -1;
  org_errdes = -1;

  if (in != STDIN_FILE_NO)
    {
      org_in = dup (STDIN_FILE_NO);
      if (org_in < 0)
	{
	  *err = errno;
	  *errmsg = "dup";
	  return (pid_t) -1;
	}
      if (dup2 (in, STDIN_FILE_NO) < 0)
	{
	  *err = errno;
	  *errmsg = "dup2";
	  return (pid_t) -1;
	}
      if (close (in) < 0)
	{
	  *err = errno;
	  *errmsg = "close";
	  return (pid_t) -1;
	}
    }

  if (out != STDOUT_FILE_NO)
    {
      org_out = dup (STDOUT_FILE_NO);
      if (org_out < 0)
	{
	  *err = errno;
	  *errmsg = "dup";
	  return (pid_t) -1;
	}
      if (dup2 (out, STDOUT_FILE_NO) < 0)
	{
	  *err = errno;
	  *errmsg = "dup2";
	  return (pid_t) -1;
	}
      if (close (out) < 0)
	{
	  *err = errno;
	  *errmsg = "close";
	  return (pid_t) -1;
	}
    }

  if (errdes != STDERR_FILE_NO
      || (flags & PEX_STDERR_TO_STDOUT) != 0)
    {
      org_errdes = dup (STDERR_FILE_NO);
      if (org_errdes < 0)
	{
	  *err = errno;
	  *errmsg = "dup";
	  return (pid_t) -1;
	}
      if (dup2 ((flags & PEX_STDERR_TO_STDOUT) != 0 ? STDOUT_FILE_NO : errdes,
		 STDERR_FILE_NO) < 0)
	{
	  *err = errno;
	  *errmsg = "dup2";
	  return (pid_t) -1;
	}
      if (errdes != STDERR_FILE_NO)
	{
	  if (close (errdes) < 0)
	    {
	      *err = errno;
	      *errmsg = "close";
	      return (pid_t) -1;
	    }
	}
    }

#ifdef __EMX__
  /* command line cannot exceed 32KB, DosExecPgm limit */
  {
    char* argv_rsp[3];
    char** _argv = argv;
    char* arg = _argv[0];
    char rsp_arg[_MAX_PATH];
    int i = 0;
    int arglen = 0;

    while( (arg = _argv[i]) != NULL)
      {
        arglen += strlen( arg) + 1;
        i++;
      }

    /* safe len check */
    if (arglen > (30*1024))
      {
        /* create temporary file */
        char* rsp = tempnam( NULL, "grsp");
        FILE* out = fopen( rsp, "w");
        if (out == NULL)
          {
            *err = errno;
            *errmsg = "tempnam";
            return (pid_t) -1;
          }
        /* dump arguments, skip program name */
        i = 1;
        while( (arg = _argv[i]) != NULL)
          {
            fprintf( out, "%s\n", arg);
            i++;
          }
        fclose( out);
        /* new command line args */
        argv_rsp[0] = _argv[0];
        sprintf( rsp_arg, "@%s", rsp);
        argv_rsp[1] = rsp_arg;
        argv_rsp[2] = NULL;
        /* add it for automatic cleanup */
        pex_add_remove( obj, rsp, 1);
        /* use new argv */
        _argv = argv_rsp;
      }

    /* this returns pid instead of status! */
    if (env)
      status = (((flags & PEX_SEARCH) != 0 ? spawnvpe : spawnve)
                (P_NOWAIT, executable, _argv, env));
    else
      status = (((flags & PEX_SEARCH) != 0 ? spawnvp : spawnv)
                (P_NOWAIT, executable, _argv));
  }
#else
  if (env)
    status = (((flags & PEX_SEARCH) != 0 ? spawnvpe : spawnve)
	      (P_WAIT, executable, argv, env));
  else
    status = (((flags & PEX_SEARCH) != 0 ? spawnvp : spawnv)
  	      (P_WAIT, executable, argv));
#endif

  if (status == -1)
    {
      *err = errno;
      *errmsg = ((flags & PEX_SEARCH) != 0) ? "spawnvp" : "spawnv";
    }

  if (in != STDIN_FILE_NO)
    {
      if (dup2 (org_in, STDIN_FILE_NO) < 0)
	{
	  *err = errno;
	  *errmsg = "dup2";
	  return (pid_t) -1;
	}
      if (close (org_in) < 0)
	{
	  *err = errno;
	  *errmsg = "close";
	  return (pid_t) -1;
	}
    }

  if (out != STDOUT_FILE_NO)
    {
      if (dup2 (org_out, STDOUT_FILE_NO) < 0)
	{
	  *err = errno;
	  *errmsg = "dup2";
	  return (pid_t) -1;
	}
      if (close (org_out) < 0)
	{
	  *err = errno;
	  *errmsg = "close";
	  return (pid_t) -1;
	}
    }

  if (errdes != STDERR_FILE_NO
      || (flags & PEX_STDERR_TO_STDOUT) != 0)
    {
      if (dup2 (org_errdes, STDERR_FILE_NO) < 0)
	{
	  *err = errno;
	  *errmsg = "dup2";
	  return (pid_t) -1;
	}
      if (close (org_errdes) < 0)
	{
	  *err = errno;
	  *errmsg = "close";
	  return (pid_t) -1;
	}
    }

#ifdef __EMX__
  if (status == -1)
    {
	  return (pid_t) -1;
    }
#endif

  /* Save the exit status for later.  When we are called, obj->count
     is the number of children which have executed before this
     one.  */
  statuses = (int *) obj->sysdep;
  statuses = XRESIZEVEC (int, statuses, obj->count + 1);
  statuses[obj->count] = status;
  obj->sysdep = (void *) statuses;

  return (pid_t) obj->count;
}

#ifdef __EMX__

#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/wait.h>

static pid_t
pex_wait (struct pex_obj *obj ATTRIBUTE_UNUSED, pid_t pid, int *status,
	  struct pex_time *time)
{
  pid_t ret;
  struct rusage r;

  ret = wait4 (pid, status, 0, &r);

  if (time != NULL)
    {
      time->user_seconds = r.ru_utime.tv_sec;
      time->user_microseconds= r.ru_utime.tv_usec;
      time->system_seconds = r.ru_stime.tv_sec;
      time->system_microseconds= r.ru_stime.tv_usec;
    }

  /* Suppress 'Internal error: Interrupt' caused by Ctrl-C and Ctrl-Break */
  if (status && WIFSIGNALED (*status))
    {
      switch (WTERMSIG (*status))
        {
        case SIGINT:      /* Ctrl-C */
        case SIGBREAK:    /* Ctrl-Break */
          /* Clear a signaled status and use SIGINT as a return code */
          *status = SIGINT << 8;
          break;
        }
    }

  return ret;
}

#endif /* __EMX__ */

/* Wait for a child process to complete.  Actually the child process
   has already completed, and we just need to return the exit
   status.  */

static pid_t
pex_djgpp_wait (struct pex_obj *obj, pid_t pid, int *status,
		struct pex_time *time, int done ATTRIBUTE_UNUSED,
		const char **errmsg ATTRIBUTE_UNUSED,
		int *err ATTRIBUTE_UNUSED)
{
  int *statuses;
#ifdef __EMX__
  pid_t _pid;
#endif

  if (time != NULL)
    memset (time, 0, sizeof *time);

  statuses = (int *) obj->sysdep;
#ifdef __EMX__
  _pid = statuses[pid];
  /* If we are cleaning up when the caller didn't retrieve process
     status for some reason, encourage the process to go away.  */
  if (done)
    kill (_pid, SIGTERM);

  if (pex_wait (obj, _pid, status, time) < 0)
    {
      *err = errno;
      *errmsg = "wait";
      return -1;
    }
#else
  *status = statuses[pid];
#endif

  return 0;
}

#ifdef __EMX__
#include <io.h>

/* Create a pipe.  */

static int
pex_djgpp_pipe (struct pex_obj *obj ATTRIBUTE_UNUSED, int *p, int binary)
{
  if (pipe (p))
    return -1;

  setmode (p[0], binary ? O_BINARY : O_TEXT);
  setmode (p[1], binary ? O_BINARY : O_TEXT);

  return 0;
}

/* Get a FILE pointer to read from a file descriptor.  */

static FILE *
pex_djgpp_fdopenr (struct pex_obj *obj ATTRIBUTE_UNUSED, int fd, int binary)
{
  return fdopen (fd, binary ? "rb" : "rt");
}

static FILE *
pex_djgpp_fdopenw (struct pex_obj *obj ATTRIBUTE_UNUSED, int fd, int binary)
{
  if (fcntl (fd, F_SETFD, FD_CLOEXEC) < 0)
    return NULL;

  return fdopen (fd, binary ? "wb" : "wt");
}

static void
pex_djgpp_cleanup (struct pex_obj *obj ATTRIBUTE_UNUSED)
{
  if (obj->sysdep != NULL)
    {
      free (obj->sysdep);
    }
}
#endif
