#include <memory>
#include <cstdio>
#include <string>

int main() {
    std::string message("Hello, world!");
    message[0] = 'Y';
    printf("%s\n", message.c_str());
    return 0;
}