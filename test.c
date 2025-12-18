#include "libhalf.h"
#include <stdio.h>

int main() {
    Function *identity = createFunction(1, createFunction(1, NULL));
    Function *b = createFunction(2, NULL);
    Function *result = reduceFunction(identity, b);
    printf("Result: %d\n", getFunctionId(result));

    destroyFunction(identity);
    destroyFunction(b);
    destroyFunction(result);

    return 0;
}
