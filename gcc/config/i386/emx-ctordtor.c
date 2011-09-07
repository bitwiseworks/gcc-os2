/* At startup call all constructor and at shutdown all destructor functions.
   Optionally initializes frame unwind info (if emx-eh is linked in). */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <InnoTekLIBC/fork.h>
#include <emx/startup.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/** Init indicator. 0 means not inited, 1 means inited. */
static int inited;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static int ctordtorForkChild(__LIBC_PFORKHANDLE pForkHandle, __LIBC_FORKOP enmOperation);
static void ctordtorForkComplete(void *pvArg, int rc, __LIBC_FORKCTX enmCtx);


/**
 * Create static C++ objects.
 */
void __ctordtorInit(void)
{
    if (!inited)
    {
        inited = 1;
        __ctordtorInit1(&__eh_init__);
        __ctordtorInit1(&__CTOR_LIST__);
    }
}


/**
 * Destroy static C++ objects.
 */
void __ctordtorTerm(void)
{
    if (inited)
    {
        inited = 0;
        __ctordtorTerm1(&__DTOR_LIST__);
        __ctordtorTerm1(&__eh_term__);
    }
}


//_FORK_CHILD1(0xfffffffe, ctordtorForkChild)

/**
 * Fork callback which will prevent the dtors from running
 * unless the fork is successful.
 *
 * @returns 0 on success.
 * @returns -errno on failure.
 * @param   pForkHandle     Pointer to fork handle.
 * @param   enmOperation    Fork operation.
 */
int ctordtorForkChild(__LIBC_PFORKHANDLE pForkHandle, __LIBC_FORKOP enmOperation)
{
    int     rc = 0;
    switch (enmOperation)
    {
        /*
         * Register a completion callback.
         */
        case __LIBC_FORK_OP_FORK_CHILD:
            rc = pForkHandle->pfnCompletionCallback(pForkHandle, ctordtorForkComplete, (void *)inited, __LIBC_FORK_CTX_CHILD);
        case __LIBC_FORK_OP_EXEC_CHILD:
            inited = 0;
            break;

        default:
            break;

    }
    return 0;
}


/**
 * Fork completion (child) callback for restoring inited to the correct value.
 *
 * @param   pvArg   The original value of inited.
 * @param   rc      The fork result. This is 0 on success. On failure it is the
 *                  negative errno value.
 * @param   enmCtx  The context the completion callback function is called in.
 */
static void ctordtorForkComplete(void *pvArg, int rc, __LIBC_FORKCTX enmCtx)
{
    /*
     * Restore the inited flag.
     */
    inited = !rc && pvArg;

    /* unreferenced */
    enmCtx = enmCtx;
}
