#include "libhalf.h"
#include <cstdio>
#include <memory>
#include <unistd.h>

Function* builtin_help(Function* arg) {
    printf("Available commands:\n");
    printf("  :help <function> - show this help message (you cannot call a function without an input sorry!)\n");
    printf("  :show <function> - show the function definition\n");
    printf("\n");
    printf("To leave Half interactive mode, press Ctrl+D\n");
    return arg->clone();
}

static int bitBuffer = 0;
static int bitCount = 0;

Function* builtin_show(Function* arg) {
    if (functionIsLambda(arg) && getFunctionBody(arg) &&
        functionIsLambda(getFunctionBody(arg))) {
        Function* inner = getFunctionBody(arg);
        Function* innerBody = getFunctionBody(inner);
        if (innerBody && functionIsVar(innerBody)) {
            int bit = -1;
            if (getFunctionId(innerBody) == getFunctionId(arg)) {
                bit = 1;
            } else if (getFunctionId(innerBody) == getFunctionId(inner)) {
                bit = 0;
            }

            if (bit >= 0) {
                if (isatty(fileno(stdout))) {
                    printf("%d\n", bit); // interactive mode
                } else {
                    bitBuffer = (bitBuffer << 1) | bit;
                    bitCount++;
                    if (bitCount == 8) {
                        putchar(bitBuffer);
                        bitBuffer = 0;
                        bitCount = 0;
                    }
                }
                fflush(stdout);
                return arg->clone();
            }
        }
    }
    return arg->clone();
}

void interactive(Runtime& runtime) {
    std::string history;
    std::string input;
    char buffer[256];
    while (true) {
        try {
            printf(">>> ");
            fflush(stdout);
            input.clear();

            if (std::fgets(buffer, sizeof(buffer), stdin) == nullptr) {
                printf("\n");
                printf("Bye!\n");
                break;
            }

            input += buffer;
            while (input.back() != '\n' && std::fgets(buffer, sizeof(buffer), stdin)) {
                input += buffer;
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
        printf("Half Interactive Mode - type ':help \\x.x' to get started\n");
        interactive(*runtime);
        return 0;
    }

    std::FILE* f = std::fopen(argv[1], "r");
    if (!f) {
        printf("Error: Failed to open file '%s'\n", argv[1]);
        return 1;
    }

    std::string content;
    char buffer[256];
    while (std::fgets(buffer, sizeof(buffer), f)) {
        content += buffer;
    }
    fclose(f);

    runtime->run(content);
    return 0;
}
