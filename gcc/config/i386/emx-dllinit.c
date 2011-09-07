/* This is the startup code for GCC version-specific runtime dll.
   It should not be exposed to anyone else as we don't wanna have
   duplicate startup code around. The right startup code is in libc. */

extern int _CRT_init (void);
extern void __ctordtorInit (void);
extern void __ctordtorTerm (void);

unsigned long _System _DLL_InitTerm (unsigned long mod_handle, unsigned long flag)
{
  switch (flag)
  {
    case 0:
      if (_CRT_init () != 0)
        break;
      __ctordtorInit ();
      return 1;
    case 1:
      __ctordtorTerm ();
      return 1;
  }
  return 0;
}
