#include "libhalf.h"

Function* test_builtin(Function* f) {
    printf("yo\n");
    return f;
}

int main() {
    const char* src = "a = \\x.x; :test a;";
    struct ParserParseTuple pout = lex_parse_script(src);

    Runtime* rtm = new_runtime(pout.program, pout.functions);
    runtime_add_builtin(rtm, "test", test_builtin);
    runtime_run(rtm);

    return 0;
}
