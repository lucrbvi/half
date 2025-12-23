#include "libhalf.h"

int main() {
    const char* src = "a = \\x.x; b = \\y.y";
    Lexer* l = new_lexer(src);

    struct LexerLexTuple out = lexer_lex(l); 
    printf("Found %zu tokens:\n", out.counter);
    for (size_t i = 0; i < out.counter; i++) {
        if (out.array[i] != NULL && out.array[i]->value != NULL) {
            printf("Token %zu: %s (type: %d)\n", i, out.array[i]->value, out.array[i]->type);
        }
    }
        
    free_lexer(l);

    Parser* p = new_parser(out.array, out.counter);

    free_tokens(out.array, out.counter);
    free(p);
        
    return 0;
}
