#include "libhalf.h"
#include <cstdio>
#include <memory>

int main() {
    // (λx.x) b  →  b
    std::unique_ptr<Function> identity(new Function(1, new Function(1)));  // λx.x
    std::unique_ptr<Function> b(new Function(2));  // b var
    std::unique_ptr<Function> result(identity->reduce(b.get()));

    printf("Result: %d\n", result->id);  // 2

    return 0;
}
