#include <string>
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

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
}
