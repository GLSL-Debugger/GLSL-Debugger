/*
 * SymbolTable.h
 *
 *  Created on: 06.03.2014
 *
 * This is the light version of glsl_symbol_table
 */

#pragma once
#ifndef SH_SYMBOL_TABLE_H_
#define SH_SYMBOL_TABLE_H_

#include "glsl/ralloc.h"
#include <new>
#include <assert.h>

extern "C" {
#include "program/symbol_table.h"
}

class sh_symbol_table_entry;
class ast_function_definition;
class ShVariable;

struct sh_symbol_table {
private:
   static void
   _ast_symbol_table_destructor (sh_symbol_table *table)
   {
      table->~sh_symbol_table();
   }

public:
   /* Callers of this ralloc-based new need not call delete. It's
    * easier to just ralloc_free 'ctx' (or any of its ancestors). */
   static void* operator new(size_t size, void *ctx)
   {
      void *table;

      table = ralloc_size(ctx, size);
      assert(table != NULL);

      ralloc_set_destructor(table, (void (*)(void*)) _ast_symbol_table_destructor);

      return table;
   }

   /* If the user *does* call delete, that's OK, we will just
    * ralloc_free in that case. Here, C++ will have already called the
    * destructor so tell ralloc not to do that again. */
   static void operator delete(void *table)
   {
      ralloc_set_destructor(table, NULL);
      ralloc_free(table);
   }

   sh_symbol_table();
   ~sh_symbol_table();

   /* In 1.10, functions and variables have separate namespaces. */
   bool separate_function_namespace;

   void push_scope();
   void pop_scope();

   /**
    * Determine whether a name was declared at the current scope
    */
   bool name_declared_this_scope(const char *name);

   /**
    * \name Methods to add symbols to the table
    *
    * There is some temptation to rename all these functions to \c add_symbol
    * or similar.  However, this breaks symmetry with the getter functions and
    * reduces the clarity of the intention of code that uses these methods.
    */
   /*@{*/
   bool add_variable(ShVariable* v);
   bool add_function(ast_function_definition *f);
   /*@}*/

   /**
    * Add an function at global scope without checking for scoping conflicts.
    */
   void add_global_function(ast_function_definition *f);

   /**
    * \name Methods to get symbols from the table
    */
   /*@{*/
   ShVariable *get_variable(const char *name);
   ast_function_definition *get_function(const char *name);
   /*@}*/

private:
   sh_symbol_table_entry *get_entry(const char *name);

   struct _mesa_symbol_table *table;
   void *mem_ctx;
};


#endif /* SH_SYMBOL_TABLE_H_ */
