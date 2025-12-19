#include "libhalf.h"
#include <stdio.h>
#include <string.h>

Function* builtin_show(Function* arg) {
    const char* str = functionToString(arg);
    printf("[SHOW] %s\n", str);
    freeString(str);

    if (functionIsLambda(arg) && getFunctionBody(arg) &&
        functionIsLambda(getFunctionBody(arg))) {

        Function* inner = getFunctionBody(arg);
        Function* innerBody = getFunctionBody(inner);

        if (innerBody && functionIsVar(innerBody)) {
            if (getFunctionId(innerBody) == getFunctionId(arg)) {
                printf("  -> TRUE\n");
            } else if (getFunctionId(innerBody) == getFunctionId(inner)) {
                printf("  -> FALSE\n");
            }
        }
    }
    return NULL;
}

int main() {
    printf("== Test 1: manual beta-reduction ==\n");
    Function* identity = createFunction(0, createFunction(0, NULL));
    Function* arg = createFunction(42, NULL); // variable
    Function* app = createApplication(identity, arg);

    Function* result = evalFunction(app);
    if (getFunctionId(result) == 42) {
        printf(" -> TEST 1 OK\n");
    } else {
        printf(" -> TEST 1 FAIL: expected 42, got %d\n", getFunctionId(result));
    }

    destroyFunction(app);
    destroyFunction(result);

    printf("== Test 2: Parser, runtimeRun() & builtin_show ==\n");

    Runtime* runtime = createRuntime();
    runtimeRegisterBuiltin(runtime, "show", builtin_show);

    runtimeRun(runtime, "id = \\x.x\ntrue = \\x.\\y.x\nfalse = \\x.\\y.y\n:show true\n:show false\n");
    printf("(test 2 passed if you saw things with '[SHOW]' before this line)\n");

    printf("\n== Test 3: Call a variable ==\n");
    runtimeRun(runtime, ":show id");

    destroyRuntime(runtime);
     printf("(test 3 passed if you saw things with '[SHOW]' before this line)\n");

    return 0;
}
