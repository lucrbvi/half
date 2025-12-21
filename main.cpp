#include "libhalf.h"
#include <cstdio>
#include <memory>
#include <unistd.h>
#include <string.h>

Function* builtin_help(Function* arg) {
    printf("Available commands:\n");
    printf("  :help <function> - show this help message (you cannot call a function without an input sorry!)\n");
    printf("  :show <function> - show the function definition\n");
    printf("  :read <function> - return a Church boolean based on the call arguments or stdin if there is no arguments.\n");
    printf("  :ast <function> - (DEBUG ONLY) show the AST of a function\n");
    printf("\n");
    printf("You can activate debug mode by calling Half with the flag '--debug'\n");
    printf("\n");
    printf("To leave Half interactive mode, press Ctrl+D\n");
    return arg->clone();
}

static int bitBuffer = 0;
static int bitCount = 0;
static char* globalArgv = nullptr;

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
                bitBuffer = (bitBuffer << 1) | bit;
                bitCount++;
                if (bitCount == 8) {
                    putchar(bitBuffer);
                    fflush(stdout);
                    bitBuffer = 0;
                    bitCount = 0;
                }
            }
        }
    }
    return arg->clone();
}

Function* builtin_read(Function* arg) {
    int bit = -1;

    if (bitCount == 0) {
        int c = getchar();
        if (c != EOF) {
            bitBuffer = c;
            bitCount = 8;
        }
    }

    if (bitCount > 0) {
        bitCount--;
        bit = (bitBuffer >> bitCount) & 1;
    }

    if (bit < 0) {
        return arg->clone();
    }

    if (bit == 1) {
        return new Function(0, new Function(1, new Function(0)));
    } else {
        return new Function(0, new Function(1, new Function(1)));
    }
}

void printASTRecursive(Function* func, int indent) {
    auto printIndent = [](int n) {
        for (int i = 0; i < n; i++) printf("  ");
    };

    if (!func) {
        printIndent(indent);
        printf("(null)\n");
        return;
    }

    if (func->isVar()) {
        printIndent(indent);
        printf("VAR %d\n", func->id);
    }
    else if (func->isLambda()) {
        printIndent(indent);
        printf("LAMBDA %d\n", func->id);
        printIndent(indent);
        printf("└─ body:\n");
        printASTRecursive(func->body, indent + 2);
    }
    else if (func->isApp()) {
        printIndent(indent);
        printf("APP\n");
        printIndent(indent);
        printf("├─ func:\n");
        printASTRecursive(func->body, indent + 2);
        printIndent(indent);
        printf("└─ arg:\n");
        printASTRecursive(func->arg, indent + 2);
    }
}

Function* builtin_ast(Function* arg) {
    printASTRecursive(arg, 0);
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
    globalArgv = *argv;

    std::unique_ptr<Runtime> runtime(new Runtime());
    runtime->registerBuiltin("show", builtin_show);
    runtime->registerBuiltin("read", builtin_read);

    if (argc > 1 && strcmp(argv[1], "--debug") == 0) {
        runtime->registerBuiltin("ast", builtin_ast);
        if (argc > 2) {
            argv[1] = argv[2];
            argv[2] = nullptr;
        }
        argc -= 1;
    }

    if (argc < 2) {
        runtime->registerBuiltin("help", builtin_help);
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
