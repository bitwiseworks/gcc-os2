/* Threads compatibily routines for libgcc2.  */
/* Compile this one with gcc.  */
/* Copyright (C) 1997-2016 Free Software Foundation, Inc.
   Contributed by KO Myung-Hun <komh@chollian.net>.

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

#ifdef __cplusplus
extern "C" {
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

typedef struct {
  int recursive;
  unsigned long mtx;
  _fmutex static_lock;
} __gthread_mutex_t, __gthread_recursive_mutex_t;

#define __GTHREAD_ONCE_INIT		{ 0, 0 }
#define __GTHREAD_MUTEX_INIT    { 0, 0, _FMUTEX_INITIALIZER }
#define __GTHREAD_MUTEX_INIT_FUNCTION	__gthread_mutex_init_function
#define __GTHREAD_RECURSIVE_MUTEX_INIT { 1, 0, _FMUTEX_INITIALIZER }
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION __gthread_recursive_mutex_init_function

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

extern void __gthread_os2_mutex_init (__gthread_mutex_t *, int);
extern int __gthread_os2_mutex_destroy (__gthread_mutex_t *, int);
extern int __gthread_os2_mutex_lock (__gthread_mutex_t *, int);
extern int __gthread_os2_mutex_trylock (__gthread_mutex_t *, int);
extern int __gthread_os2_mutex_unlock (__gthread_mutex_t *, int);

static inline void
__gthread_mutex_init_function (__gthread_mutex_t *mutex)
{
  __gthread_os2_mutex_init (mutex, 0);
}

static inline int
__gthread_mutex_destroy (__gthread_mutex_t *mutex )
{
  return __gthread_os2_mutex_destroy (mutex, 0);
}

static inline int
__gthread_mutex_lock (__gthread_mutex_t *mutex)
{
  return __gthread_os2_mutex_lock (mutex, 0);
}

static inline int
__gthread_mutex_trylock (__gthread_mutex_t *mutex)
{
  return __gthread_os2_mutex_trylock (mutex, 0);
}

static inline int
__gthread_mutex_unlock (__gthread_mutex_t *mutex)
{
  return __gthread_os2_mutex_unlock (mutex, 0);
}

static inline void
__gthread_recursive_mutex_init_function (__gthread_recursive_mutex_t *mutex)
{
  __gthread_os2_mutex_init (mutex, 1);
}

static inline int
__gthread_recursive_mutex_destroy (__gthread_recursive_mutex_t *mutex)
{
  return __gthread_os2_mutex_destroy (mutex, 1);
}

static inline int
__gthread_recursive_mutex_lock (__gthread_recursive_mutex_t *mutex)
{
  return __gthread_os2_mutex_lock (mutex, 1);
}

static inline int
__gthread_recursive_mutex_trylock (__gthread_recursive_mutex_t *mutex)
{
  return __gthread_os2_mutex_trylock (mutex, 1);
}

static inline int
__gthread_recursive_mutex_unlock (__gthread_recursive_mutex_t *mutex)
{
  return __gthread_os2_mutex_unlock (mutex, 1);
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

#define __GTHREAD_HAS_COND 1

typedef struct {
  unsigned long ev;
  unsigned long ack;
  unsigned volatile signaled;
  unsigned volatile waiters;
  _fmutex static_lock;
} __gthread_cond_t;

#define __GTHREAD_COND_INIT { 0, 0, 0, 0, _FMUTEX_INITIALIZER }
#define __GTHREAD_COND_INIT_FUNCTION __gthread_cond_init_function

extern void __gthread_os2_cond_init (__gthread_cond_t *);
extern int __gthread_os2_cond_destroy (__gthread_cond_t *);
extern int __gthread_os2_cond_broadcast (__gthread_cond_t *);
extern int __gthread_os2_cond_wait (__gthread_cond_t *, __gthread_mutex_t *);
extern int __gthread_os2_cond_wait_recursive (__gthread_cond_t *,
                                              __gthread_recursive_mutex_t *);

static inline void
__gthread_cond_init_function (__gthread_cond_t *cond)
{
  __gthread_os2_cond_init (cond);
}

static inline int
__gthread_cond_destroy (__gthread_cond_t *cond)
{
  return __gthread_os2_cond_destroy (cond);
}

static inline int
__gthread_cond_broadcast (__gthread_cond_t *cond)
{
  return __gthread_os2_cond_broadcast (cond);
}

static inline int
__gthread_cond_wait (__gthread_cond_t *cond, __gthread_mutex_t *mutex)
{
  return __gthread_os2_cond_wait (cond, mutex);
}

static inline int
__gthread_cond_wait_recursive (__gthread_cond_t *cond,
                               __gthread_recursive_mutex_t *mutex)
{
  return __gthread_os2_cond_wait_recursive (cond, mutex);
}

#define __GTHREADS_CXX0X 1

#include <time.h>

typedef int __gthread_t;

typedef struct timespec __gthread_time_t;

extern int __gthread_os2_create (__gthread_t *, void *(*) (void*), void *);
extern int __gthread_os2_join (__gthread_t, void **);
extern int __gthread_os2_detach (__gthread_t);
extern int __gthread_os2_equal (__gthread_t, __gthread_t);
extern int __gthread_os2_self (void);
extern int __gthread_os2_yield (void);
extern int __gthread_os2_mutex_timedlock (__gthread_mutex_t *,
                                          const __gthread_time_t *);
extern int
  __gthread_os2_recursive_mutex_timedlock (__gthread_recursive_mutex_t *,
                                           const __gthread_time_t *);
extern int __gthread_os2_cond_signal (__gthread_cond_t *);
extern int __gthread_os2_cond_timedwait (__gthread_cond_t *,
                                         __gthread_mutex_t *,
                                         const __gthread_time_t *);

static inline int
__gthread_create (__gthread_t *thread, void *(*func) (void*), void *args)
{
  return __gthread_os2_create (thread, func, args);
}

static inline int
__gthread_join (__gthread_t thread, void **value_ptr)
{
  return __gthread_os2_join (thread, value_ptr);
}

static inline int
__gthread_detach (__gthread_t thread)
{
  return __gthread_os2_detach (thread);
}

/* Compare threads. Return non-zero if equal, otherwise 0.  */
static inline int
__gthread_equal (__gthread_t t1, __gthread_t t2)
{
  return __gthread_os2_equal (t1, t2);
}

static inline __gthread_t
__gthread_self (void)
{
  return __gthread_os2_self ();
}

static inline int
__gthread_yield (void)
{
  return __gthread_os2_yield ();
}

static inline int
__gthread_mutex_timedlock (__gthread_mutex_t *mutex,
                           const __gthread_time_t *abs_timeout)
{
  return __gthread_os2_mutex_timedlock (mutex, abs_timeout);
}

static inline int
__gthread_recursive_mutex_timedlock (__gthread_recursive_mutex_t *mutex,
                                     const __gthread_time_t *abs_timeout)
{
  return __gthread_os2_recursive_mutex_timedlock (mutex, abs_timeout);
}

static inline int
__gthread_cond_signal (__gthread_cond_t *cond)
{
  return __gthread_os2_cond_signal (cond);
}

static inline int
__gthread_cond_timedwait (__gthread_cond_t *cond, __gthread_mutex_t *mutex,
                          const __gthread_time_t *abs_timeout)
{
  return __gthread_os2_cond_timedwait (cond, mutex, abs_timeout);
}

#ifdef __cplusplus
}
#endif

#endif /* not __gthr_os2_h */
