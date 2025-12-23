/*

             ⠀⠀⠀⢀⣠⣴⣶⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
             ⠀⠀⣴⣿⣿⣿⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
             ⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
             ⠘⣿⣿⣿⣿⡟⠉⢿⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
             ⠀⠈⠛⠛⠋⠀⠀⠈⣿⣿⣿⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
             ⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⡀  The Half Programming Language
             ⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀
             ⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⣿⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀
             ⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀
             ⠀⠀⠀⠀⢀⣼⣿⣿⣿⣿⡿⢿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀
             ⠀⠀⠀⢠⣾⣿⣿⣿⣿⡟⠁⠘⣿⣿⣿⣿⣷⠀⠀⠀⣀⡀⠀⠀
             ⠀⠀⣠⣿⣿⣿⣿⣿⠏⠀⠀⠀⢻⣿⣿⣿⣿⡆⣰⣿⣿⣿⣷⡀
             ⠀⣴⣿⣿⣿⣿⣿⠋⠀⠀⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁
             ⠰⣿⣿⣿⣿⡿⠁⠀⠀⠀⠀⠀⠀⠘⢿⣿⣿⣿⣿⣿⣿⡟⠁⠀
             ⠀⠙⠻⠿⠛⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⠿⠟⠛⠁⠀⠀⠀

    
    This project is licensed under the Boost Software License 1.0
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// 1. Half Core (only lambda calculus)

// \x.x # this is an anonymous function
typedef struct Function {
    size_t id;          
    bool visited;
    bool builtin;
    char* name;
    size_t body_count;      // number of functions in body (max 2)
    struct Function **body; // dynamically allocated array of Function pointers
} Function;

static inline void bind_names(Function *f, size_t id, char* name) {
    if (f == NULL) return;

    if (f->body_count == 0 && f->id == id && f->name == NULL) {
        f->name = strdup(name);
    }

    for (size_t i = 0; i < f->body_count; i++) {
        bind_names(f->body[i], id, name);
    }
}

static inline Function* new_function(size_t id, char* name, Function *body[], size_t count) {
    if (count > 2) {
        fprintf(stderr, "Error: Function body count exceeds maximum allowed (max=2)\n");
        return NULL;
    }

    Function *f = (Function*)malloc(sizeof(Function));
    if (f == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    f->id = id;
    f->visited = false;
    f->builtin = false;
    f->body_count = count;

    if (name == NULL) {
        f->name = NULL;
    } else {
        f->name = (char*)malloc(strlen(name) + 1);
        if (f->name != NULL) {
            strcpy(f->name, name);
        }
    }

    if (count > 0) {
        f->body = (Function**)malloc(count * sizeof(Function*));
        if (f->body == NULL) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            free(f->name);
            free(f);
            return NULL;
        }
        memcpy(f->body, body, count * sizeof(Function*));
    } else {
        f->body = NULL;
    }

    if (name != NULL && count == 1) {
        bind_names(f->body[0], id, name);
    }

    return f;
}

static inline void free_function(Function *f) {
    if (f == NULL) return;

    if (f->body != NULL) {
        for (size_t i = 0; i < f->body_count; i++) {
            if (f->body[i] != NULL) {
                free_function(f->body[i]);
            }
        }
        free(f->body);
    }

    free(f->name);
    free(f);
}

static inline char* function_to_string(Function *f) {
    if (f == NULL) {
        return strdup("NULL");
    }
    
    char buffer[256];
    
    if (f->body_count == 0) {
        if (f->name != NULL) {
            snprintf(buffer, sizeof(buffer), "%s", f->name);
        } else {
            snprintf(buffer, sizeof(buffer), "%zu", f->id);
        }
    } else if (f->body_count == 1) {
        char* body_str = function_to_string(f->body[0]);
        if (f->name != NULL) {
            snprintf(buffer, sizeof(buffer), "(\\%s.%s)", f->name, body_str);
        } else {
            snprintf(buffer, sizeof(buffer), "(\\%zu.%s)", f->id, body_str);
        }
        free(body_str);
    } else if (f->body_count == 2) {
        char* fn_str = function_to_string(f->body[0]);
        char* arg_str = function_to_string(f->body[1]);
        snprintf(buffer, sizeof(buffer), "(%s %s)", fn_str, arg_str);
        free(fn_str);
        free(arg_str);
    }
    
    return strdup(buffer);
}

static inline Function* substitute(Function *node, size_t id, Function *arg) {
    if (node == NULL) return NULL;

    if (node->body_count == 0) {
        return (node->id == id) ? arg : node;
    }

    if (node->body_count == 1 && node->id == id) {
        return node;
    }

    for (size_t i = 0; i < node->body_count; i++) {
        node->body[i] = substitute(node->body[i], id, arg);
    }

    return node;
}

typedef struct {
    // map names->functions and functions->names
    char** names;
    Function** functions;

    size_t count;
    size_t capacity;
} Context;

Context* new_context() {
    Context* ctx = (Context*)malloc(sizeof(Context));
    if (ctx == NULL) return NULL;
    
    ctx->capacity = 10;
    ctx->count = 0;
    ctx->names = (char**)malloc(ctx->capacity * sizeof(char*));
    ctx->functions = (Function**)malloc(ctx->capacity * sizeof(Function*));
    
    if (ctx->names == NULL || ctx->functions == NULL) {
        free(ctx->names);
        free(ctx->functions);
        free(ctx);
        return NULL;
    }
    
    return ctx;
}

void context_add(Context* ctx, char* name, Function* f) {
    if (ctx->count >= ctx->capacity) {
        ctx->capacity *= 2;
        ctx->names = (char**)realloc(ctx->names, ctx->capacity * sizeof(char*));
        ctx->functions = (Function**)realloc(ctx->functions, ctx->capacity * sizeof(Function*));
    }
    
    ctx->names[ctx->count] = strdup(name);
    ctx->functions[ctx->count] = f;
    ctx->count++;
}

Function* context_get(Context* ctx, char* name) {
    for (size_t i = 0; i < ctx->count; i++) {
        if (strcmp(ctx->names[i], name) == 0) {
            return ctx->functions[i];
        }
    }
    return NULL;
}

void free_context(Context* ctx) {
    if (ctx == NULL) return;
    for (size_t i = 0; i < ctx->count; i++) {
        free(ctx->names[i]);
    }
    free(ctx->names);
    free(ctx->functions);
    free(ctx);
}

// lazy reduction
static inline Function* reduce_function(Function *root, Context* ctx) {
    if (root == NULL) return NULL;
    if (root->visited) return root;
    
    if (root->body_count == 0 && root->name != NULL && ctx != NULL) {
        Function* resolved = context_get(ctx, root->name);
        if (resolved != NULL) {
            return resolved;
        }
    }
    
    if (root->body == NULL || root->body_count == 0) {
        return root;
    }

    if (root->body_count > 2) {
        fprintf(stderr, "Error: Function body array size exceeds maximum allowed (max=2)\n");
        free_function(root);
        return NULL;
    }

    root->visited = true;

    for (size_t i = 0; i < root->body_count; i++) {
        root->body[i] = reduce_function(root->body[i], ctx);
    }

    if (root->builtin && root->body_count == 2) {
        Function *fn = root->body[0];
        Function *arg = root->body[1];
        if (fn != NULL && fn->body_count == 1 && fn->body != NULL) {
            Function *result = substitute(fn->body[0], fn->id, arg);
            return reduce_function(result, ctx);
        }
    }

    return root;
}

/* 
   2. Half Lexer
   We have a python-like syntax.

  ```hl
   x = \x.x # comment here
   y = \banana.x # return '\x.x' -> currying (https://en.wikipedia.org/wiki/Currying)
  ```
*/

typedef enum {
    TOKEN_INVALID,
    TOKEN_NEWLINE,
    TOKEN_LAMBDA, // defined by '\\'
    TOKEN_DOT,
    TOKEN_EQUAL,
    TOKEN_NAME,
    TOKEN_COLON, // to call a builtin
    TOKEN_OPAREN, // (
    TOKEN_CPAREN, // )
} Token_Type;

typedef struct {
    Token_Type type;
    const char* value;
    size_t row, line;
} Token;

typedef struct {
    size_t row, line, pos;
} Cursor;

typedef struct {
    Cursor* cursor;
    const char* source;
} Lexer;

Lexer* new_lexer(const char* source) {
    Lexer* l = malloc(sizeof(Lexer));
    if (l == NULL) return NULL;
    
    l->cursor = malloc(sizeof(Cursor));
    if (l->cursor == NULL) {
        free(l);
        return NULL;
    }
    
    l->cursor->row = 0;
    l->cursor->line = 0;
    l->cursor->pos = 0;
    l->source = source;
    return l;
}

static inline void free_lexer(Lexer* l) {
    if (l) {
        free(l->cursor);
        free(l);
    }
}

static inline char lexer_get_char(Lexer* l) {
    return l->source[l->cursor->pos];
}

static inline char lexer_get_next_char(Lexer* l) {
    return l->source[l->cursor->pos++];
}

static inline bool lexer_source_end(Lexer* l) {
    return '\0' == lexer_get_char(l);
}

static inline void lexer_skip_line(Lexer* l) {
    while (lexer_get_next_char(l) != '\n') {}
}

struct LexerLexTuple{
    Token** array;
    size_t counter;
};

// Return an array of Tokens
static inline struct LexerLexTuple lexer_lex(Lexer* l) {
    size_t counter = 0;
    size_t capacity = 35;

    Token** array = (Token**)malloc(capacity * sizeof(Token*));
    if (array == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        struct LexerLexTuple error = {NULL, 0};
        return error;
    }

    while (!lexer_source_end(l)){
        char c = lexer_get_next_char(l);

        Token* token = (Token*)malloc(sizeof(Token));
        if (token == NULL) {
            fprintf(stderr, "Error: Memory allocation failed \n");
            free(array);
            struct LexerLexTuple error = {NULL, 0};
            return error;
        }

        token->row = l->cursor->row;
        token->line = l->cursor->line;
        token->type = TOKEN_INVALID;
        token->value = NULL;
        
        // Skip whitespace
        if (c == ' ' || c == '\t') {
            continue;
        }

        if (c == '\n' || c == ';') {
            token->type = TOKEN_NEWLINE;
            token->value = (c == '\n') ? strdup("\n") : strdup(";");
            if (c == '\n') l->cursor->line++;
            array[counter++] = token;
            continue;
        }
        
        // Skip comments
        if (c == '#') {
            lexer_skip_line(l);
            l->cursor->line++;
            continue;
        }

        if (counter >= capacity) {
            capacity *= 2;
            Token** temp = (Token**)realloc(array, capacity * sizeof(Token*));
            if (temp == NULL) {
                fprintf(stderr, "Error: Memory reallocation failed\n");
                free(array);
                struct LexerLexTuple error = {NULL, 0};
                return error;            
            }
            array = temp;
        }

        switch (c) {
            case '\\':
                token->type = TOKEN_LAMBDA;
                token->value = strdup("\\");
                break;
            case '.':
                token->type = TOKEN_DOT;
                token->value = strdup(".");
                break;
            case '=':
                token->type = TOKEN_EQUAL;
                token->value = strdup("=");
                break;
            case ':':
                token->type = TOKEN_COLON;
                token->value = strdup(":");
                break;
            case '(':
                token->type = TOKEN_OPAREN;
                token->value = strdup("(");
                break;
            case ')':
                token->type = TOKEN_CPAREN;
                token->value = strdup(")");
                break;
            default:
                // Handle identifiers (alphanumeric + underscore)
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
                    char buffer[256] = {0};
                    buffer[0] = c;
                    int idx = 1;
                    
                    while (!lexer_source_end(l) && 
                           (isalnum(lexer_get_char(l)) || lexer_get_char(l) == '_')) {
                        buffer[idx++] = lexer_get_next_char(l);
                        if (idx >= 255) break;
                    }
                    
                    token->type = TOKEN_NAME;
                    token->value = strdup(buffer);
                } else {
                    token->type = TOKEN_INVALID;
                    char buf[2] = {c, '\0'};
                    token->value = strdup(buf);
                }
                break;
        }

        if (token->value != NULL) {
            array[counter++] = token;
        } else {
            free(token);
        }
    }

    struct LexerLexTuple tuple;
    tuple.array = array;
    tuple.counter = counter;

    return tuple;
}

static inline void free_tokens(Token** tokens, size_t count) {
    if (tokens == NULL) return;
    for (size_t i = 0; i < count; i++) {
        if (tokens[i] != NULL) {
            free((void*)tokens[i]->value);
            free(tokens[i]);
        }
    }
    free(tokens);
}

/*
   3. Half Parser 
   
   Grammar:
    program      ::= statement*
    statement    ::= comment | definition | builtin_call
    comment      ::= "#" [^\n]* "\n"
    definition   ::= identifier "=" expression
    builtin_call ::= ":" identifier expression*
    expression   ::= identifier | lambda | application | "(" expression ")"
    lambda       ::= "\" identifier "." expression
    application  ::= expression expression
    identifier   ::= [a-zA-Z_][a-zA-Z0-9_]*
*/

typedef struct {
    Token** array;
    size_t array_size, pos, functions;
} Parser;

Parser* new_parser(Token** array, size_t array_size) {
    Parser* p = malloc(sizeof(Parser));
    if (p == NULL) return NULL;

    p->array = array;
    p->array_size = array_size;
    p->pos = 0;
    p->functions = 0;
    return p;
}

void parser_error(Parser* p, const char* msg) {
    printf("Parser error at line %zu, row %zu: %s\n", p->array[p->pos]->line+1, p->array[p->pos]->row+1, msg);
}

bool parser_except(Parser* p, int type) {
    if (p->array[p->pos]->type == type) return true;
    return false;
}

bool parser_except_error(Parser* p, int type) {
    if (parser_except(p, type)) return true;
    char msg[128];
    snprintf(msg, sizeof(msg), "unexpected symbol '%s'", p->array[p->pos]->value);
    parser_error(p, msg);
    return false;
}

bool parser_end(Parser* p) {
    if (p->pos >= p->array_size) return true;
    return false;
}

Function* expression(Parser* p, Function** program); // forward declaration - check bellow for 'expression'

Function* lambda(Parser* p, Function** program) {
    p->pos++;
    
    if (p->array[p->pos]->type != TOKEN_NAME) {
        parser_error(p, "Expected parameter name after \\");
        return NULL;
    }
    
    char* param_name = (char*)p->array[p->pos]->value;
    p->pos++;
    
    if (p->array[p->pos]->type != TOKEN_DOT) {
        parser_error(p, "Expected . after parameter name");
        return NULL;
    }
    p->pos++;
    
    Function* body = expression(p, program);
    
    if (body == NULL) {
        return NULL;
    }
    
    Function* body_array[1] = {body};
    Function* lambda_func = new_function(p->functions, param_name, body_array, 1);
    
    return lambda_func;
}

Function* expression(Parser* p, Function** program) {
    if (parser_end(p)) {
        return NULL;
    }
    
    Token* current = p->array[p->pos];

    switch(current->type) {
        case TOKEN_NAME: {
            char* var_name = (char*)current->value;
            p->pos++;
            
            if (!parser_end(p) && 
                (p->array[p->pos]->type == TOKEN_NAME || 
                 p->array[p->pos]->type == TOKEN_LAMBDA ||
                 p->array[p->pos]->type == TOKEN_OPAREN)) {
                
                Function* fn = new_function(p->functions, var_name, NULL, 0);
                Function* arg = expression(p, program);
                
                if (arg == NULL) return NULL;
                
                Function* app_body[2] = {fn, arg};
                Function* app = new_function(p->functions, NULL, app_body, 2);
                return app;
            } else {
                return new_function(p->functions, var_name, NULL, 0);
            }
        }
        
        case TOKEN_LAMBDA:
            return lambda(p, program);
        
        case TOKEN_OPAREN: {
            p->pos++;
            Function* expr = expression(p, program);
            
            if (p->array[p->pos]->type != TOKEN_CPAREN) {
                parser_error(p, "Expected )");
                return NULL;
            }
            p->pos++;
            
            return expr;
        }
 
        default: {
            char msg[128];
            snprintf(msg, sizeof(msg), "unexpected symbol '%s'", current->value);
            parser_error(p, msg);
            return NULL;
        }
    }
}

void statement(Parser* p, Function** program) {
    if (parser_end(p)) return;
    
    Token* current = p->array[p->pos];

    switch(current->type) {
        case TOKEN_NAME: {
            char* var_name = (char*)current->value;
            p->pos++;
            
            if (!parser_except(p, TOKEN_EQUAL)) {
                parser_error(p, "Expected = after variable name");
                return;
            }
            p->pos++;
            
            Function* expr_result = expression(p, program);
            
            if (expr_result == NULL) {
                parser_error(p, "Failed to parse expression");
                return;
            }

            Function* named_func = new_function(
                p->functions, 
                var_name,
                expr_result->body_count > 0 ? expr_result->body : NULL,
                expr_result->body_count
            );
            
            if (named_func != NULL) {
                program[p->functions] = named_func;
                p->functions++;
            }
            
            if (!parser_end(p) && parser_except(p, TOKEN_NEWLINE)) {
                p->pos++;
            }
            break;
        }
        
        case TOKEN_COLON: {
            p->pos++;
            
            if (p->array[p->pos]->type != TOKEN_NAME) {
                parser_error(p, "Expected builtin name after :");
                return;
            }
            
            char* builtin_name = (char*)p->array[p->pos]->value;
            p->pos++;
            
            Function* expr_result = expression(p, program);
            
            if (expr_result == NULL) {
                parser_error(p, "Failed to parse expression");
                return;
            }
            
            Function* builtin_func = new_function(
                p->functions,
                builtin_name,
                expr_result->body_count > 0 ? expr_result->body : NULL,
                expr_result->body_count
            );
            
            if (builtin_func != NULL) {
                builtin_func->builtin = true;
                program[p->functions] = builtin_func;
                p->functions++;
            }
            
            if (!parser_end(p) && parser_except(p, TOKEN_NEWLINE)) {
                p->pos++;
            }
            break;
        }
        
        case TOKEN_NEWLINE: {
            p->pos++;
            break;
        }
        
        default: {
            char msg[128];
            snprintf(msg, sizeof(msg), "unexpected symbol '%s'", current->value);
            parser_error(p, msg);
            p->pos++;
            break;
        }
    }
}

struct ParserParseTuple {
    Function** program; // we will execute this element by element
    size_t functions;
};

struct ParserParseTuple parser_parse(Parser* p) {
    size_t capacity = 30;
    Function** program = (Function**)malloc(capacity * sizeof(Function*));
    if (program == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        struct ParserParseTuple error = {NULL, 0};
        return error;
    }
    
    p->functions = 0; // safe reset

    while(!parser_end(p)) {
        if (p->functions >= capacity) {
            capacity *= 2;
            Function** temp = (Function**)realloc(program, capacity * sizeof(Function*));
            if (temp == NULL) {
                fprintf(stderr, "Error: Memory reallocation failed\n");
                free(program);
                struct ParserParseTuple error = {NULL, 0};
                return error;
            }
            program = temp;
        }

        statement(p, program);
    }

    struct ParserParseTuple tuple;
    tuple.program = program;
    tuple.functions = p->functions;
    return tuple;
}

/*

Copyright 2025 Luc Robert--Villanueva

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/
