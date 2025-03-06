#include <memory>
#include <cstdio>

int main() {
    char *c = new char('c');
    std::shared_ptr<char> ptr(c);

    delete c; // double-free
    return 0;
}