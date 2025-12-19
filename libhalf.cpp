#include "libhalf.h"
#include <cstring>

extern "C" {
    Function *createFunction(int id, Function *body) {
        return new Function(id, body);
    }

    Function *createApplication(Function *func, Function *arg) {
        return new Function(func, arg);
    }

    void destroyFunction(Function *func) {
        delete func;
    }

    Function* reduceFunction(Function* source, Function* replacer) {
        return source->reduce(replacer);
    }

    Function* evalFunction(Function* func) {
        return func->eval();
    }

    int getFunctionId(Function* func) {
        return func->id;
    }

    Function* getFunctionBody(Function* func) {
        return func->body;
    }

    Function* getFunctionArg(Function* func) {
        return func->arg;
    }

    Function* cloneFunction(Function* func) {
        return func->clone();
    }

    int functionIsVar(Function* func) {
        return func->isVar() ? 1 : 0;
    }

    int functionIsLambda(Function* func) {
        return func->isLambda() ? 1 : 0;
    }

    int functionIsApp(Function* func) {
        return func->isApp() ? 1 : 0;
    }

    Runtime *createRuntime() {
        return new Runtime();
    }

    void destroyRuntime(Runtime* runtime) {
        delete runtime;
    }

    void runtimeSet(Runtime* runtime, Function* fn) {
        runtime->set(fn);
    }

    Function* runtimeGet(Runtime* runtime, int index) {
        return runtime->get(static_cast<size_t>(index));
    }

    int runtimeSize(Runtime* runtime) {
        return runtime->size();
    }

    void runtimeRegisterBuiltin(Runtime* runtime, const char* name, Fn callback) {
        runtime->registerBuiltin(name, callback);
    }

    int runtimeHasBuiltin(Runtime* runtime, const char* name) {
        return runtime->isBuiltin(name) ? 1 : 0;
    }

    Function* runtimeCallBuiltin(Runtime* runtime, const char* name, Function* arg) {
        return runtime->callBuiltin(name, arg);
    }

    void runtimeRun(Runtime* runtime, const char* source) {
        Parser p(std::string(source), runtime);

        while (p.hasMore()) {
            auto [name, func] = p.parseNextStatement();

            if (runtime->isBuiltin(name)) {
                Function* evaluated = func->eval();
                runtime->callBuiltin(name, evaluated);
                delete evaluated;
            } else {
                Function* evaluated = func->eval();
                runtime->bind(name, evaluated);
            }
            delete func;
        }
    }

    Parser* createParser(const char* source, Runtime* runtime) {
        return new Parser(std::string(source), runtime);
    }

    void destroyParser(Parser* parser) {
        delete parser;
    }

    const char* functionToString(Function* func) {
        std::string str = func->toString();
        char* result = new char[str.length() + 1];
        strcpy(result, str.c_str());
        return result;
    }

    void freeString(const char* str) {
        delete[] (char*)str;
    }
}
