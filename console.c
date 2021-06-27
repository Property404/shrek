#include "console.h"
#include "cmisc.h"
#include "io.h"

#define CONSOLE_INPUT_BUFFER_SIZE 100

void console(void) {
    char input_buffer[CONSOLE_INPUT_BUFFER_SIZE + 1];// Extra 1 for null byte
    memcpy(&input_buffer, 0x00, sizeof(input_buffer));


    while (true) {
        puts("Shrek> ");
        size_t buffer_index = 0;

        while(true) {
            while(! testchar());
            const int c = getchar();
            if (c < 0) {
                puts("[IO-Error]");
            }
            if (c == '\r' || c == '\n') {
                putchar('[LF]\n');
                break;
            }
            if (buffer_index < sizeof(input_buffer)) {
                putchar(c);
                input_buffer[buffer_index++] = c;
            }
        }
        input_buffer[buffer_index] = 0;

        if (buffer_index == 0)
            continue;

        if(! strncmp("exit", input_buffer, buffer_index)) {
            puts("Exited!\n");
            break;
        } else {
            puts("Unknown command :");
            puts(input_buffer);
            putchar('\n');
        }
    }
}
