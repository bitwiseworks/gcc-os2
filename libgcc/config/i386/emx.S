/* $Id: emx-libgcc1.asm 2115 2005-06-30 02:11:54Z bird $ */
/** @file
 *
 * GCC alloca helper
 *
 * Copyright (c) 2005 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of InnoTek LIBC.
 *
 * InnoTek LIBC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * InnoTek LIBC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with InnoTek LIBC; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

        .text

#ifdef L_alloca
        .globl __alloca
// IN:   EAX = stack space to allocate (rounded to 4 boundary by GCC)
// OUT:  ESP adjusted down by EAX, stack probed
// NOTE: Never call this from C!
// CHG:  EAX. ESP
__alloca:
        /*
         * Calc new %esp, store it in %eax.
         */
        negl    %eax
        lea     4(%esp, %eax), %eax

        /*
         * Setup the prober, %ecx, to point to the top of the stack page below.
         */
        pushl   %ecx
        movl    %esp, %ecx
        andl    $0xfffff000, %ecx
        subl    $8, %ecx
        /* Do we need to probe anything? */
        cmpl    %eax, %ecx
        jl      L2

        /*
         * The probe loop
         */
        .align 2, 0x90
L1:
        orl     $0, (%ecx)              /* probe */
        subl    $0x1000, %ecx           /* next page */
        cmpl    %eax, %ecx              /* done? */
        jnl     L1                      /* jump if done */

        /*
         * Done probing, commit the allocation and jump to the return address.
         */
L2:     popl    %ecx
        xchg    %esp, %eax              /* commit */
        movl    (%eax), %eax            /* load return address */
#if defined (__EPILOGUE__)
___POST$_alloca:
#endif
        jmp     *%eax                   /* return */
#endif // L_alloca
