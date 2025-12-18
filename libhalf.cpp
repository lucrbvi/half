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
}
