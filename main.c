#include "libhalf.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc > 1) {
        const char* script = read_script(argv[1]);
        struct ParserParseTuple pout = lex_parse_script(script);
        if (argc > 2) {
            size_t total_len = 0;
            for (int i = 2; i < argc; i++) {
                total_len += strlen(argv[i]);
                if (i < argc - 1) {
                    total_len++; // spaces
                }
            }
            total_len++; // '\0'
            
            char* combined = malloc(total_len);
            combined[0] = '\0';
            
            for (int i = 2; i < argc; i++) {
                strcat(combined, argv[i]);
                if (i < argc - 1) {
                    strcat(combined, " ");
                }
            }
            
            update_input_data(combined);
            free(combined);
        }

        Runtime* rtm = new_runtime(pout.program, pout.functions);
        runtime_run(rtm);
    }
    
    return 0;
}
