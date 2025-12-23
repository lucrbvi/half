#include "libhalf.h"

Function* test_builtin(Function* f) {
    return f;
}

int main() {
    const char* src = "a = \\x.x; :test a";
    Lexer* l = new_lexer(src);

    struct LexerLexTuple out = lexer_lex(l); 
    printf("Found %zu tokens:\n", out.counter);
    for (size_t i = 0; i < out.counter; i++) {
        if (out.array[i] != NULL && out.array[i]->value != NULL) {
            printf("Token %zu: %s (type: %d)\n", i, out.array[i]->value, out.array[i]->type);
        }
    }
        
    free_lexer(l);
    printf("\n");

    Parser* p = new_parser(out.array, out.counter);
    struct ParserParseTuple parser_out = parser_parse(p);
    for (size_t i = 0; i < parser_out.functions; i++) {
        printf("F%zu %s(%zu)\n",parser_out.program[i]->id, parser_out.program[i]->name, parser_out.program[i]->body_count);
    }

    Runtime* r = new_runtime(parser_out.program, parser_out.functions);
    runtime_add_builtin(r, "test", test_builtin); 
    runtime_run(r);

    free_tokens(out.array, out.counter);
    free(p);
        
    return 0;
}
