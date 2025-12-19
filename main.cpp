#include "libhalf.h"
#include <cstdio>
#include <memory>

int main() {
    std::unique_ptr<Function> identity(new Function(1, new Function(1)));
    std::unique_ptr<Function> b(new Function(2));
    std::unique_ptr<Function> result(identity->reduce(b.get()));

    printf("Result: %d\n", result->id);

    std::string source = "yo = \\x.x";
    std::unique_ptr<Parser> parser(new Parser(source));
    auto program = parser->parseProgram();

    for (const auto& stmt : program) {
        printf("Name: %s, Function ID: %d\n", stmt.first.c_str(), stmt.second->id);
    }

    return 0;
}
