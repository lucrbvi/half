#include "libhalf.h"

extern "C" {
    Function *createFunction(int id, Function *body) {
        return new Function(id, body);
    }

    void destroyFunction(Function *func) {
        delete func;
    }

    Function* reduceFunction(Function* source, Function* replacer) {
        return source->reduce(replacer);
    }

    int getFunctionId(Function* func) {
        return func->id;
    }

    Function* getFunctionBody(Function* func) {
        return func->body;
    }

    Function* cloneFunction(Function* func) {
        return func->clone();
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

    void runtimeRegisterBuiltin(Runtime* runtime, const char* name, BuiltinCallback callback, void* userdata) {
        runtime->registerBuiltin(std::string(name), callback, userdata);
    }

    int runtimeHasBuiltin(Runtime* runtime, const char* name) {
        return runtime->hasBuiltin(std::string(name)) ? 1 : 0;
    }

    Function* runtimeCallBuiltin(Runtime* runtime, const char* name, Function* arg) {
        return runtime->callBuiltin(std::string(name), arg);
    }

    Parser* createParser(const char* source, Runtime* runtime) {
        return new Parser(std::string(source), runtime);
    }

    void destroyParser(Parser* parser) {
        delete parser;
    }

    Function* parserParseExpression(Parser* parser) {
        return parser->parseExpression();
    }

    Program* parserParseProgram(Parser* parser) {
        return new Program(parser->parseProgram());
    }

    void destroyProgram(Program* program) {
        delete program;
    }

    int programSize(Program* program) {
        return program->size();
    }

    const char* programGetName(Program* program, int index) {
        return program->getName(index);
    }

    Function* programGetFunction(Program* program, int index) {
        return program->getFunction(index);
    }
}
