#include "libhalf.h"

extern const char* input_data;

int main(int argc, char* argv[]) {
    if (argc > 1) {
        input_data = argv[1];
    }

    const char* src = "true = \\x.\\y.x; false = \\x.\\y.y; :show :read; :show :read; :show :read; :show :read; :show :read; :show :read; :show :read; :show :read;";
    struct ParserParseTuple pout = lex_parse_script(src);

    Runtime* rtm = new_runtime(pout.program, pout.functions);
    runtime_run(rtm);

    return 0;
}
