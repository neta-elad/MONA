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

#ifndef __PREDLIB_H
#define __PREDLIB_H

#include <utility>
#include <vector>

#include "ast.h"
#include "signature.h"
#include "st_dfa.h"
#include "st_gta.h"
#include "deque.h"

class PredLibEntry {
public:
  PredLibEntry(IdentList *eFormals, IdentList *eFrees, IdentList *eBound,
	       ASTForm *eFormula, bool eIsMacro, int eName, char *eSource) :
    formals(eFormals), frees(eFrees), bound(eBound), ast(eFormula),
    isMacro(eIsMacro), name(eName), source(eSource) {}
  PredLibEntry(IdentList *eFormals, IdentList *eFrees, IdentList *eBound,
	       ASTFormPtr eFormula, bool eIsMacro, int eName, char *eSource) :
    formals(eFormals), frees(eFrees), bound(eBound), ast(std::move(eFormula)),
    isMacro(eIsMacro), name(eName), source(eSource) {}
  ~PredLibEntry() 
  {delete formals; delete frees; delete bound;}

  IdentList *formals;
  IdentList *frees;
  IdentList *bound;
  ASTFormPtr ast;
  bool       isMacro;
  Ident      name;
  char      *source;
};

enum TestResult {
  tOK,
  tWrongNoParameters,
  tWrongParameterType
};

#define PREDLIB_SIZE 113

class PredicateLib {
  std::vector<PredLibEntry *> table[PREDLIB_SIZE];

  void insert(PredLibEntry *);

  int            idx;
  std::vector<PredLibEntry *>::iterator current;

public:
  PredicateLib();
  ~PredicateLib();
  
  void insert(IdentList *formals, IdentList *frees, IdentList *bound,
		       ASTForm *formula, bool isMacro, Ident name, char *source);
  void insert(IdentList *formals, IdentList *frees, IdentList *bound,
		       ASTFormPtr formula, bool isMacro, Ident name);
  void remove(Ident);
  PredLibEntry *lookup(Ident);
  TestResult    testTypes(Ident name, ASTList *acts, int *no = NULL, bool loose = false);
  PredLibEntry *first();
  PredLibEntry *next();
  void clear();
  void stats();
};

#endif
