/* Threads compatibily routines for libgcc2.  */
/* Compile this one with gcc.  */
/* Copyright (C) 1997 Free Software Foundation, Inc.

This file is part of GNU CC.

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

/* As a special exception, if you link this library with other files,
   some of which are compiled with GCC, to produce an executable,
   this library does not by itself cause the resulting executable
   to be covered by the GNU General Public License.
   This exception does not however invalidate any other reasons why
   the executable file might be covered by the GNU General Public License.  */

#ifndef __gthr_os2_h
#define __gthr_os2_h

#ifdef _LIBOBJC
#error "gthr-os2.h doesn't implement the _LIBOBJC mode!"
#endif

#define __GTHREADS 1

/* OS/2 threads specific definitions */

#include <sys/builtin.h>
#include <sys/fmutex.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <InnoTekLIBC/thread.h>
#include <InnoTekLIBC/backend.h>

typedef int __gthread_key_t;
typedef struct
{
  signed char volatile done;
  signed char volatile started;
} __gthread_once_t;
typedef _fmutex __gthread_mutex_t;
typedef struct {
  long depth;
  uint32_t owner;
  _fmutex actual;
} __gthread_recursive_mutex_t;

#define __GTHREAD_ONCE_INIT		{ 0, 0 }
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION __gthread_recursive_mutex_init_function
#define __GTHREAD_MUTEX_INIT_FUNCTION	__gthread_mutex_init_function

static inline int
__gthread_active_p (void)
{
  return 1;
}

static inline int
__gthread_once (__gthread_once_t *once, void (*func) (void))
{
  if (once == NULL || func == NULL)
      return EINVAL;
  if (__cxchg (&once->started, 1) == 0)
    {
      func ();
      __cxchg (&once->done, 1);
    }
  else if (!once->done)
    {
      unsigned outer = 0;
      do
        {
          int i = 4096;
          while (i-- > 0 && !once->done)
            __libc_Back_threadSleep(0, NULL);
          if (once->done)
            break;
          __libc_Back_threadSleep(++outer % 32, NULL);
        }
      while (!once->done);
    }
  return 0;
}

static inline void
__gthread_mutex_init_function (__gthread_mutex_t *mutex)
{
  _fmutex_create (mutex, 0);
}

static inline int
__gthread_mutex_destroy (__gthread_mutex_t *mutex )
{
  return 0;
}

static inline int
__gthread_mutex_lock (__gthread_mutex_t *mutex)
{
  return _fmutex_request (mutex, _FMR_IGNINT);
}

static inline int
__gthread_mutex_trylock (__gthread_mutex_t *mutex)
{
  return _fmutex_request (mutex, _FMR_IGNINT | _FMR_NOWAIT);
}

static inline int
__gthread_mutex_unlock (__gthread_mutex_t *mutex)
{
  return _fmutex_release (mutex);
}

static inline int
__gthread_recursive_mutex_init_function (__gthread_recursive_mutex_t *mutex)
{
  mutex->depth = 0;
  mutex->owner = fibGetTidPid();
  return _fmutex_create (&mutex->actual, 0);
}

static inline int
__gthread_recursive_mutex_lock (__gthread_recursive_mutex_t *mutex)
{
  if (__gthread_active_p ())
    {
      uint32_t me = fibGetTidPid();

      if (mutex->owner != me)
	{
	  __gthread_mutex_lock(&mutex->actual);
	  mutex->owner = me;
	}

      mutex->depth++;
    }
  return 0;
}

static inline int
__gthread_recursive_mutex_trylock (__gthread_recursive_mutex_t *mutex)
{
  if (__gthread_active_p ())
    {
      uint32_t me = fibGetTidPid();

      if (mutex->owner != me)
	{
	  if (__gthread_mutex_trylock(&mutex->actual))
	    return 1;
	  mutex->owner = me;
	}

      mutex->depth++;
    }
  return 0;
}

static inline int
__gthread_recursive_mutex_unlock (__gthread_recursive_mutex_t *mutex)
{
  if (__gthread_active_p ())
    {
      if (--mutex->depth == 0)
	{
	   mutex->owner = fibGetTidPid();
	   __gthread_mutex_unlock(&mutex->actual);
	}
    }
  return 0;
}

static inline int
__gthread_recursive_mutex_destroy (__gthread_recursive_mutex_t *__mutex)
{
  return 0;
}

static inline int
__gthread_key_create (__gthread_key_t *key, void (*dtor) (void *))
{
  int iTLS = __libc_TLSAlloc ();
  if (iTLS < 0)
    return errno;
  __libc_TLSDestructor (iTLS, (void (*)(void *, int, unsigned))dtor, 0);
  *key = iTLS;
  return 0;
}

static inline int
__gthread_key_dtor (__gthread_key_t key, void *ptr)
{
  if (__libc_TLSSet (key, NULL))
    return errno;
  return 0;
}

static inline int
__gthread_key_delete (__gthread_key_t key)
{
  if (__libc_TLSFree (key))
    return errno;
  return 0;
}

static inline void *
__gthread_getspecific (__gthread_key_t key)
{
  return __libc_TLSGet (key);
}

static inline int
__gthread_setspecific (__gthread_key_t key, const void *ptr)
{
  if (__libc_TLSSet (key, (void *)ptr))
    return errno;
  return 0;
}

#endif /* not __gthr_os2_h */
