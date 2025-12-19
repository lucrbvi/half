#include "libhalf.h"
#include <stdio.h>
#include <string.h>

int main() {
    Function *identity = createFunction(1, createFunction(1, NULL));
    Function *b = createFunction(2, NULL);
    Function *result = reduceFunction(identity, b);

    printf("Result: %d\n", getFunctionId(result));

    char source[] = "yo = \\x.x";
    Parser *parser = createParser(source, NULL);
    Program *program = parserParseProgram(parser);

    int size = programSize(program);
    for (int i = 0; i < size; i++) {
        Function *func = programGetFunction(program, i);
        printf("Name: %s, Function ID: %d\n", programGetName(program, i), getFunctionId(func));
    }

    destroyFunction(identity);
    destroyFunction(b);
    destroyFunction(result);

    return 0;
}
