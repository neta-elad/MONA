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

#ifndef __AST_H
#define __AST_H

#include <memory>
#include <utility>
#include <vector>

#include "ident.h"
#include "codetable.h"
#include "printline.h"

////////// Bit list ///////////////////////////////////////////////////////////

enum Bit {
  Zero, One
};

class BitList: public Deque<Bit> {
public:
  BitList(char *str);

  void dump();
};

////////// ASTTermCode ////////////////////////////////////////////////////////

class ASTTermCode {
public:
  ASTTermCode() {}
  ASTTermCode(Ident v, bool f, VarCode c) :
    var(v), fresh(f), code(c) {}
  ASTTermCode(const ASTTermCode *t) :
    var(t->var), fresh(t->fresh), code(t->code) {}

  Ident var; // the variable code is bound to
  bool fresh; // true if var is fresh in code, i.e. needs to be projected away 
  VarCode code;
};

////////// Substitution ///////////////////////////////////////////////////////

enum SubstCodeKind {sTermCode, sVarCode, sIdent};

class SubstCode { // a substitution array is terminated with .formal=-1
public:
  Ident formal;
  SubstCodeKind kind;
  union {
    ASTTermCode *termCode;
    VarCode *varCode;
    Ident ident;
  };
};

////////// Abstract syntax tree ///////////////////////////////////////////////

enum ASTKind {
  aVar1, aDot1, aUp1, aInt, aPlus1, aMinus1, aPlusModulo1, aMinusModulo1, 
  aPlus2, aMinus2, aMin, aMax, aInterval, aVar2, aEmpty, aUnion, aInter, 
  aSetminus, aSet, aVar0, aTrue, aFalse, aIn, aNotin, aRoot, aEmptyPred, 
  aSub, aEqual1, aEqual2, aNotEqual1, aFirstOrder, aIdLeft, aPresbConst,
  aNotEqual2, aLess, aLessEq, aImpl, aBiimpl, aAnd, aOr, aNot, aEx0, aEx1, 
  aEx2, aAll0, aAll1, aAll2, aLet0, aLet1, aLet2, aCall, 
  aUniv, aImport, aExport, aPrefix, aDot2, aUp2, aRootPred, aInStateSpace1,
  aInStateSpace2, aTreeRoot, aWellFormedTree, aTree, aTerm2Formula, 
  aSomeType, aRestrict
};

enum ASTOrder {oTerm1, oTerm2, oForm, oUniv};

class AST {
public:
  AST(ASTOrder o, ASTKind k, Pos p) :
    order(o), kind(k), pos(p) {}
  virtual ~AST() {};

  virtual void freeVars(IdentList*, IdentList*) {};
  virtual void dump() {};

  ASTOrder order;
  ASTKind kind;
  Pos pos;
}; 

class ASTList: public DequeGC<AST*> {
public:
  void dump();
};

using ASTPtr = std::shared_ptr<AST>;

class SharedASTList : public std::vector<ASTPtr> {
public:
  SharedASTList(ASTList *alist) :
    std::vector<ASTPtr>(fromAstList(alist)) {}
  SharedASTList() {}

  void dump();
  std::unique_ptr<ASTList> toAstList();

  static std::vector<ASTPtr> fromAstList(ASTList *alist);
};

class ASTTerm: public AST {
public:
  ASTTerm(ASTOrder o, ASTKind kind, Pos p) :
    AST(o, kind, p) {}

  virtual ASTTermCode *makeCode(SubstCode *subst = NULL) = 0;
  void dump() = 0; 
};

class ASTTerm1: public ASTTerm {
public:
  ASTTerm1(ASTKind kind, Pos p) :
    ASTTerm(oTerm1, kind, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL) = 0;
  void dump() = 0; 
};

using ASTTerm1Ptr = std::shared_ptr<ASTTerm1>;

class Term1List: public DequeGC<ASTTerm1*> {};

class ASTTerm2: public ASTTerm {
public:
  ASTTerm2(ASTKind kind, Pos p) :
    ASTTerm(oTerm2, kind, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL) = 0;
  void dump() = 0; 
};

using ASTTerm2Ptr = std::shared_ptr<ASTTerm2>;

class Term2List: public DequeGC<ASTTerm2*> {};

class ASTForm: public AST {
public:
  ASTForm(ASTKind kind, Pos p) :
    AST(oForm, kind, p) {}

  virtual VarCode makeCode(SubstCode *subst = NULL) = 0;
  void dump() = 0;
};

using ASTFormPtr = std::shared_ptr<ASTForm>;

class FormList: public DequeGC<ASTForm*> {};

class ASTUniv: public AST {
public:
  ASTUniv(Ident univ, Pos p) :
    AST(oUniv, aUniv, p), u(univ) {}

  void dump();

  Ident u;
};

class ASTComponent {
public:
  ASTComponent(char *n, char *t, Pos p) :
    name(n), type(t), pos(p), path(0) {}
  ~ASTComponent() {delete path;}

  void dump();

  char *name;
  char *type;
  Pos pos;
  BitList *path;
};

class ASTComponentList: public DequeGC<ASTComponent*> {
public: 
  void dump();
};

class ASTVariant {
public:
  ASTVariant(char *n, ASTComponentList *c, Pos p) :
    name(n), components(c), pos(p), path(0) {}
  ~ASTVariant() {delete components; delete path;}

  void dump();

  char *name;
  ASTComponentList *components;
  Pos pos;
  BitList *path;
};

class ASTVariantList: public DequeGC<ASTVariant*> {
public:
  void dump();
};

////////// Abstract classes ///////////////////////////////////////////////////

class ASTTerm1_n: public ASTTerm1 {
public:
  ASTTerm1_n(ASTKind kind, int c, Pos p) :
    ASTTerm1(kind, p), n(c) {}

protected:
  int n;
}; 

class ASTTerm1_T: public ASTTerm1 {
public:
  ASTTerm1_T(ASTKind kind, ASTTerm2 *TT, Pos p) :
    ASTTerm1(kind, p), T(TT) {} //todo(neta) delete
  ASTTerm1_T(ASTKind kind, ASTTerm2Ptr TT, Pos p) :
    ASTTerm1(kind, p), T(std::move(TT)) {}
  ~ASTTerm1_T() = default;

  void freeVars(IdentList*, IdentList*);

protected:
  ASTTerm2Ptr T;
};

class ASTTerm1_t: public ASTTerm1 {
public:
  ASTTerm1_t(ASTKind kind, ASTTerm1 *tt, Pos p) :
    ASTTerm1(kind, p), t(tt) {}
  ~ASTTerm1_t() {delete t;}

  void freeVars(IdentList*, IdentList*);

protected:
  ASTTerm1 *t;
};

class ASTTerm1_tn: public ASTTerm1 {
public:
  ASTTerm1_tn(ASTKind kind, ASTTerm1 *tt, int nn, Pos p) :
    ASTTerm1(kind, p), t(tt), n(nn) {} //todo(neta) remove
  ASTTerm1_tn(ASTKind kind, ASTTerm1Ptr tt, int nn, Pos p) :
    ASTTerm1(kind, p), t(std::move(tt)), n(nn) {}
  ~ASTTerm1_tn() = default;

  void freeVars(IdentList*, IdentList*);

protected:
  ASTTerm1Ptr t;
  int n;
}; 

class ASTTerm1_tnt: public ASTTerm1 {
public:
  ASTTerm1_tnt(ASTKind kind, ASTTerm1 *tt1, int nn, ASTTerm1 *tt2, Pos p) :
    ASTTerm1(kind, p), t1(tt1), t2(tt2), n(nn) {}
  ~ASTTerm1_tnt() {delete t1; delete t2;}

  void freeVars(IdentList*, IdentList*);

protected:
  ASTTerm1 *t1;
  ASTTerm1 *t2;
  int n;
};

class ASTTerm2_TT : public ASTTerm2 {
public:
  ASTTerm2_TT(ASTKind kind, ASTTerm2 *TT1, ASTTerm2 *TT2, Pos p)
    : ASTTerm2_TT(kind, ASTTerm2Ptr(TT1), ASTTerm2Ptr(TT2), p) {
  } // todo(neta.e) delete
  ASTTerm2_TT(ASTKind kind, ASTTerm2Ptr TT1, ASTTerm2Ptr TT2, Pos p = dummyPos)
    : ASTTerm2(kind, p), T1(std::move(TT1)), T2(std::move(TT2)) {
  }

  ~ASTTerm2_TT() = default;

  void freeVars(IdentList*, IdentList*);
  
protected:
  ASTTerm2Ptr T1;
  ASTTerm2Ptr T2;
};

class ASTTerm2_Tn: public ASTTerm2 {
public:
  ASTTerm2_Tn(ASTKind kind, ASTTerm2 *TT, int nn, Pos p) :
    ASTTerm2(kind, p), T(TT), n(nn) {}
  ~ASTTerm2_Tn() {delete T;}

  void freeVars(IdentList*, IdentList*);
  
protected:
  ASTTerm2 *T;
  int n;
};

class ASTForm_tT: public ASTForm {
public:
  ASTForm_tT(ASTKind kind, ASTTerm1 *tt1, ASTTerm2 *TT2, Pos p) : //todo(neta) delete
    ASTForm(kind, p), t1(tt1), T2(TT2) {}
  ASTForm_tT(ASTKind kind, ASTTerm1Ptr tt1, ASTTerm2Ptr TT2, Pos p) :
    ASTForm(kind, p), t1(std::move(tt1)), T2(std::move(TT2)) {}
  ~ASTForm_tT() = default;

  void freeVars(IdentList*, IdentList*);

protected:
  ASTTerm1Ptr t1;
  ASTTerm2Ptr T2;
};

class ASTForm_T: public ASTForm {
public:
  ASTForm_T(ASTKind kind, ASTTerm2 *TT, Pos p) : //todo(neta) delete
    ASTForm(kind, p), T(TT) {}
  ASTForm_T(ASTKind kind, ASTTerm2Ptr TT, Pos p) : //todo(neta) delete
    ASTForm(kind, p), T(std::move(TT)) {}
  ~ASTForm_T() = default;
    
  void freeVars(IdentList*, IdentList*);

protected:
  ASTTerm2Ptr T;
};

class ASTForm_TT: public ASTForm {
public:
  ASTForm_TT(ASTKind kind, ASTTerm2 *TT1, ASTTerm2 *TT2, Pos p) :
    ASTForm(kind, p), T1(TT1), T2(TT2) {} //todo(neta) delete
  ASTForm_TT(ASTKind kind, ASTTerm2Ptr TT1, ASTTerm2Ptr TT2, Pos p) :
    ASTForm(kind, p), T1(std::move(TT1)), T2(std::move(TT2)) {} //todo(neta) delete
  ~ASTForm_TT() = default;

  void freeVars(IdentList*, IdentList*);

protected:
  ASTTerm2Ptr T1;
  ASTTerm2Ptr T2;
};

class ASTForm_tt: public ASTForm {
public:
  ASTForm_tt(ASTKind kind, ASTTerm1 *tt1, ASTTerm1 *tt2, Pos p) :
    ASTForm(kind, p), t1(tt1), t2(tt2) {} // todo(neta) delete
  ASTForm_tt(ASTKind kind, ASTTerm1Ptr tt1, ASTTerm1Ptr tt2, Pos p) :
    ASTForm(kind, p), t1(std::move(tt1)), t2(std::move(tt2)) {}
  ~ASTForm_tt() = default;

  void freeVars(IdentList*, IdentList*);

protected:
  ASTTerm1Ptr t1;
  ASTTerm1Ptr t2;
};

class ASTForm_nt: public ASTForm {
public:
  ASTForm_nt(ASTKind kind, int nn, ASTTerm1 *tt, Pos p) :
    ASTForm(kind, p), n(nn), t(tt) {} 
  ~ASTForm_nt() {delete t;}

  void freeVars(IdentList*, IdentList*);

protected:
  int n;
  ASTTerm1 *t;
};

class ASTForm_nT: public ASTForm {
public:
  ASTForm_nT(ASTKind kind, int nn, ASTTerm2 *TT, Pos p) :
    ASTForm(kind, p), n(nn), T(TT) {}
  ~ASTForm_nT() {delete T;}

  void freeVars(IdentList*, IdentList*);

protected:
  int n;
  ASTTerm2 *T;
};

class ASTForm_f: public ASTForm {
public:
  ASTForm_f(ASTKind kind, ASTForm *ff, Pos p) :
    ASTForm(kind, p), f(ff) {}
  ~ASTForm_f() {delete f;}

  void freeVars(IdentList*, IdentList*);

protected:
  ASTForm *f;
};

class ASTForm_ff: public ASTForm {
public:
  ASTForm_ff(ASTKind kind, ASTForm *ff1, ASTForm *ff2, Pos p) : // todo(neta) delete
    ASTForm_ff(kind, ASTFormPtr(ff1), ASTFormPtr(ff2), p) {}
  ASTForm_ff(ASTKind kind, ASTFormPtr ff1, ASTFormPtr ff2, Pos p) :
    ASTForm(kind, p), f1(std::move(ff1)), f2(std::move(ff2)) {}
  ~ASTForm_ff() = default;

  void freeVars(IdentList*, IdentList*);

protected:
  ASTFormPtr f1;
  ASTFormPtr f2;
};

class ASTForm_vf: public ASTForm {
public:
  ASTForm_vf(ASTKind kind, IdentList *vll, ASTForm *ff, Pos p) : // todo(neta) delete
    ASTForm_vf(kind, vll, ASTFormPtr(ff), p) {}
  ASTForm_vf(ASTKind kind, IdentList *vll, ASTFormPtr ff, Pos p) :
    ASTForm(kind, p), vl(vll), f(std::move(ff)) {}
  ~ASTForm_vf() {delete vl;}

  void freeVars(IdentList*, IdentList*);

protected:
  IdentList *vl;
  ASTFormPtr f;
};

class ASTForm_uvf: public ASTForm {
public:
  ASTForm_uvf(ASTKind kind, IdentList *ull, IdentList *vll, 
		  ASTForm *ff, Pos p) :
    ASTForm(kind, p), ul(ull), vl(vll), f(ASTFormPtr(ff)) {} // todo(neta) delete
  ASTForm_uvf(ASTKind kind, IdentList *ull, IdentList *vll,
		  ASTFormPtr ff, Pos p) :
    ASTForm(kind, p), ul(ull), vl(vll), f(std::move(ff)) {}
  ~ASTForm_uvf() {delete ul; delete vl;}

  void freeVars(IdentList*, IdentList*);

protected:
  IdentList *ul;
  IdentList *vl;
  ASTFormPtr f;
};

////////// Syntactical categories of ASTTerm1 /////////////////////////////////

class ASTTerm1_Var1: public ASTTerm1_n {
public:
  ASTTerm1_Var1(int n, Pos p = dummyPos) :
    ASTTerm1_n(aVar1, n, p) {}

  void freeVars(IdentList*, IdentList*);
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTTerm1_Dot: public ASTTerm1_t {
public:
  ASTTerm1_Dot(ASTTerm1 *tt, BitList *bts, Pos p) :
    ASTTerm1_t(aDot1, tt, p), bits(bts) {}
  ~ASTTerm1_Dot() {delete bits;}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  BitList *bits;
}; 

class ASTTerm1_Up: public ASTTerm1_t {
public:
  ASTTerm1_Up(ASTTerm1 *tt, Pos p) :
    ASTTerm1_t(aUp1, tt, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
}; 

class ASTTerm1_Root: public ASTTerm1 {
public:
  ASTTerm1_Root(Ident u, Pos p) :
    ASTTerm1(aRoot, p), univ(u) {}
  
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  Ident univ;
};

class ASTTerm1_Int: public ASTTerm1_n {
public:
  ASTTerm1_Int(int n, Pos p = dummyPos) :
    ASTTerm1_n(aInt, n, p) {}

  void freeVars(IdentList*, IdentList*) {}
  int value();
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
}; 

class ASTTerm1_Plus: public ASTTerm1_tn {
public:
  ASTTerm1_Plus(ASTTerm1 *t, int n, Pos p) :
    ASTTerm1_tn(aPlus1, t, n, p) {} //todo(neta) remove
  ASTTerm1_Plus(ASTTerm1Ptr t, int n, Pos p = dummyPos) :
    ASTTerm1_tn(aPlus1, std::move(t), n, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
}; 

class ASTTerm1_Minus: public ASTTerm1_tn {
public:
  ASTTerm1_Minus(ASTTerm1 *t, int n, Pos p) :
    ASTTerm1_tn(aMinus1, t, n, p) {} // todo(neta) remove
  ASTTerm1_Minus(ASTTerm1Ptr t, int n, Pos p = dummyPos) :
    ASTTerm1_tn(aMinus1, std::move(t), n, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  VarCode unfold(int v1, int v2, int n, SubstCode *subst, Pos pos);
  void dump();
}; 

class ASTTerm1_PlusModulo: public ASTTerm1_tnt {
public:
  ASTTerm1_PlusModulo(ASTTerm1 *t1, int n, ASTTerm1 *t2, Pos p) :
    ASTTerm1_tnt(aPlusModulo1, t1, n, t2, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  VarCode unfold(int v1, int v2, int n, int v3, SubstCode *subst, Pos pos);
  void dump();
}; 

class ASTTerm1_MinusModulo: public ASTTerm1_tnt {
public:
  ASTTerm1_MinusModulo(ASTTerm1 *t1, int n, ASTTerm1 *t2, Pos p) :
    ASTTerm1_tnt(aMinusModulo1, t1, n, t2, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  VarCode unfold(int v1, int v2, int n, int v3, SubstCode *subst, Pos pos);
  void dump();
}; 

class ASTTerm1_Min: public ASTTerm1_T {
public:
  ASTTerm1_Min(ASTTerm2 *T, Pos p) :
    ASTTerm1_T(aMin, T, p) {} // todo(neta) delete
  ASTTerm1_Min(ASTTerm2Ptr T, Pos p = dummyPos) :
    ASTTerm1_T(aMin, std::move(T), p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTTerm1_Max: public ASTTerm1_T {
public:
  ASTTerm1_Max(ASTTerm2 *T, Pos p) :
    ASTTerm1_T(aMax, T, p) {} //todo(neta) - delete
  ASTTerm1_Max(ASTTerm2Ptr T, Pos p = dummyPos) :
    ASTTerm1_T(aMax, std::move(T), p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTTerm1_TreeRoot: public ASTTerm1_T {
public:
  ASTTerm1_TreeRoot(ASTTerm2 *T, Pos p) :
    ASTTerm1_T(aTreeRoot, T, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
};

////////// Syntactical categories of ASTTerm2 /////////////////////////////////

class ASTTerm2_Var2: public ASTTerm2 {
public:
  ASTTerm2_Var2(int nn, Pos p = dummyPos) :
    ASTTerm2(aVar2, p), n(nn) {}

  void freeVars(IdentList*, IdentList*);
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  Ident getVar() {return n;};
  void dump();

protected:
  int n;
}; 

class ASTTerm2_VarTree: public ASTTerm2 {
public:
  ASTTerm2_VarTree(int nn, Pos p) :
    ASTTerm2(aTree, p), n(nn) {}

  void freeVars(IdentList*, IdentList*);
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  Ident getVar() {return n;};
  void dump();

protected:
  int n;
}; 

class ASTTerm2_Dot: public ASTTerm2 {
public:
  ASTTerm2_Dot(ASTTerm2 *TT, BitList *bts, Pos p) :
    ASTTerm2(aDot2, p), bits(bts), T(TT) {}
  ~ASTTerm2_Dot() {delete bits; delete T;}

  void freeVars(IdentList*, IdentList*);
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  BitList *bits;
  ASTTerm2 *T;
}; 

class ASTTerm2_Up: public ASTTerm2 {
public:
  ASTTerm2_Up(ASTTerm2 *TT, Pos p) :
    ASTTerm2(aUp2, p), T(TT) {}
  ~ASTTerm2_Up() {delete T;}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void freeVars(IdentList*, IdentList*);
  void dump();

protected:
  ASTTerm2 *T;
}; 

class ASTTerm2_Empty: public ASTTerm2 {
public:
  ASTTerm2_Empty(Pos p = dummyPos) :
    ASTTerm2(aEmpty, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
}; 

class ASTTerm2_Union: public ASTTerm2_TT {
public:
  ASTTerm2_Union(ASTTerm2 *T1, ASTTerm2 *T2, Pos p) :
    ASTTerm2_TT(aUnion, T1, T2, p) {} // todo(neta) remove
  ASTTerm2_Union(ASTTerm2Ptr T1, ASTTerm2Ptr T2, Pos p = dummyPos) :
    ASTTerm2_TT(aUnion, std::move(T1), std::move(T2), p) {} // todo(neta) remove
  
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTTerm2_Inter: public ASTTerm2_TT {
public:
  ASTTerm2_Inter(ASTTerm2 *T1, ASTTerm2 *T2, Pos p) :
    ASTTerm2_TT(aInter, T1, T2, p) {} //todo(neta) remove
  ASTTerm2_Inter(ASTTerm2Ptr T1, ASTTerm2Ptr T2, Pos p = dummyPos) :
    ASTTerm2_TT(aInter, std::move(T1), std::move(T2), p) {}
  
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTTerm2_Setminus: public ASTTerm2_TT {
public:
  ASTTerm2_Setminus(ASTTerm2 *T1, ASTTerm2 *T2, Pos p) :
    ASTTerm2_TT(aSetminus, T1, T2, p) {} //todo(neta) delete
  ASTTerm2_Setminus(ASTTerm2Ptr T1, ASTTerm2Ptr T2, Pos p = dummyPos) :
    ASTTerm2_TT(aSetminus, std::move(T1), std::move(T2), p) {}
  
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTTerm2_Set: public ASTTerm2 {
public:
  ASTTerm2_Set(SharedASTList elms, Pos p = dummyPos) :
    ASTTerm2(aSet, p), elements(std::move(elms)) {}
  ~ASTTerm2_Set() = default;

  void freeVars(IdentList*, IdentList*);
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  SharedASTList elements;
};

class ASTTerm2_Plus: public ASTTerm2_Tn {
public:
  ASTTerm2_Plus(ASTTerm2 *T, int n, Pos p) :
    ASTTerm2_Tn(aPlus2, T, n, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  VarCode unfold(int v1, int v2, int n, SubstCode *subst, Pos pos);
  void dump();
}; 

class ASTTerm2_Minus: public ASTTerm2_Tn {
public:
  ASTTerm2_Minus(ASTTerm2 *T, int n, Pos p) :
    ASTTerm2_Tn(aMinus2, T, n, p) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  VarCode unfold(int v1, int v2, int n, SubstCode *subst, Pos pos);
  void dump();
}; 

class ASTTerm2_Interval: public ASTTerm2 {
public:
  ASTTerm2_Interval(ASTTerm1 *tt1, ASTTerm1 *tt2, Pos p) :
    ASTTerm2(aInterval, p), t1(tt1), t2(tt2) {} 
  ~ASTTerm2_Interval() {delete t1; delete t2;}

  void freeVars(IdentList*, IdentList*);
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  ASTTerm1 *t1;
  ASTTerm1 *t2;
};

class ASTTerm2_PresbConst: public ASTTerm2 {
public:
  ASTTerm2_PresbConst(int v, Pos p) :
    ASTTerm2(aPresbConst, p), value(v) {}

  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  int value;
};

class ASTTerm2_Formula: public ASTTerm2 {
public:
  ASTTerm2_Formula(Ident id, ASTForm *ff, Pos p) :
    ASTTerm2(aTerm2Formula, p), fresh(id), f(ff) {}
  ~ASTTerm2_Formula() {delete f;}

  void freeVars(IdentList*, IdentList*);
  ASTTermCode *makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  Ident fresh;
  ASTForm *f;
};

////////// Syntactical categories of ASTForm //////////////////////////////////

class ASTForm_Var0: public ASTForm {
public:
  ASTForm_Var0(int nn, Pos p = dummyPos) :
    ASTForm(aVar0, p), n(nn) {}

  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  int n;
};

class ASTForm_True: public ASTForm {
public:
  ASTForm_True(Pos p = dummyPos) :
    ASTForm(aTrue, p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_False: public ASTForm {
public:
  ASTForm_False(Pos p = dummyPos) :
    ASTForm(aFalse, p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_In: public ASTForm_tT {
public:
  ASTForm_In(ASTTerm1 *t1, ASTTerm2 *T2, Pos p) : //todo(neta) delete
    ASTForm_tT(aIn, t1, T2, p) {}
  ASTForm_In(ASTTerm1Ptr t1, ASTTerm2Ptr T2, Pos p = dummyPos) :
    ASTForm_tT(aIn, std::move(t1), std::move(T2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Notin: public ASTForm_tT {
public:
  ASTForm_Notin(ASTTerm1 *t1, ASTTerm2 *T2, Pos p) :
    ASTForm_tT(aNotin, t1, T2, p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_RootPred: public ASTForm {
public:
  ASTForm_RootPred(ASTTerm1 *tt, IdentList *ull, Pos p) :
    ASTForm(aRootPred, p), ul(ull), t(tt) {}
  ~ASTForm_RootPred() {delete t; delete ul;}

  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  IdentList *ul;
  ASTTerm1  *t;
};

class ASTForm_EmptyPred: public ASTForm_T {
public:
  ASTForm_EmptyPred(ASTTerm2 *T, Pos p) : //todo(neta) delete
    ASTForm_T(aEmptyPred, T, p) {}
  ASTForm_EmptyPred(ASTTerm2Ptr T, Pos p = dummyPos) :
    ASTForm_T(aEmptyPred, std::move(T), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_FirstOrder: public ASTForm {
public:
  ASTForm_FirstOrder(ASTTerm1 *tt, Pos p) :
    ASTForm(aFirstOrder, p), t(tt) {}
  ~ASTForm_FirstOrder() {delete t;}

  VarCode makeCode(SubstCode *subst = NULL);
  void freeVars(IdentList*, IdentList*);
  void dump();

protected:
  ASTTerm1 *t;
};

class ASTForm_Sub: public ASTForm_TT {
public:
  ASTForm_Sub(ASTTerm2 *T1, ASTTerm2 *T2, Pos p) : //todo(neta) delete
    ASTForm_TT(aSub, T1, T2, p) {}
  ASTForm_Sub(ASTTerm2Ptr T1, ASTTerm2Ptr T2, Pos p = dummyPos) :
    ASTForm_TT(aSub, std::move(T1), std::move(T2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Equal1: public ASTForm_tt {
public:
  ASTForm_Equal1(ASTTerm1 *t1, ASTTerm1 *t2, Pos p) :
    ASTForm_tt(aEqual1, t1, t2, p) {} //todo(neta) delete
  ASTForm_Equal1(ASTTerm1Ptr t1, ASTTerm1Ptr t2, Pos p = dummyPos) :
    ASTForm_tt(aEqual1, std::move(t1), std::move(t2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Equal2: public ASTForm_TT {
public:
  ASTForm_Equal2(ASTTerm2 *T1, ASTTerm2 *T2, Pos p) :
    ASTForm_TT(aEqual2, T1, T2, p) {} //todo(neta) delete
  ASTForm_Equal2(ASTTerm2Ptr T1, ASTTerm2Ptr T2, Pos p = dummyPos) :
    ASTForm_TT(aEqual2, std::move(T1), std::move(T2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_NotEqual1: public ASTForm_tt {
public:
  ASTForm_NotEqual1(ASTTerm1 *t1, ASTTerm1 *t2, Pos p) :
    ASTForm_tt(aNotEqual1, t1, t2, p) {} //todo(neta) delete
  ASTForm_NotEqual1(ASTTerm1Ptr t1, ASTTerm1Ptr t2, Pos p = dummyPos) :
    ASTForm_tt(aNotEqual1, std::move(t1), std::move(t2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_NotEqual2: public ASTForm_TT {
public:
  ASTForm_NotEqual2(ASTTerm2 *T1, ASTTerm2 *T2, Pos p) :
    ASTForm_TT(aNotEqual2, T1, T2, p) {} // todo(neta) delete
  ASTForm_NotEqual2(ASTTerm2Ptr T1, ASTTerm2Ptr T2, Pos p = dummyPos) :
    ASTForm_TT(aNotEqual2, std::move(T1), std::move(T2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Less: public ASTForm_tt {
public:
  ASTForm_Less(ASTTerm1 *t1, ASTTerm1 *t2, Pos p) : //todo(neta) delete
    ASTForm_tt(aLess, t1, t2, p) {}
  ASTForm_Less(ASTTerm1Ptr t1, ASTTerm1Ptr t2, Pos p = dummyPos) :
    ASTForm_tt(aLess, std::move(t1), std::move(t2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_LessEq: public ASTForm_tt {
public:
  ASTForm_LessEq(ASTTerm1 *t1, ASTTerm1 *t2, Pos p) : //todo(neta) delete
    ASTForm_tt(aLessEq, t1, t2, p) {}
  ASTForm_LessEq(ASTTerm1Ptr t1, ASTTerm1Ptr t2, Pos p = dummyPos) :
    ASTForm_tt(aLessEq, std::move(t1), std::move(t2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_WellFormedTree: public ASTForm_T {
public:
  ASTForm_WellFormedTree(ASTTerm2 *t, Pos p) :
    ASTForm_T(aWellFormedTree, t, p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Impl: public ASTForm_ff {
public:
  ASTForm_Impl(ASTForm *f1, ASTForm *f2, Pos p) : //todo(neta) remove
    ASTForm_ff(aImpl, f1, f2, p) {}
  ASTForm_Impl(ASTFormPtr f1, ASTFormPtr f2, Pos p = dummyPos) :
    ASTForm_ff(aImpl, std::move(f1), std::move(f2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Biimpl: public ASTForm_ff {
public:
  ASTForm_Biimpl(ASTForm *f1, ASTForm *f2, Pos p) : //todo(neta) delete
    ASTForm_ff(aBiimpl, f1, f2, p) {}
  ASTForm_Biimpl(ASTFormPtr f1, ASTFormPtr f2, Pos p = dummyPos) :
    ASTForm_ff(aBiimpl, std::move(f1), std::move(f2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_And: public ASTForm_ff {
public:
  ASTForm_And(ASTForm *f1, ASTForm *f2, Pos p) : //todo(neta) delete
    ASTForm_ff(aAnd, f1, f2, p) {}
  ASTForm_And(ASTFormPtr f1, ASTFormPtr f2, Pos p = dummyPos) :
    ASTForm_ff(aAnd, std::move(f1), std::move(f2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_IdLeft: public ASTForm_ff {
public:
  ASTForm_IdLeft(ASTForm *f1, ASTForm *f2, Pos p) : //todo(neta) delete
    ASTForm_ff(aIdLeft, f1, f2, p) {}
  ASTForm_IdLeft(ASTFormPtr f1, ASTFormPtr f2, Pos p) :
    ASTForm_ff(aIdLeft, std::move(f1), std::move(f2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Or: public ASTForm_ff {
public:
  ASTForm_Or(ASTForm *f1, ASTForm *f2, Pos p) : //todo(neta) delete
    ASTForm_ff(aOr, f1, f2, p) {}
  ASTForm_Or(ASTFormPtr f1, ASTFormPtr f2, Pos p = dummyPos) :
    ASTForm_ff(aOr, std::move(f1), std::move(f2), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Not: public ASTForm {
public:
  ASTForm_Not(ASTForm *ff, Pos p) : //todo(neta) delete
    ASTForm_Not(ASTFormPtr(ff), p) {}
  ASTForm_Not(ASTFormPtr ff, Pos p = dummyPos) :
    ASTForm(aNot, p), f(std::move(ff)) {}
  ~ASTForm_Not() = default;

  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  ASTFormPtr f;
};

class ASTForm_Ex0: public ASTForm_vf {
public:
  ASTForm_Ex0(IdentList *vl, ASTFormPtr f, Pos p = dummyPos) :
    ASTForm_vf(aEx0, vl, std::move(f), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Ex1: public ASTForm_uvf {
public:
  ASTForm_Ex1(IdentList *ul, IdentList *vl, ASTForm *f, Pos p) :
    ASTForm_uvf(aEx1, ul, vl, f, p) {} // todo(neta) delete
  ASTForm_Ex1(IdentList *ul, IdentList *vl, ASTFormPtr f, Pos p = dummyPos) :
    ASTForm_uvf(aEx1, ul, vl, std::move(f), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Ex2: public ASTForm_uvf {
public:
  ASTForm_Ex2(IdentList *ul, IdentList *vl, ASTForm *f, Pos p) :
  ASTForm_uvf(aEx2, ul, vl, f, p) {} // todo(neta) delete
  ASTForm_Ex2(IdentList *ul, IdentList *vl, ASTFormPtr f, Pos p = dummyPos) :
    ASTForm_uvf(aEx1, ul, vl, std::move(f), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_All0: public ASTForm_vf {
public:
  ASTForm_All0(IdentList *vl, ASTForm *f, Pos p) : //todo(neta) delete
    ASTForm_vf(aAll0, vl, f, p) {}
  ASTForm_All0(IdentList *vl, ASTFormPtr f, Pos p = dummyPos) :
    ASTForm_vf(aAll0, vl, std::move(f), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_All1: public ASTForm_uvf {
public:
  ASTForm_All1(IdentList *ul, IdentList *vl, ASTForm *f, Pos p) :
    ASTForm_uvf(aAll1, ul, vl, f, p) {} // todo(neta) delete
  ASTForm_All1(IdentList *ul, IdentList *vl, ASTFormPtr f, Pos p = dummyPos) :
    ASTForm_uvf(aAll1, ul, vl, std::move(f), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_All2: public ASTForm_uvf {
public:
  ASTForm_All2(IdentList *ul, IdentList *vl, ASTForm *f, Pos p) :
  ASTForm_uvf(aAll2, ul, vl, f, p) {} // todo(neta) delete
  ASTForm_All2(IdentList *ul, IdentList *vl, ASTFormPtr f, Pos p = dummyPos) :
    ASTForm_uvf(aAll1, ul, vl, std::move(f), p) {}

  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Let0: public ASTForm {
public:
  ASTForm_Let0(IdentList *ids, FormList *fs, ASTForm *ff, Pos p) :
    ASTForm(aLet0, p), defIdents(ids), defForms(fs), f(ff) {}
  ~ASTForm_Let0() {delete f;}

  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  IdentList *defIdents;
  FormList *defForms;
  ASTForm *f;
};

class ASTForm_Let1: public ASTForm {
public:
  ASTForm_Let1(IdentList *ids, Term1List *ts, ASTForm *ff, Pos p) :
    ASTForm(aLet1, p), defIdents(ids), defTerms(ts), f(ff) {}
  ~ASTForm_Let1() {delete f;}

  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  IdentList *defIdents;
  Term1List *defTerms;
  ASTForm *f;
};

class ASTForm_Let2: public ASTForm {
public:
  ASTForm_Let2(IdentList *ids, Term2List *ts, ASTForm *ff, Pos p) :
    ASTForm(aLet2, p), defIdents(ids), defTerms(ts), f(ff) {}
  ~ASTForm_Let2() {delete f;}

  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  IdentList *defIdents;
  Term2List *defTerms;
  ASTForm *f;
};

class ASTForm_Call: public ASTForm {
public:
  ASTForm_Call(int nn, SharedASTList alist, Pos p = dummyPos) :
    ASTForm(aCall, p), args(std::move(alist)), n(nn) {}
  ~ASTForm_Call() = default;

  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();

protected:
  SharedASTList args;
  int n;

  VarCode fold(SharedASTList::iterator iter, IdentList &actuals,
	       SubstCode *subst = NULL);
};

class ASTForm_Import: public ASTForm {
public:
  ASTForm_Import(char* fil, Deque<char*> *fv, IdentList *ids, Pos p) :
    ASTForm(aImport, p), file(fil), fileVars(fv), idents(ids) {}
  ~ASTForm_Import() {delete idents;}
  
  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
  
protected:
  char *file;
  Deque<char*> *fileVars;
  IdentList *idents;
};

class ASTForm_Export: public ASTForm_f {
public:
  ASTForm_Export(ASTForm *ff, char* fil, Pos p) :
    ASTForm_f(aExport, ff, p), file(fil) {}
  
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
  
protected:
  char *file;
};
  
class ASTForm_Prefix: public ASTForm_f {
public:
  ASTForm_Prefix(ASTForm *ff, Pos p) :
    ASTForm_f(aPrefix, ff, p) {}
  
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_Restrict: public ASTForm_f {
public:
  ASTForm_Restrict(ASTForm *ff, Pos p) :
    ASTForm_f(aRestrict, ff, p) {}
  
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
};

class ASTForm_InStateSpace1: public ASTForm {
public:
  ASTForm_InStateSpace1(ASTTerm1 *tt, IdentList *s, Pos p) :
    ASTForm(aInStateSpace1, p), t(tt), ss(s) {}
  ~ASTForm_InStateSpace1() {delete t; delete ss;}
  
  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
  
protected:
  ASTTerm1 *t;
  IdentList *ss;
};
 
class ASTForm_InStateSpace2: public ASTForm {
public:
  ASTForm_InStateSpace2(ASTTerm2 *TT, IdentList *s, Pos p) :
    ASTForm(aInStateSpace2, p), T(TT), ss(s) {}
  ~ASTForm_InStateSpace2() {delete T; delete ss;}
  
  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
  
protected:
  ASTTerm2 *T;
  IdentList *ss;
};
 
class  ASTForm_SomeType: public ASTForm {
public:
  ASTForm_SomeType(ASTTerm *tt, Pos p) :
    ASTForm(aSomeType, p), t(tt) {}
  ~ASTForm_SomeType() {delete t;}
  
  void freeVars(IdentList*, IdentList*);
  VarCode makeCode(SubstCode *subst = NULL);
  void dump();
  
protected:
  ASTTerm *t;
};

////////// MonaAST ////////////////////////////////////////////////////////////

class MonaAST {
public:
  MonaAST(ASTForm *f, ASTForm *a) :
    MonaAST(ASTFormPtr(f), ASTFormPtr(a)) {}
  explicit MonaAST(ASTFormPtr f) :
    MonaAST(std::move(f), std::make_shared<ASTForm_True>(dummyPos)) {}
  MonaAST(ASTFormPtr f, ASTFormPtr a) :
    formula(std::move(f)), assertion(std::move(a)), lastPosVar(-1), allPosVar(-1) {}
  ~MonaAST() = default;
  
  ASTFormPtr formula;
  ASTFormPtr assertion;

  DequeGC<ASTForm *> verifyformlist;
  Deque<char *>      verifytitlelist;

  IdentList globals; // all globally declared variables
  Ident lastPosVar;  // lastpos variable
  Ident allPosVar;   // allpos variable
};

////////// Auxiliary functions ////////////////////////////////////////////////

VarCode getRestriction(Ident id, SubstCode *subst);

VarCode project(VarCode vc, ASTTermCode *t, Pos p);
VarCode projectList(VarCode vc, IdentList *projList, Pos p);

VarCode andList(VarCode vc1, VarCode vc2);
VarCode andList(VarCode vc1, VarCode vc2, VarCode vc3);
VarCode andList(VarCode vc1, VarCode vc2, VarCode vc3, VarCode vc4);

#endif
