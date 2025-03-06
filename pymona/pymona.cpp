#include <string>
#include <utility>
#include <variant>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/optional.h>

#include "symboltable.h"
#include "printline.h"
#include "dfa.h"
#include "env.h"
#include "lib.h"
#include "offsets.h"
#include "predlib.h"
#include "untyped.h"

extern "C" {
#include "mem.h"
}

#include "ident.h"

#include "model.h"
#include "utils.h"

Options options;
MonaUntypedAST *untypedAST;
SymbolTable symbolTable(1019);
PredicateLib predicateLib;
Offsets offsets;
CodeTable *codeTable;
Guide guide;
AutLib lib;
int numTypes = 0;
bool regenerate = false;

namespace nb = nanobind;
using namespace nb::literals;

int add(int a, int b = 1) {
    return a + b;
}

struct Foo {
    std::string name;

    explicit Foo(std::string name) : name(std::move(name)) {
    }

    std::string greet() const {
        return "Hello, I am " + name;
    }
};

using Identifiers = std::set<Ident>;

Identifiers set_union(const Identifiers &i1, const Identifiers &i2) {
    Identifiers result(i1);
    result.insert(i2.begin(), i2.end());
    return std::move(result);
}

struct BoolRef {
    Identifiers identifiers;
    ASTFormPtr form;
};

struct BoolIdent : BoolRef {
    BoolIdent(const std::string &name)
        : BoolIdent(addVar(name, Varname0)) {
    }

    BoolIdent(Ident identt)
        : BoolRef{
              Identifiers{identt},
              std::make_shared<ASTForm_Var0>(identt)
          }, ident(identt) {
    }

    Ident ident;
};

struct ElementRef {
    ElementRef(int i)
        : term(std::make_shared<ASTTerm1_Int>(i)) {
    }

    ElementRef(Identifiers identifiers, ASTTerm1Ptr term)
        : identifiers(std::move(identifiers)),
          term(std::move(term)) {
    }

    Identifiers identifiers;
    ASTTerm1Ptr term;
};

struct ElementIdent : ElementRef {
    ElementIdent(const std::string &name)
        : ElementIdent(addVar(name, Varname1)) {
    }

    ElementIdent(Ident identt)
        : ElementRef(
              Identifiers{identt},
              std::make_shared<ASTTerm1_Var1>(identt)
          ), ident(identt) {
    }

    Ident ident;
};

struct SetRef {
    Identifiers identifiers;
    ASTTerm2Ptr term;
};

struct SetIdent : SetRef {
    SetIdent(const std::string &name)
        : SetIdent(addVar(name, Varname2)) {
    }

    SetIdent(Ident identt)
        : SetRef{
              Identifiers{identt},
              std::make_shared<ASTTerm2_Var2>(identt)
          }, ident(identt) {
    }

    Ident ident;
};

ElementRef makeInt(int i) {
    return i;
}

BoolRef makeLessThan(const ElementRef &i1, const ElementRef &i2) {
    return BoolRef{
        set_union(i1.identifiers, i2.identifiers),
        std::make_shared<ASTForm_Less>(i1.term, i2.term, dummyPos)
    };
}

BoolRef makeIn(const ElementRef &e, const SetRef &s) {
    return BoolRef{
        set_union(e.identifiers, s.identifiers),
        std::make_shared<ASTForm_In>(e.term, s.term, dummyPos)
    };
}

BoolRef makeTrue() {
    return BoolRef{
        std::set<Ident>(),
        std::make_shared<ASTForm_True>(dummyPos)
    };
}

BoolRef makeFalse() {
    return BoolRef{
        std::set<Ident>(),
        std::make_shared<ASTForm_False>(dummyPos)
    };
}

BoolRef makeAnd(nb::args args) {
    Identifiers identifiers;
    ASTFormPtr result = std::make_shared<ASTForm_True>(dummyPos);
    for (auto arg: args) {
        BoolRef f = nb::cast<BoolRef>(arg);
        identifiers.insert(f.identifiers.begin(), f.identifiers.end());
        result = std::make_shared<ASTForm_And>(
            std::move(result), f.form, dummyPos
        );
    }
    return BoolRef{identifiers, std::move(result)};
}

BoolRef makeOr(nb::args args) {
    Identifiers identifiers;
    ASTFormPtr result = std::make_shared<ASTForm_False>(dummyPos);
    for (auto arg: args) {
        BoolRef f = nb::cast<BoolRef>(arg);
        identifiers.insert(f.identifiers.begin(), f.identifiers.end());
        result = std::make_shared<ASTForm_Or>(
            std::move(result), f.form, dummyPos
        );
    }
    return BoolRef{identifiers, std::move(result)};
}

BoolRef makeImplies(const BoolRef &f1, const BoolRef &f2) {
    return BoolRef{
        set_union(f1.identifiers, f2.identifiers),
        std::make_shared<ASTForm_Impl>(f1.form, f2.form, dummyPos)
    };
}

BoolRef makeNot(const BoolRef &f) {
    return BoolRef{
        f.identifiers,
        std::make_shared<ASTForm_Not>(f.form, dummyPos)
    };
}

BoolRef makeForall1(const ElementIdent &id, const BoolRef &f) {
    IdentList *list = new IdentList(id.ident);
    return BoolRef{
        set_union(id.identifiers, f.identifiers),
        std::make_shared<ASTForm_All1>(nullptr, list, f.form)
    };
}

BoolRef makeForall1(nb::iterable ids, const BoolRef &f) {
    IdentList *list = new IdentList;
    Identifiers identifiers;
    for (auto id : nb::iter(ids)) {
        Ident ident = nb::cast<ElementIdent>(id).ident;
        identifiers.insert(ident);
        list->insert(ident);
    }
    identifiers.insert(f.identifiers.begin(), f.identifiers.end());
    return BoolRef{
        identifiers,
        std::make_shared<ASTForm_All1>(nullptr, list, f.form)
    };
}

BoolRef makeExists1(const ElementIdent &id, const BoolRef &f) {
    IdentList *list = new IdentList(id.ident);
    return BoolRef{
        set_union(id.identifiers, f.identifiers),
        std::make_shared<ASTForm_Ex1>(nullptr, list, f.form)
    };
}

BoolRef makeExists1(nb::iterable ids, const BoolRef &f) {
    IdentList *list = new IdentList;
    Identifiers identifiers;
    for (auto id : nb::iter(ids)) {
        Ident ident = nb::cast<ElementIdent>(id).ident;
        identifiers.insert(ident);
        list->insert(ident);
    }
    identifiers.insert(f.identifiers.begin(), f.identifiers.end());
    return BoolRef{
        identifiers,
        std::make_shared<ASTForm_Ex1>(nullptr, list, f.form)
    };
}

std::optional<Model> solve(const BoolRef &formula) {
    MonaAST ast{formula.form};
    for (const auto ident: formula.identifiers) {
        ast.globals.insert(ident);
    }
    return getModel(ast);
}

std::string lookupSymbol(Ident ident) {
    return symbolTable.lookupSymbol(ident);
}


NB_MODULE(pymona, m) {
    m.doc() = "Python bindings for the WS1S/WS2S solver MONA";
    m.def("add", &add, "a"_a, "b"_a = 1,
          "This function adds two numbers and increments if only one is provided.");

    nb::class_<Foo>(m, "Foo")
            .def(nb::init<std::string>())
            .def("greet", &Foo::greet)
            .def_rw("name", &Foo::name)
            .def("__repr__",
                 [](const Foo &p) { return "<pymona.Foo named '" + p.name + "'>"; });

    nb::class_<BoolRef>(m, "BoolRef");
    nb::class_<BoolIdent, BoolRef>(m, "BoolIdent")
            .def(nb::init<std::string>())
            .def("__str__",
                 [](const BoolIdent &ident) { return lookupSymbol(ident.ident); });

    nb::class_<ElementRef>(m, "ElementRef")
            .def(nb::init_implicit<int>());
    nb::class_<ElementIdent, ElementRef>(m, "ElementIdent")
            .def(nb::init<std::string>())
            .def("__str__",
                 [](const ElementIdent &ident) { return lookupSymbol(ident.ident); });

    nb::class_<SetRef>(m, "SetRef");
    nb::class_<SetIdent, SetRef>(m, "SetIdent")
            .def(nb::init<std::string>())
            .def("__str__",
                 [](const SetIdent &ident) { return lookupSymbol(ident.ident); });

    m.def("int_", &makeInt);
    m.def("less_than", &makeLessThan);

    m.def("in_", &makeIn);

    m.def("true", &makeTrue);
    m.def("false", &makeFalse);
    m.def("and_", &makeAnd);
    m.def("or_", &makeOr);
    m.def("implies", &makeImplies);
    m.def("not_", &makeNot);

    m.def("forall1",
        nb::overload_cast<const ElementIdent &, const BoolRef &>(&makeForall1));
    m.def("forall1",
        nb::overload_cast<nb::iterable, const BoolRef &>(&makeForall1));
    m.def("exists1",
        nb::overload_cast<const ElementIdent &, const BoolRef &>(&makeExists1));
    m.def("exists1",
        nb::overload_cast<nb::iterable, const BoolRef &>(&makeExists1));

    m.def("solve", &solve);
}
