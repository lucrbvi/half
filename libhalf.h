#ifdef __cplusplus
class Function {
public:
    int id;
    Function *body;

    Function(int id, Function *body = nullptr) : id(id), body(body) {}

    ~Function() { if (body) delete body; }

    Function* clone() {
        return new Function(id, body ? body->clone() : nullptr);
    }

    // β-reduction
    Function* reduce(Function* replacer) {
        if (!body) return clone(); // var -> no reduction
        return body->substitute(id, replacer);
    }

private:
    Function* substitute(int var_id, Function* replacement) {
        if (!body) {
            return (id == var_id) ? replacement->clone() : clone();
        }
        if (id == var_id) return clone();
        return new Function(id, body->substitute(var_id, replacement));
    }
};

extern "C" {
#else
typedef struct Function Function;
#endif

Function *createFunction(int id, Function *body);
void destroyFunction(Function *func);
Function* reduceFunction(Function* source, Function* replacer);
int getFunctionId(Function* func);

#ifdef __cplusplus
}
#endif
