#ifdef __cplusplus

#include <vector>
#include <string>
#include <cctype>
#include <memory>
#include <stdexcept>
#include <unordered_map>

class Function {
private:
    Function* substitute(int var_id, Function* replacement) {
        if (!body) {
            return (id == var_id) ? replacement->clone() : clone();
        }
        if (id == var_id) return clone();
        return new Function(id, body->substitute(var_id, replacement));
    }

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
};

/*
 * Runtime class
 * - Manage functions and their ids
 * - Is used with the parser
 *
 * IS NOT MEANT TO BE USED DIRECTLY
 */
class Runtime {
private:
    std::vector<std::unique_ptr<Function>> vars;

public:
    Runtime() = default;

    void set(Function* fn) {
        vars.push_back(std::unique_ptr<Function>(fn));
    }

    Function* get(size_t index) {
        if (index >= vars.size()) return nullptr;
        return vars[index].get();
    }

    int getFnId(Function* fn) {
        for (size_t i = 0; i < vars.size(); ++i) {
            if (vars[i].get() == fn) return static_cast<int>(i);
        }
        return -1;
    }

    int size() const {
        return vars.size();
    }
};

class Program {
public:
    std::vector<std::pair<std::string, Function*>> statements;
    
    Program(std::vector<std::pair<std::string, Function*>> stmts) 
        : statements(std::move(stmts)) {}
    
    int size() const { return statements.size(); }
    
    const char* getName(int index) const {
        if (index < 0 || static_cast<size_t>(index) >= statements.size()) return nullptr;
        return statements[index].first.c_str();
    }
    
    Function* getFunction(int index) const {
        if (index < 0 || static_cast<size_t>(index) >= statements.size()) return nullptr;
        return statements[index].second;
    }
};

class Parser {
private:
    Runtime* runtime;
    std::string source;
    size_t pos;
    std::unordered_map<std::string, int> symbols;

    void skipSpaces() {
        while (pos < source.length() && std::isspace(source[pos])) {
            pos++;
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

        if (id.empty()) throw std::runtime_error("Unknown ID " + std::to_string(pos));
        return id;
    }

    bool isEndOfExpr() {
        char c = peek();
        return c == '\0' || c == '=' || c == ':' || c == ')' || c == '\n';
    }

public:
    Parser(std::string source, Runtime* rt = nullptr) 
        : runtime(rt ? rt : new Runtime()), source(source), pos(0) {}

    /*
     * Half Grammar:
     *
     * program := (statement)*
     * statement := assignment | builtin
     * assignment := identifier '=' expression
     * builtin := ':' identifier expression
     * expression := lambda | application | atom
     * lambda := '\' identifier '.' expression
     * application := expression expression
     * atom := identifier | '(' expression ')'
     * identifier := [a-zA-Z0-9]+
     */

    Function* parseExpression() {
        if (peek() == '\\') {
            return parseLambda();
        }
        if (peek() == '(') {
            consume();
            auto expr = parseExpression();
            if (!match(')')) {
                throw std::runtime_error("except ')'");
            }
            return expr;
        }
        std::string name = parseIdentifier();
        auto it = symbols.find(name);
        if (it == symbols.end()) {
            throw std::runtime_error("Undefined symbol: " + name);
        }
        return new Function(it->second);
    }

    Function* parseLambda() {
        if (!match('\\')) {
            throw std::runtime_error("except '\\'");
        }
        std::string param = parseIdentifier();
        if (!match('.')) {
            throw std::runtime_error("except '.'");
        }

        int oldId = -1;
        auto oldIt = symbols.find(param);
        if (oldIt != symbols.end()) {
            oldId = oldIt->second;
        }

        int id = runtime->size();
        symbols[param] = id;

        Function* body = parseExpression();

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
            throw std::runtime_error("except '='");
        }
        Function* expr = parseExpression();
        return {name, expr};
    }

    std::pair<std::string, Function*> parseBuiltin() {
        if (!match(':')) {
            throw std::runtime_error("except ':'");
        }
        std::string name = parseIdentifier();
        Function* arg = parseExpression();
        return {name, arg};
    }

    std::pair<std::string, Function*> parseStatement() {
        skipSpaces();
        if (peek() == ':') {
            return parseBuiltin();
        }
        return parseAssignment();
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
};

extern "C" {
#else
typedef struct Function Function;
typedef struct Runtime Runtime;
typedef struct Program Program;
typedef struct Parser Parser;
#endif

Function *createFunction(int id, Function *body);
void destroyFunction(Function *func);
Function* reduceFunction(Function* source, Function* replacer);
int getFunctionId(Function* func);
Function* getFunctionBody(Function* func);
Function* cloneFunction(Function* func);

Runtime *createRuntime();
void destroyRuntime(Runtime* runtime);
void runtimeSet(Runtime* runtime, Function* fn);
Function* runtimeGet(Runtime* runtime, int index);
int runtimeSize(Runtime* runtime);

Parser* createParser(const char* source, Runtime* runtime);
void destroyParser(Parser* parser);
Function* parserParseExpression(Parser* parser);
Program* parserParseProgram(Parser* parser);

void destroyProgram(Program* program);
int programSize(Program* program);
const char* programGetName(Program* program, int index);
Function* programGetFunction(Program* program, int index);

#ifdef __cplusplus
}
#endif
