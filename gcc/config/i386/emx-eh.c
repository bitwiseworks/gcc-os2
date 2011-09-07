/* This piece of code is linked into the program from gccXYZ.a or libgcc_eh.a
   if any of the object files in the link are using exception (i.e. have exception
   frame tables inside). The code automatically registers all exception
   tables into a central list (meant to be placed inside gcc*.dll),
   upon program exit these lists are deregistered. */

extern int __eh_frame__;
extern void __register_frame_table (void *begin);
extern void __deregister_frame (void *begin);

/** Exception handler stuff init indicator. 0 means not inited, 1 means inited. */
static int inited;

/**
 * Inits exception handler stuff.
 * Called by __ctordtorInit() thru the __eh_init__ vector.
 */
void __ehInit (void)
{
  if (!inited)
    {
      int *ptr = &__eh_frame__;
      inited = 1;
      __register_frame_table (&ptr [*ptr == -2 ? 1 : 2]);
    }
}

/**
 * Terminates exception handler stuff.
 * Called by __ctordtorTerm() thru the __eh_term__ vector.
 */
void __ehTerm (void)
{
  if (inited)
    {
      int *ptr = &__eh_frame__;
      inited = 0;
      __deregister_frame (&ptr [*ptr == -2 ? 1 : 2]);
    }
}

/* emx-ctordtor.c will process these. */
__asm__ (".stabs \"___eh_init__\", 23, 0, 0, ___ehInit");
__asm__ (".stabs \"___eh_term__\", 23, 0, 0, ___ehTerm");
