/* emx.c: Functions for emx as target system.

Original version by Eberhard Mattes, based on i386.c.
Heavily modified by Andrew Zabolotny and Knut St. Osmundsen.
Modified for GCC 4.x by Paul Smedley 2008-2014

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "output.h"
#include "tree.h"
#include "flags.h"
#include "tm_p.h"
#include "diagnostic-core.h"
#include "hashtab.h"
#include "langhooks.h"
#include "ggc.h"
#include "target.h"
#include "except.h"
#include "stringpool.h"
#include "varasm.h"

/* The size of the target's pointer type.  */
#ifndef PTR_SIZE
#define PTR_SIZE (POINTER_SIZE / BITS_PER_UNIT)
#endif

/* DO NOT COMMIT WITH THIS DEFINED!!! */
/* #define BIRD_DEBUG */

#ifdef BIRD_DEBUG
static const char *code(tree node)
{
    if (node)
    {
        switch (TREE_CODE (node))
        {
            case IDENTIFIER_NODE:   return "IDENTIFIER_NODE";
            case TREE_LIST:         return "TREE_LIST      ";
            case TREE_VEC:          return "TREE_VEC       ";
            case BLOCK:             return "BLOCK          ";
            case FUNCTION_TYPE:     return "FUNCTION_TYPE  ";
            case FUNCTION_DECL:     return "FUNCTION_DECL  ";
            case METHOD_TYPE:       return "METHOD_TYPE    ";
            case FIELD_DECL:        return "FIELD_DECL     ";
            case TYPE_DECL:         return "TYPE_DECL      ";
            case VAR_DECL:          return "VAR_DECL       ";
            case PARM_DECL:         return "PARM_DECL      ";
            case RESULT_DECL:       return "RESULT_DECL    ";
            case CONST_DECL:        return "CONST_DECL     ";
            case POINTER_TYPE:      return "POINTER_TYPE   ";
            case VOID_TYPE:         return "VOID_TYPE      ";
            case INTEGER_TYPE:      return "INTEGER_TYPE   ";
            case BOOLEAN_TYPE:      return "BOOLEAN_TYPE   ";
            case ARRAY_TYPE:        return "ARRAY_TYPE     ";
            case RECORD_TYPE:       return "RECORD_TYPE    ";
            case QUAL_UNION_TYPE:   return "QUAL_UNION_TYPE";
            case UNION_TYPE:        return "UNION_TYPE     ";
            case INTEGER_CST:       return "INTEGER_CST    ";
            default:
                break;
        }
    }
    return "";
}

const char *birddump_callingconv(tree node);
const char *birddump_callingconv(node)
    tree node;
{
    static const char *apsz[] = {"system", "optlink", "stdcall", "cdecl", "dllimport", "dllexport"};
    static const char *psznone = "none";
    static const char *pszdefault = "default";
    tree        attr;
    unsigned    i;

    if (!node)
        return psznone;

    attr = DECL_P (node) ? DECL_ATTRIBUTES (node) : TYPE_P (node) ? TYPE_ATTRIBUTES (node) : NULL_TREE;
    if (!attr)
        return pszdefault;

    for (i = 0; i < sizeof(apsz) / sizeof(apsz[0]); i++)
        if (node && attr && lookup_attribute (apsz[i], attr))
            return apsz[i];

    return pszdefault;
}

const char *birddump_name(tree node);
const char *birddump_name(tree node)
{
    tree name = NULL_TREE;
    if (node == NULL_TREE)
        name = NULL_TREE;
    else if (DECL_P(node))
        name = DECL_NAME(node);
    else if (TYPE_P(node))
    {
        name = TYPE_NAME(node);
        if (TREE_CODE(node) == RECORD_TYPE && DECL_NAME(name))
            name = DECL_NAME(name);
    }
    if (name != NULL_TREE)
        return IDENTIFIER_POINTER(name);
    return "<none>";
}

void birddump (tree node, const char *pszFunction)
{
    tree type, type2, context;
    if (!node)
        return;

    type = TREE_TYPE (node);
    type2 = type ? TREE_TYPE (type) : NULL_TREE;
    context = DECL_P (node) ? DECL_CONTEXT (node) : NULL_TREE;

    fprintf(stderr, "dbg: node=%d %s %p %s '%s'  type=%d %s %p %s '%s'  type_type=%d %s %p %s '%s'  context=%d %s %p %s '%s' (%s)\n",
            TREE_CODE(node), code(node), (void*)node, birddump_callingconv(node), birddump_name(node),
            type ? (int)TREE_CODE(type) : -1, code(type), (void*)type, birddump_callingconv(type), birddump_name (type),
            type2 ? (int)TREE_CODE(type2) : -1, code(type2), (void*)type2, birddump_callingconv(type2), birddump_name (type2),
            context ? (int)TREE_CODE(context) : -1, code(context), (void*)context, birddump_callingconv(context), birddump_name (context),
            pszFunction);
}

#define dfprintf(a) fprintf a
/*#define DUMP(node) birddump(node, __FUNCTION__)*/
#define DUMP(node) do {} while (0)
#else
#define dfprintf(a) do {} while (0)
/*#define dfprintf(a) fprintf a */
#define DUMP(node) do {} while (0)
#endif

void
emx_eh_frame_section (const void*)
{
/*
  tree label = get_file_function_name ('F');
  The __ehInit undefined external here is to drag __ehInit/__ehInitDLL into
  the linking so DLLs are initiated correctly. (#577)
*/
  dfprintf ((stderr, "trace: emx_eh_frame_section\n"));
  switch_to_section (data_section);
  ASM_OUTPUT_ALIGN (asm_out_file, floor_log2 (PTR_SIZE));
  fputs ("\t.stabs\t\"___eh_frame__\",24,0,0,Lframe1\n", asm_out_file); /* N_SETD */
  fputs ("\t.stabs\t\"___ehInit\",1,0,0,0\n", asm_out_file);  /* N_UNDEF | N_EXT */
}

void
i386_emx_init_sections ()
{
  dfprintf ((stderr, "trace: emx_init_sections\n"));
  exception_section = data_section;
  eh_frame_section = get_unnamed_section (0,
					 emx_eh_frame_section,
					 NULL);
}

/* Add a __POST$xxx label before epilogue if -mepilogue specified */
void emx_output_function_begin_epilogue (FILE *f)
{
  dfprintf ((stderr, "trace: emx_output_function_begin_epilogue\n"));
  if (TARGET_EPILOGUE && TREE_PUBLIC (current_function_decl))
  {
    const char *func_label = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (current_function_decl));
    char *lbl = XALLOCAVEC (char, strlen (func_label) + 8);
    strcpy (lbl, "__POST$");
    if (func_label[0] == '*')
      func_label++;
    strcat (lbl, func_label);
    ASM_OUTPUT_LABEL (f, lbl);
  }
}


/* Checks if the function is using either optlink or system calling convention
   and returns 1 in those cases forcing the return value to be in memory.
   The problem (#631) was that structures less than 8 bytes were returned in
   registers. _System and _Optlink requires them to be passed in as a hidden
   parameter.
   @type is the return type.
   @fntype is function type, call expression, function declaration or null.
   The return value is 1 to force the return value into memory. Return 0 if we
   don't know. */

int emx_return_in_memory_with_fntype (tree type, tree fntype)
{
  dfprintf ((stderr, "trace: emx_return_in_memory_with_fntype\n"));
  /* (from aggregate_value_p() CVS trunk) */
  if (fntype)
    switch (TREE_CODE (fntype))
      {
      case CALL_EXPR:
        fntype = get_callee_fndecl (fntype);
        fntype = fntype ? TREE_TYPE (fntype) : 0;
        break;
      case FUNCTION_DECL:
        fntype = TREE_TYPE (fntype);
        break;
      case FUNCTION_TYPE:
      case METHOD_TYPE:
        break;
      case IDENTIFIER_NODE:
        fntype = 0;
        break;
      default:
        /* We don't expect other rtl types here.  */
        abort();
      }

  /* (future targetm.calls.return_in_memory additions) */
  if (fntype
   && AGGREGATE_TYPE_P (type)
   && (   lookup_attribute ("optlink", TYPE_ATTRIBUTES (fntype))
       || lookup_attribute ("system", TYPE_ATTRIBUTES (fntype))))
    {
      dfprintf((stderr, "emx_return_in_memory_with_fntype: returns 1\n"));
      return 1;
    }

  /* return ix86_return_in_memory (exp); - future */
  dfprintf((stderr, "emx_return_in_memory_with_fntype: returns 0\n"));
  return 0;
}

/* copy&hack from winnt.c */

static tree associated_type (tree);
static bool i386_emx_determine_dllexport_p (tree);
static bool i386_emx_determine_dllimport_p (tree);

/* Handle a "shared" attribute;
   arguments as in struct attribute_spec.handler.  */
tree
ix86_handle_shared_attribute (tree *node, tree name,
			      tree args ATTRIBUTE_UNUSED,
			      int flags ATTRIBUTE_UNUSED, bool *no_add_attrs)
{
  dfprintf ((stderr, "trace: ix86_handle_shared_attribute\n"));
  DUMP (*node);
  if (TREE_CODE (*node) != VAR_DECL)
    {
      warning (OPT_Wattributes, "%qE attribute only applies to variables",
	       name);
      *no_add_attrs = true;
    }

  return NULL_TREE;
}

/* Handle a "selectany" attribute;
   arguments as in struct attribute_spec.handler.  */
tree
ix86_handle_selectany_attribute (tree *node, tree name,
			         tree args ATTRIBUTE_UNUSED,
			         int flags ATTRIBUTE_UNUSED,
				 bool *no_add_attrs)
{
  dfprintf ((stderr, "trace: ix86_handle_selectany_attribute\n"));
  /* The attribute applies only to objects that are initialized and have
     external linkage.  However, we may not know about initialization
     until the language frontend has processed the decl. We'll check for
     initialization later in encode_section_info.  */
  if (TREE_CODE (*node) != VAR_DECL || !TREE_PUBLIC (*node))
    {
      error ("%qE attribute applies only to initialized variables"
             " with external linkage", name);
      *no_add_attrs = true;
    }

  return NULL_TREE;
}

/* Return the type that we should use to determine if DECL is
   imported or exported.  */

static tree
associated_type (tree decl)
{
  dfprintf ((stderr, "trace: associated_type\n"));
  return (DECL_CONTEXT (decl) && TYPE_P (DECL_CONTEXT (decl))
          ?  DECL_CONTEXT (decl) : NULL_TREE);
}

/* Return true if DECL should be a dllexport'd object.  */

static bool
i386_emx_determine_dllexport_p (tree decl)
{
  dfprintf((stderr, "i386_emx_determine_dllexport_p\n"));
  if (TREE_CODE (decl) != VAR_DECL && TREE_CODE (decl) != FUNCTION_DECL)
    return false;

  /* Don't export local clones of dllexports.  */
  if (!TREE_PUBLIC (decl))
    return false;

  if (lookup_attribute ("dllexport", DECL_ATTRIBUTES (decl)))
    return true;

  return false;
}

/* Return true if DECL should be a dllimport'd object.  */

static bool
i386_emx_determine_dllimport_p (tree decl)
{
  dfprintf((stderr, "i386_emx_determine_dllimport_p\n"));
  tree assoc;

  if (TREE_CODE (decl) != VAR_DECL && TREE_CODE (decl) != FUNCTION_DECL)
    return false;

  if (DECL_DLLIMPORT_P (decl))
    return true;

  /* The DECL_DLLIMPORT_P flag was set for decls in the class definition
     by  targetm.cxx.adjust_class_at_definition.  Check again to emit
     error message if the class attribute has been overridden by an
     out-of-class definition of static data.  */
  assoc = associated_type (decl);
  if (assoc && lookup_attribute ("dllimport", TYPE_ATTRIBUTES (assoc))
      && TREE_CODE (decl) == VAR_DECL
      && TREE_STATIC (decl) && TREE_PUBLIC (decl)
      && !DECL_EXTERNAL (decl)
      /* vtable's are linkonce constants, so defining a vtable is not
	 an error as long as we don't try to import it too.  */
      && !DECL_VIRTUAL_P (decl))
	error ("definition of static data member %q+D of "
	       "dllimport%'d class", decl);

  return false;
}

/* Handle the -mno-fun-dllimport target switch.  */
bool
i386_emx_valid_dllimport_attribute_p (const_tree decl)
{
  dfprintf ((stderr, "trace: i386_emx_valid_dllimport_attribute_p\n"));
   if (TARGET_NOP_FUN_DLLIMPORT && TREE_CODE (decl) == FUNCTION_DECL)
     return false;
   return true;
}

/* Return string which is the function name, identified by ID, modified
   with a suffix consisting of an atsign (@) followed by the number of
   bytes of arguments.  If ID is NULL use the DECL_NAME as base. If
   FASTCALL is true, also add the FASTCALL_PREFIX.
   Return NULL if no change required.  */

static tree
gen_stdcall_or_fastcall_suffix (tree decl, tree id, bool fastcall)
{
  HOST_WIDE_INT total = 0;
  const char *old_str = IDENTIFIER_POINTER (id != NULL_TREE ? id : DECL_NAME (decl));
  char *new_str, *p;
  tree type = TREE_TYPE (decl);
  tree arg;
  function_args_iterator args_iter;

  gcc_assert (TREE_CODE (decl) == FUNCTION_DECL);

  if (prototype_p (type))
    {
      /* This attribute is ignored for variadic functions.  */
      if (stdarg_p (type))
	return NULL_TREE;

      /* Quit if we hit an incomplete type.  Error is reported
	 by convert_arguments in c-typeck.c or cp/typeck.c.  */
      FOREACH_FUNCTION_ARGS(type, arg, args_iter)
	{
	  HOST_WIDE_INT parm_size;
	  HOST_WIDE_INT parm_boundary_bytes = PARM_BOUNDARY / BITS_PER_UNIT;

	  if (! COMPLETE_TYPE_P (arg))
	    break;

	  parm_size = int_size_in_bytes (arg);
	  if (parm_size < 0)
	    break;

	  /* Must round up to include padding.  This is done the same
	     way as in store_one_arg.  */
	  parm_size = ((parm_size + parm_boundary_bytes - 1)
		       / parm_boundary_bytes * parm_boundary_bytes);
	  total += parm_size;
	}
      }
  /* Assume max of 8 base 10 digits in the suffix.  */
  p = new_str = XALLOCAVEC (char, 1 + strlen (old_str) + 1 + 8 + 1);
  if (fastcall)
    *p++ = FASTCALL_PREFIX;
  sprintf (p, "%s@" HOST_WIDE_INT_PRINT_DEC, old_str, total);

  return get_identifier (new_str);
}

static tree
gen_system_or_optlink_prefix (tree decl, tree id)
{
  const char *old_str = IDENTIFIER_POINTER (id != NULL_TREE ? id : DECL_NAME (decl));
  char *new_str, *p;
  gcc_assert (TREE_CODE (decl) == FUNCTION_DECL);

  p = new_str = XALLOCAVEC (char, 1 + strlen (old_str));
  *p++ = '*';
  sprintf (p, "%s", old_str);
  return get_identifier (new_str);
}

/* Maybe decorate and get a new identifier for the DECL of a stdcall or
   fastcall function. The original identifier is supplied in ID. */

static tree
i386_emx_maybe_mangle_decl_assembler_name (tree decl, tree id)
{
  tree new_id = NULL_TREE;

  if (TREE_CODE (decl) == FUNCTION_DECL)
    {
      tree type_attributes = TYPE_ATTRIBUTES (TREE_TYPE (decl));
      if (lookup_attribute ("stdcall", type_attributes))
	new_id = gen_stdcall_or_fastcall_suffix (decl, id, false);
      else if (lookup_attribute ("fastcall", type_attributes))
	new_id = gen_stdcall_or_fastcall_suffix (decl, id, true);
      else if (lookup_attribute ("system", type_attributes))
	new_id = gen_system_or_optlink_prefix (decl, id);
      else if (lookup_attribute ("optlink", type_attributes))
	new_id = gen_system_or_optlink_prefix (decl, id);
    }

  return new_id;
}

/* This is used as a target hook to modify the DECL_ASSEMBLER_NAME
   in the language-independent default hook
   langhooks,c:lhd_set_decl_assembler_name ()
   and in cp/mangle,c:mangle_decl ().  */
tree
i386_emx_mangle_decl_assembler_name (tree decl, tree id)
{
  tree new_id = i386_emx_maybe_mangle_decl_assembler_name (decl, id);

  return (new_id ? new_id : id);
}

void
i386_emx_encode_section_info (tree decl, rtx rtl, int first ATTRIBUTE_UNUSED)
{
  rtx symbol;
  int flags;

  dfprintf ((stderr, "trace: i386_emx_encode_section_info\n"));
  DUMP (decl);

  /* Do this last, due to our frobbing of DECL_DLLIMPORT_P above.  */
  default_encode_section_info (decl, rtl, first);

  /* Careful not to prod global register variables.  */
  if (!MEM_P (rtl))
    return;

  symbol = XEXP (rtl, 0);
  gcc_assert (GET_CODE (symbol) == SYMBOL_REF);

#if 0
  switch (TREE_CODE (decl))
    {
    case FUNCTION_DECL:
      if (first)
	{
	  /* FIXME: In Ada, and perhaps other language frontends,
	     imported stdcall names may not yet have been modified.
	     Check and do it know.  */
         tree new_id;
         tree old_id = DECL_ASSEMBLER_NAME (decl);
	  const char* asm_str = IDENTIFIER_POINTER (old_id);
          /* Do not change the identifier if a verbatim asmspec
	     or if stdcall suffix already added. */
	  if (*asm_str == '*' || strchr (asm_str, '@'))
            break;
	  if ((new_id = i386_emx_maybe_mangle_decl_assembler_name (decl, old_id)))
	    {
	      /* These attributes must be present on first declaration,
		 change_decl_assembler_name will warn if they are added
		 later and the decl has been referenced, but duplicate_decls
		 should catch the mismatch first.  */
	      change_decl_assembler_name (decl, new_id);
	      XSTR (symbol, 0) = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (decl));
	    }
	}
      break;

    case VAR_DECL:
      if (lookup_attribute ("selectany", DECL_ATTRIBUTES (decl)))
	{
	  if (DECL_INITIAL (decl)
	      /* If an object is initialized with a ctor, the static
		 initialization and destruction code for it is present in
		 each unit defining the object.  The code that calls the
		 ctor is protected by a link-once guard variable, so that
		 the object still has link-once semantics,  */
	      || TYPE_NEEDS_CONSTRUCTING (TREE_TYPE (decl)))
	    make_decl_one_only (decl, DECL_ASSEMBLER_NAME (decl));
	  else
	    error ("%q+D:'selectany' attribute applies only to "
		   "initialized objects", decl);
	}
      break;

    default:
      return;
    }
#endif

  /* Mark the decl so we can tell from the rtl whether the object is
     dllexport'd or dllimport'd.  tree.c: merge_dllimport_decl_attributes
     handles dllexport/dllimport override semantics.  */
  flags = (SYMBOL_REF_FLAGS (symbol) &
	   ~(SYMBOL_FLAG_DLLIMPORT | SYMBOL_FLAG_DLLEXPORT));

  if (i386_emx_determine_dllexport_p (decl))
    flags |= SYMBOL_FLAG_DLLEXPORT;
  else if (i386_emx_determine_dllimport_p (decl))
    flags |= SYMBOL_FLAG_DLLIMPORT;

  SYMBOL_REF_FLAGS (symbol) = flags;
}

bool
i386_emx_binds_local_p (const_tree exp)
{
  /* PE does not do dynamic binding.  Indeed, the only kind of
     non-local reference comes from a dllimport'd symbol.  */
  if ((TREE_CODE (exp) == VAR_DECL || TREE_CODE (exp) == FUNCTION_DECL)
      && DECL_DLLIMPORT_P (exp))
    return false;

  return true;
}

/* Also strip the fastcall prefix and stdcall suffix.  */

const char *
emx_strip_name_encoding_full (const char *str)
{
  dfprintf ((stderr, "trace: i386_emx_strip_name_encoding_full(%s)\n", str));
  const char *p;
  const char *ret;
  const char *name = default_strip_name_encoding (str);

  /* Strip leading '@' on fastcall symbols.  */
  if (*name == '@')
    name++;

  /* Strip trailing "@n".  */
  p = strchr (name, '@');
  if (p)
    ret = ggc_alloc_string (name, p - name);
  else
    ret = name;
  dfprintf ((stderr, "trace: emx_strip_name_encoding_full: '%s' -> '%s'\n", str, ret));
  return ret;
}

/* Keep a list of exported symbols.  */

struct export_list GTY(())
{
  struct export_list *next;
  tree decl;
  const char *name;
  const char *asm_name;
  const char *exported_name;
  unsigned    ordinal;
  int is_data;		/* used to type tag exported symbols.  */
};

static GTY(()) struct export_list *export_head;

void
i386_emx_asm_output_aligned_decl_common (FILE *stream, tree decl,
					const char *name, HOST_WIDE_INT size,
					HOST_WIDE_INT align ATTRIBUTE_UNUSED)
{
  dfprintf ((stderr, "trace: i386_emx_asm_output_aligned_decl_common\n"));

  i386_emx_maybe_record_exported_symbol (decl, name, 1);
  rtx symbol;
  symbol = XEXP (DECL_RTL (decl), 0);
  gcc_assert (GET_CODE (symbol) == SYMBOL_REF);
  if (!SYMBOL_REF_DLLIMPORT_P (symbol)){
      /* 16 is the best we can do (segment para). */
      const int xalign = (align) > 16 ? 16 : (align);
      fprintf ((stream), "\t.comm\t");
      assemble_name ((stream), (name));
      fprintf ((stream), ", %d\t# %d\n",
	       (((size) + xalign - 1) / xalign) * xalign,
               (size));
  }
}

/* Assemble an export symbol entry.  We need to keep a list of
   these, so that we can output the export list at the end of the
   assembly.  We used to output these export symbols in each function,
   but that causes problems with GNU ld when the sections are
   linkonce.  */

void
i386_emx_maybe_record_exported_symbol (tree decl, const char *name, int is_data EMX_DBG_LOC_DECL)
{
  rtx symbol;
  struct export_list *p;

  if (!decl)
    return;

  symbol = XEXP (DECL_RTL (decl), 0);
  gcc_assert (GET_CODE (symbol) == SYMBOL_REF);
  if (!SYMBOL_REF_DLLEXPORT_P (symbol)){
    return;}

  dfprintf ((stderr,
             "trace: i386_emx_maybe_record_exported_symbol: %s %s(%d)\n"
             "           name='%s' is_data=%d\n", EMX_DBG_LOC_RARGS, name, is_data));

  gcc_assert (TREE_PUBLIC (decl));

  p = (struct export_list *) xmalloc (sizeof *p);
  p->next = export_head;
  p->name = name;
  p->asm_name = NULL;
  p->exported_name = NULL;
  p->ordinal = 0;
  p->is_data = is_data;
  export_head = p;
}


/* This is called at the end of assembly.  For each external function
   which has not been defined, we output a declaration now.  We also
   output the .drectve section.  */

void
i386_emx_file_end (void)
{
  dfprintf ((stderr, "trace: emx_file_end\n"));
  struct extern_list *p;

#if 0 /* don't think we need this */
  for (p = extern_head; p != NULL; p = p->next)
    {
      tree decl;

      decl = p->decl;

      /* Positively ensure only one declaration for any given symbol.  */
      if (! TREE_ASM_WRITTEN (decl)
	  && TREE_SYMBOL_REFERENCED (DECL_ASSEMBLER_NAME (decl)))
	{
	  TREE_ASM_WRITTEN (decl) = 1;
	  i386_emx_declare_function_type (asm_out_file, p->name,
					 TREE_PUBLIC (decl));
	}
    }
#endif

  if (export_head)
    {
      struct export_list *q;
      fprintf (asm_out_file, "\t%s Exports: \"<exportname>,<ordinal>=<asmname>,<code|data>\", N_EXP,0,0,-42\n", ASM_COMMENT_START);

      for (q = export_head; q != NULL; q = q->next)
	{
          static const char * const types[2] = {",code", ",data"};
          tree decl = q->decl;
          if (decl)
            {
              DUMP(decl);
              /* breaks thunks. Windows ain't doing this either.
                 If it turns out to break stuff, try add "|| DECL_COMDAT (decl)".
              if (!TREE_STATIC (decl))
                {
                  dfprintf ((stderr, "dbg: skipping %p. !TREE_STATIC\n", decl));
                  continue;
                }
               */
              if (!q->asm_name)
                q->asm_name = q->exported_name ? q->exported_name
                  : IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (decl));
            }
          if (!q->asm_name)
            q->asm_name = q->exported_name ? q->exported_name : q->name;
          if (!q->exported_name && !q->ordinal)
            q->exported_name = q->asm_name ? q->asm_name : q->name;
          fprintf (asm_out_file, "\t.stabs\t\"");
          ASM_OUTPUT_LABELREF(asm_out_file, q->exported_name);
          fprintf (asm_out_file, ",%u=", q->ordinal);
          ASM_OUTPUT_LABELREF(asm_out_file, q->asm_name);
          fprintf (asm_out_file, "%s\",0x6c,0,0,-42\t%s %s\n", /* 0x6c == N_EXP */
                   types[q->is_data], ASM_COMMENT_START, q->name);

	}
    }
}

/* Always default to .text section.  */

section *
emx_function_section (tree decl ATTRIBUTE_UNUSED,
                      enum node_frequency freq ATTRIBUTE_UNUSED,
                      bool startup ATTRIBUTE_UNUSED,
                      bool exit ATTRIBUTE_UNUSED)
{
  return NULL;
}
