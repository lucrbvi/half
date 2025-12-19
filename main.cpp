#include "libhalf.h"
#include <cstdio>
#include <memory>

Function* builtin_help(Function* arg) {
    printf("Available commands:\n");
    printf("  :help <function> - show this help message (you cannot call a function without an input sorry!)\n");
    printf("  :show <function> - show the function definition\n");
    printf("\n");
    printf("To leave Half interactive mode, press Ctrl+C\n");
    return NULL;
}

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
                printf(" -> TRUE\n");
            } else if (getFunctionId(innerBody) == getFunctionId(inner)) {
                printf(" -> FALSE\n");
            }
        }
    }
    return NULL;
}

void interactive(Runtime& runtime) {
    std::string history;
    std::string input;
    char buffer[256];

    while (true) {
        try {
            printf(">>> ");
            input.clear();

            while (std::fgets(buffer, sizeof(buffer), stdin)) {
                input += buffer;
                if (input.back() == '\n') {
                    break;
                }
            }

            history += input;
            runtime.run(input);
        } catch (const std::exception& e) {
            printf("Error: %s\n", e.what());
        }
    }
}

int main(int argc, char* argv[]) {
    std::unique_ptr<Runtime> runtime(new Runtime());
    runtime->registerBuiltin("show", builtin_show);
    runtime->registerBuiltin("help", builtin_help);

    if (argc < 2) {
        printf("Half 0.1 - type ':help \\x.x' to get started\n");
        interactive(*runtime);
        return 0;
    }

    return 0;
}
