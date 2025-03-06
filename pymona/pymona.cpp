#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/set.h>

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

struct ElementRef {
    Identifiers identifiers;
    ASTTerm1Ptr term;
};

struct SetRef {
    Identifiers identifiers;
    ASTTerm2Ptr term;
};

BoolRef create_bool(const std::string &name) {
    Ident ident = addVar(name, Varname0);
    return BoolRef{
        std::set{ident},
        std::make_shared<ASTForm_Var0>(ident, dummyPos)
    };
}

ElementRef create_element(const std::string &name) {
    Ident ident = addVar(name, Varname1);
    return ElementRef{
        std::set{ident},
        std::make_shared<ASTTerm1_Var1>(ident, dummyPos)
    };
}

ElementRef create_int(int i) {
    return ElementRef{
        Identifiers{},
        std::make_shared<ASTTerm1_Int>(i, dummyPos)
    };
}

SetRef create_set(const std::string &name) {
    Ident ident = addVar(name, Varname2);
    return SetRef{
        std::set{ident},
        std::make_shared<ASTTerm2_Var2>(ident, dummyPos)
    };
}

BoolRef create_less_than(const ElementRef &i1, const ElementRef &i2) {
    return BoolRef{
        set_union(i1.identifiers, i2.identifiers),
        std::make_shared<ASTForm_Less>(i1.term, i2.term, dummyPos)
    };
}

BoolRef create_in(const ElementRef &e, const SetRef &s) {
    return BoolRef{
        set_union(e.identifiers, s.identifiers),
        std::make_shared<ASTForm_In>(e.term, s.term, dummyPos)
    };
}

BoolRef create_true() {
    return BoolRef{
        std::set<Ident>(),
        std::make_shared<ASTForm_True>(dummyPos)
    };
}

BoolRef create_false() {
    return BoolRef{
        std::set<Ident>(),
        std::make_shared<ASTForm_False>(dummyPos)
    };
}

BoolRef create_and(nb::args args) {
    Identifiers identifiers;
    ASTFormPtr result = std::make_shared<ASTForm_True>(dummyPos);
    for (auto arg: args) {
        if (nb::isinstance<BoolRef>(arg)) {
            BoolRef f = nb::cast<BoolRef>(arg);
            identifiers.insert(f.identifiers.begin(), f.identifiers.end());
            result = std::make_shared<ASTForm_And>(
                std::move(result), f.form, dummyPos
            );
        } else {
            throw nb::value_error("Invalid argument type, expected a BoolRef");
        }
    }
    return BoolRef{identifiers, std::move(result)};
}

BoolRef create_or(nb::args args) {
    Identifiers identifiers;
    ASTFormPtr result = std::make_shared<ASTForm_False>(dummyPos);
    for (auto arg: args) {
        if (nb::isinstance<BoolRef>(arg)) {
            BoolRef f = nb::cast<BoolRef>(arg);
            identifiers.insert(f.identifiers.begin(), f.identifiers.end());
            result = std::make_shared<ASTForm_Or>(
                std::move(result), f.form, dummyPos
            );
        } else {
            throw nb::value_error("Invalid argument type, expected a BoolRef");
        }
    }
    return BoolRef{identifiers, std::move(result)};
}

BoolRef create_implies(const BoolRef &f1, const BoolRef &f2) {
    return BoolRef{
        set_union(f1.identifiers, f2.identifiers),
        std::make_shared<ASTForm_Impl>(f1.form, f2.form, dummyPos)
    };
}

BoolRef create_not(const BoolRef &f) {
    return BoolRef{
        f.identifiers,
        std::make_shared<ASTForm_Not>(f.form, dummyPos)
    };
}

Model solve(const BoolRef &formula) {
    MonaAST ast{formula.form};
    for (const auto ident: formula.identifiers) {
        ast.globals.insert(ident);
    }
    return getModel(ast);
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
    nb::class_<ElementRef>(m, "ElementRef");
    nb::class_<SetRef>(m, "SetRef");

    m.def("create_element", &create_element);
    m.def("create_int", &create_int);
    m.def("create_less_than", &create_less_than);

    m.def("create_set", &create_set);
    m.def("create_in", &create_in);

    m.def("create_bool", &create_bool);
    m.def("create_true", &create_true);
    m.def("create_false", &create_false);
    m.def("create_and", &create_and);
    m.def("create_or", &create_or);
    m.def("create_implies", &create_implies);
    m.def("create_not", &create_not);
    m.def("solve", &solve);
}
