#ifdef __cplusplus

#include <vector>
#include <string>
#include <cctype>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <functional>

#define APP_ID -1

class Runtime;

class Function {
private:
    Function* substitute(int var_id, Function* replacement) {
        if (id == APP_ID) {
            return new Function(
                body->substitute(var_id, replacement),
                arg->substitute(var_id, replacement)
            );
        }
        if (!body) {
            return (id == var_id) ? replacement->clone() : clone();
        }
        if (id == var_id) return clone();
        return new Function(id, body->substitute(var_id, replacement));
    }

public:
    int id;
    Function* body;
    Function* arg;

    Function(int id, Function* body = nullptr)
        : id(id), body(body), arg(nullptr) {}

    Function(Function* func, Function* argument)
        : id(APP_ID), body(func), arg(argument) {}

    ~Function() {
        if (body) delete body;
        if (arg) delete arg;
    }

    bool isVar() const { return id != APP_ID && body == nullptr; }
    bool isLambda() const { return id != APP_ID && body != nullptr; }
    bool isApp() const { return id == APP_ID; }

    Function* clone() {
        if (isApp()) {
            return new Function(body->clone(), arg->clone());
        }
        return new Function(id, body ? body->clone() : nullptr);
    }

    Function* reduce(Function* replacer) {
        if (!isLambda()) return clone();
        return body->substitute(id, replacer);
    }

    Function* eval(Runtime* runtime);

    std::string toString() const {
        if (isVar()) {
            return std::to_string(id);
        }
        if (isLambda()) {
            return "(\\" + std::to_string(id) + "." + body->toString() + ")";
        }
        if (isApp()) {
            return "(" + body->toString() + " " + arg->toString() + ")";
        }
        return "?";
    }
};

typedef Function* (*Fn)(Function*);

class Runtime {
private:
    std::vector<std::unique_ptr<Function>> vars;
    std::unordered_map<std::string, Fn> builtins;
    std::unordered_map<std::string, Function*> bindings;
    std::unordered_map<int, std::string> builtinIds;

public:
    Runtime() = default;

    void set(Function* fn) {
        vars.push_back(std::unique_ptr<Function>(fn));
    }

    Function* get(size_t index) {
        if (index >= vars.size()) return nullptr;
        return vars[index].get();
    }

    int size() const {
        return vars.size();
    }

    void registerBuiltin(const char* name, Fn callback) {
        builtins[name] = callback;
    }

    bool isBuiltin(const std::string& name) const {
        return builtins.find(name) != builtins.end();
    }

    Function* callBuiltin(const std::string& name, Function* arg) {
        auto it = builtins.find(name);
        if (it == builtins.end()) return nullptr;
        return it->second(arg);
    }

    void bind(const std::string& name, Function* fn) {
        bindings[name] = fn;
    }

    Function* lookup(const std::string& name) {
        auto it = bindings.find(name);
        if (it == bindings.end()) return nullptr;
        return it->second;
    }

    void registerBuiltinId(int id, const std::string& name) {
        builtinIds[id] = name;
    }

    std::string getBuiltinName(int id) const {
        auto it = builtinIds.find(id);
        if (it != builtinIds.end()) {
            return it->second;
        }
        return "";
    }

    void interpret(std::vector<std::pair<std::string, Function*>>& program) {
        for (auto& [name, func] : program) {
            if (isBuiltin(name)) {
                Function* evaluated = func->eval(this);
                callBuiltin(name, evaluated);
                delete evaluated;
            } else {
                Function* evaluated = func->eval(this);
                bind(name, evaluated);
            }
        }
    }

    void run(const std::string& source);
};

inline Function* Function::eval(Runtime* runtime) {
    if (isVar()) {
        std::string builtinName = runtime->getBuiltinName(id);
        if (!builtinName.empty()) {
            return clone();
        }
        return clone();
    }

    if (isLambda()) {
        return clone();
    }

    if (isApp()) {
        Function* evalFunc = body->eval(runtime);
        Function* evalArg = arg->eval(runtime);

        std::string builtinName = runtime->getBuiltinName(evalFunc->id);
        if (!builtinName.empty() && evalFunc->isVar()) {
            Function* result = runtime->callBuiltin(builtinName, evalArg);
            delete evalFunc;
            delete evalArg;
            if (result) {
                return result;
            }
            return new Function(evalFunc->id);
        }

        if (evalFunc->isLambda()) {
            Function* reduced = evalFunc->reduce(evalArg);
            delete evalFunc;
            delete evalArg;
            Function* result = reduced->eval(runtime);
            delete reduced;
            return result;
        }

        return new Function(evalFunc, evalArg);
    }

    return clone();
}

class Parser {
private:
    std::string source;
    size_t pos;
    std::unordered_map<std::string, int> symbols;
    int nextId;
    int nextBuiltinId;

    void skipSpaces() {
        while (pos < source.length()) {
            if (std::isspace(source[pos])) {
                pos++;
                continue;
            }

            if (source[pos] == '#') {
                while (pos < source.length() && source[pos] != '\n') {
                    pos++;
                }
                if (pos < source.length() && source[pos] == '\n') {
                    pos++;
                }
                continue;
            }

            break;
        }
    }

    char peek() {
        skipSpaces();
        if (pos >= source.length()) return '\0';
        return source[pos];
    }

    char consume() {
        skipSpaces();
        if (pos >= source.length()) return '\0';
        return source[pos++];
    }

    bool match(char c) {
        if (peek() == c) {
            consume();
            return true;
        }
        return false;
    }

    std::string parseIdentifier() {
        skipSpaces();
        std::string id;

        while (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '_')) {
            id += source[pos++];
        }

        if (id.empty()) throw std::runtime_error("Expected identifier at position " + std::to_string(pos));
        return id;
    }

    bool isAtomStart() {
        char c = peek();
        if (c == '\0' || c == '=' || c == ')') {
            return false;
        }
        return c == '\\' || c == '(' || c == ':' || std::isalnum(c) || c == '_';
    }

    bool looksLikeNewStatement(size_t startPos) {
        size_t savedPos = pos;
        pos = startPos;  // Start from the position BEFORE isAtomStart() was called

        // Check if there's a newline between start position and next token
        bool crossedNewline = false;
        while (pos < source.length() && std::isspace(source[pos])) {
            if (source[pos] == '\n') {
                crossedNewline = true;
            }
            pos++;
        }

        // Skip comments after potential newline
        while (pos < source.length() && source[pos] == '#') {
            while (pos < source.length() && source[pos] != '\n') pos++;
            if (pos < source.length() && source[pos] == '\n') {
                crossedNewline = true;
                pos++;
            }
            // Skip any more whitespace after comment
            while (pos < source.length() && std::isspace(source[pos])) {
                if (source[pos] == '\n') crossedNewline = true;
                pos++;
            }
        }

        if (pos < source.length() && source[pos] == ':') {
            pos = savedPos;
            return true;
        }

        if (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '_')) {
            std::string id;
            while (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '_')) {
                id += source[pos++];
            }
            skipSpaces();
            bool isAssignment = (pos < source.length() && source[pos] == '=');
            pos = savedPos;

            // It's a new statement if it's an assignment OR
            // if we crossed a newline and this identifier is not a lambda parameter
            if (isAssignment) {
                return true;
            }
            if (crossedNewline && symbols.find(id) == symbols.end()) {
                return true;
            }
            return false;
        }

        pos = savedPos;
        return false;
    }

public:
    Runtime* runtime;

    Parser(const std::string& source, Runtime* rt = nullptr)
        : source(source), pos(0), nextId(0), nextBuiltinId(-2), runtime(rt ? rt : new Runtime()) {}

    Function* parseAtom() {
        if (peek() == '\\') {
            return parseLambda();
        }
        if (peek() == '(') {
            consume();
            auto expr = parseApplication();
            if (!match(')')) {
                throw std::runtime_error("Expected ')'");
            }
            return expr;
        }

        if (peek() == ':') {
            consume();
            std::string name = parseIdentifier();

            if (!runtime->isBuiltin(name)) {
                throw std::runtime_error("Unknown builtin: " + name);
            }

            int builtinId = nextBuiltinId--;
            runtime->registerBuiltinId(builtinId, name);

            return new Function(builtinId);
        }

        std::string name = parseIdentifier();

        Function* bound = runtime->lookup(name);
        if (bound) {
            return bound->clone();
        }

        auto it = symbols.find(name);
        if (it == symbols.end()) {
            throw std::runtime_error("Undefined symbol: " + name);
        }
        return new Function(it->second);
    }

    Function* parseApplication() {
        Function* left = parseAtom();

        while (true) {
            // Save position BEFORE peek()/skipSpaces() modifies it
            size_t posBeforePeek = pos;

            if (!isAtomStart()) break;
            if (looksLikeNewStatement(posBeforePeek)) {
                // Reset pos so parent can rescan correctly
                pos = posBeforePeek;
                break;
            }

            Function* right = parseAtom();
            left = new Function(left, right);
        }

        return left;
    }

    Function* parseLambda() {
        if (!match('\\')) {
            throw std::runtime_error("Expected '\\'");
        }
        std::string param = parseIdentifier();
        if (!match('.')) {
            throw std::runtime_error("Expected '.'");
        }

        int oldId = -1;
        auto oldIt = symbols.find(param);
        if (oldIt != symbols.end()) {
            oldId = oldIt->second;
        }

        int id = nextId++;
        symbols[param] = id;

        Function* body = parseApplication();

        if (oldId >= 0) {
            symbols[param] = oldId;
        } else {
            symbols.erase(param);
        }

        return new Function(id, body);
    }

    std::pair<std::string, Function*> parseAssignment() {
        std::string name = parseIdentifier();
        if (!match('=')) {
            throw std::runtime_error("Expected '='");
        }
        Function* expr = parseApplication();
        return {name, expr};
    }

    std::pair<std::string, Function*> parseBuiltin() {
        if (!match(':')) {
            throw std::runtime_error("Expected ':'");
        }
        std::string name = parseIdentifier();

        if (!runtime->isBuiltin(name)) {
            throw std::runtime_error("Unknown builtin: " + name);
        }

        Function* arg = parseApplication();
        return {name, arg};
    }

    std::pair<std::string, Function*> parseStatement() {
        skipSpaces();
        if (peek() == ':') {
            return parseBuiltin();
        }

        size_t savedPos = pos;

        if (std::isalnum(peek()) || peek() == '_') {
            std::string name = parseIdentifier();
            skipSpaces();
            if (peek() == '=') {
                pos = savedPos;
                return parseAssignment();
            }
        }

        pos = savedPos;
        Function* expr = parseApplication();
        return {"", expr};
    }

    std::vector<std::pair<std::string, Function*>> parseProgram() {
        std::vector<std::pair<std::string, Function*>> statements;

        while (pos < source.length()) {
            skipSpaces();
            if (pos >= source.length()) break;

            statements.push_back(parseStatement());
        }

        return statements;
    }

    bool hasMore() {
        skipSpaces();
        return pos < source.length();
    }

    std::pair<std::string, Function*> parseNextStatement() {
        skipSpaces();
        if (pos >= source.length()) {
            throw std::runtime_error("No more statements");
        }
        return parseStatement();
    }
};

inline void Runtime::run(const std::string& source) {
    Parser parser(source, this);

    while (parser.hasMore()) {
        auto [name, func] = parser.parseNextStatement();

        if (name.empty()) {
            Function* evaluated = func->eval(this);
            delete evaluated;
            delete func;
        } else if (isBuiltin(name)) {
            Function* evaluated = func->eval(this);
            callBuiltin(name, evaluated);
            delete evaluated;
            delete func;
        } else {
            Function* evaluated = func->eval(this);
            bind(name, evaluated);
            delete func;
        }
    }
}

extern "C" {
#else
typedef struct Function Function;
typedef struct Runtime Runtime;
typedef struct Parser Parser;
typedef Function* (*Fn)(Function*);
#endif

Function *createFunction(int id, Function *body);
Function *createApplication(Function *func, Function *arg);
void destroyFunction(Function *func);
Function* reduceFunction(Function* source, Function* replacer);
Function* evalFunction(Function* func);
int getFunctionId(Function* func);
Function* getFunctionBody(Function* func);
Function* getFunctionArg(Function* func);
Function* cloneFunction(Function* func);
int functionIsVar(Function* func);
int functionIsLambda(Function* func);
int functionIsApp(Function* func);

Runtime *createRuntime();
void destroyRuntime(Runtime* runtime);
void runtimeSet(Runtime* runtime, Function* fn);
Function* runtimeGet(Runtime* runtime, int index);
int runtimeSize(Runtime* runtime);
void runtimeRegisterBuiltin(Runtime* runtime, const char* name, Fn callback);
int runtimeHasBuiltin(Runtime* runtime, const char* name);
Function* runtimeCallBuiltin(Runtime* runtime, const char* name, Function* arg);
void runtimeRun(Runtime* runtime, const char* source);

Parser* createParser(const char* source, Runtime* runtime);
void destroyParser(Parser* parser);

void freeString(const char* str);
const char* functionToString(Function* func);

#ifdef __cplusplus
}
#endif
