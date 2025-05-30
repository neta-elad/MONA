/*
 * MONA
 * Copyright (C) 1997-2013 Aarhus University.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the  Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335,
 * USA.
 */

#include "offsets.h"
#include "predlib.h"

#include <utility>
#include "symboltable.h"

extern Offsets offsets;
extern SymbolTable symbolTable;

void
PredicateLib::insert(PredLibEntry *p)
{
  unsigned i = p->name % PREDLIB_SIZE;
  
  table[i].push_back(p);
}

PredicateLib::PredicateLib()
{
}

PredicateLib::~PredicateLib()
{
  int i;
  for (i = 0; i < PREDLIB_SIZE; i++)
    for (const auto &j : table[i])
      delete j;
}

void 
PredicateLib::insert(IdentList *formals, 
		     IdentList *frees,
		     IdentList *bound,
		     ASTForm   *formula,
		     bool       isMacro,
		     Ident      name,
		     char      *source)
{
  insert(new PredLibEntry(formals, frees, bound,  
			  formula, isMacro, name, source));
}
void
PredicateLib::insert(IdentList *formals,
		     IdentList *frees,
		     IdentList *bound,
		     ASTFormPtr formula,
		     bool       isMacro,
		     Ident      name)
{
  insert(new PredLibEntry(formals, frees, bound,
			  std::move(formula), isMacro, name, nullptr));
}

void
PredicateLib::remove(Ident id) {
  unsigned i = id % PREDLIB_SIZE;

  std::vector<PredLibEntry *>::iterator pp;
  for (pp = table[i].begin(); pp != table[i].end(); ++pp)
    if ((*pp)->name == id)
      break;

  if ((*pp)->name == id) {
    delete *pp;
    table[i].erase(pp);
    return;
  }

  invariant(false);
}

PredLibEntry *
PredicateLib::lookup(Ident id)
{
  unsigned i = id % PREDLIB_SIZE;

  for (const auto &entry : table[i]) {
    if (entry->name == id) {
      return entry;
    }
  }

  invariant(false);
  return NULL;
}

TestResult
PredicateLib::testTypes(Ident name, ASTList *acts, int *no, bool loose)
{ 
  PredLibEntry *entry = lookup(name);

  if (entry->formals->size() != acts->size())
    return tWrongNoParameters; 
  
  ASTList::iterator a;
  IdentList::iterator f;
  int i;
  for (f = entry->formals->begin(), a = acts->begin(), i = 1;
       f != entry->formals->end();
       f++, a++, i++) {
  
    if (no)
      *no = i;

    MonaTypeTag t = symbolTable.lookupType(*f);

    switch ((*a)->order) {

    case oTerm1:
      if (t != Parname1 && t != ParnameU && (!loose || t != Varname1))
	return tWrongParameterType;
      break;

    case oTerm2:
      if (t != Parname2 && t != ParnameU && (!loose || t != Varname2))
	return tWrongParameterType;
      break;

    case oForm:
      if (t != Parname0 && (!loose || t != Varname0))
	return tWrongParameterType;
      break;

    case oUniv:
      if (t != ParnameU)
	return tWrongParameterType;
      break;
    }
  } 

  return tOK;
}

PredLibEntry*
PredicateLib::first()
{
  idx = -1;

  while (++idx < PREDLIB_SIZE)
    if ((current = table[idx].begin()) != table[idx].end())
      return *current;

  return NULL;
}

PredLibEntry*
PredicateLib::next()
{
  if (++current != table[idx].end())
    return *current;

  while (++idx < PREDLIB_SIZE)
    if ((current = table[idx].begin()) != table[idx].end())
      return *current;

  return NULL;
}

void PredicateLib::clear() {
  this->~PredicateLib();
  new (this) PredicateLib();
}

void PredicateLib::stats() {
  std::cout << table->size() << " predicates\n";
}