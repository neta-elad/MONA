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

#ifndef __SYMBOLTABLE_H
#define __SYMBOLTABLE_H

#include <memory>
#include <utility>

#include "deque.h"
#include "ast.h"
#include "printline.h"
#include "ident.h"

class Name {
public:
  Name() {} // dummy
  Name(char *s, Pos p) :
    str(s), pos(p) {}
  Name(const Name& n) :
    str(n.str), pos(n.pos) {}

  char *str;
  Pos pos;
};

class NameList: public DequeGC<Name*> {};

enum MonaTypeTag { 
  Varname0, Varname1, Varname2, VarnameTree,
  Parname0, Parname1, Parname2, ParnameU,
  Univname, Predname, Constname, Statespacename,
  Typename
};

class Entry;

class SymbolTable {
  Ident  insert(Entry*);
  void   remove(int idx); // must be last-in-first-out order
  Entry &lookup(Name*);
  void   check(Name*);
  int    hash(char*);

  Deque<char*>  *symbols;          // hashtable of symbols
                                   // in essence, a cache of strings and a centralized place
                                   // to keep and then free unused strings

  Deque<Entry*> *declarationTable; // hashtable String->Entry
                                   // this queue holds a non-owning reference
                                   // (the owning pointer is at identMap

  Deque<int>     localStack;       // stack of hashtable indexes (-1 sentinel)
  Deque<std::pair<int, Ident>>     freshStack;       // stack of "fresh" pairs of hashtable index and Ident (-1 sentinel)

  Deque<Entry*>  identMap;         // map Ident->Entry
                                   // owns the Entry

  IdentList      allUnivIds;       // all universe Idents, sorted
  IdentList      allRealUnivIds;   // allUnivIds except dummy
  IdentList      statespaceIds;    // all statespace IDs in order
  unsigned       size;             // hashtable array size

public:
   SymbolTable(int size);
  ~SymbolTable();

  char  *insertString(char *);

  Ident  insertPred(Name*);
  Ident  insertVar(Name*, MonaTypeTag, IdentList *univs, 
		   bool local = false, bool implicit = false,
		   bool fresh = false);
  Ident  insertUniv(Name*, char *pos, bool dummy = false);
  Ident  insertUniv(Name*, Ident type);
  Ident  insertConst(Name*, int value);
  Ident  insertStatespace(Name*);
  Ident  insertType(Name*);
  void   setTypeVariants(Ident, ASTVariantList*);
  void   setTypeNumber(Ident, int);
  void   setTypeReachable(Ident);
  void   setSSType(Ident, Ident);

  Ident  insertFresh(MonaTypeTag t, IdentList *univs = NULL, bool implicit = true);

  void   openLocal();
  void   closeLocal();

  void   openFresh();
  void   closeFresh();
  
  Ident           lookupIdent(Name*);
  char           *lookupSymbol(Ident);
  MonaTypeTag     lookupType(Ident);
  MonaTypeTag     lookupType(Name*);
  Ident           lookupStatespaceId(int number);
  int             lookupOrder(Ident);
  int             lookupValue(Name*);
  int             lookupNumber(Name*);
  int             lookupNumber(Ident ident);
  int             lookupDepth(Ident);
  IdentList      *lookupUnivIdents(NameList&);
  IdentList      *lookupUnivs(Ident);
  ASTForm        *lookupRestriction(Ident);
  bool            lookupImplicit(Ident);
  int             lookupUnivNumber(Ident);
  char           *lookupPos(Ident);
  Ident           lookupUnivType(Ident);
  ASTVariantList *lookupTypeVariants(Ident);
  int             lookupTypeNumber(Ident);
  bool            lookupTypeReachable(Ident);
  IdentList      *lookupTypeStatespaces(Ident);
  Ident           lookupSSType(Ident);
  bool            exists(char*);
  ASTForm        *getRestriction(Ident, Ident *formal);
  ASTForm        *getDefault1Restriction(Ident *formal);
  ASTForm        *getDefault2Restriction(Ident *formal);

  IdentList  *allUnivs();
  IdentList  *allRealUnivs();
  void        updateRestriction(Ident, ASTForm *);
  void        updateStateSpaces(Ident, Deque<unsigned> *statespaces);
  void        updateUnivPos(Ident, char *pos);
  void        addTypeStatespace(Ident, char *ssname);

  void        checkUniqueness(NameList&);

  void        setDefaultRestriction(MonaTypeTag, ASTForm *, Ident);

  void        dump(); // dump contents

  void        clear();

  unsigned  noIdents;       // total number of identifiers
  int       noSS;           // number of state spaces
  Ident     defaultIdent1;  // formal parameter for defaultwhere1
  Ident     defaultIdent2;  // formal parameter for defaultwhere2
  ASTForm  *defaultRestriction1; // default restriction for first order variables
  ASTForm  *defaultRestriction2; // default restriction for second order variables
};

#endif
