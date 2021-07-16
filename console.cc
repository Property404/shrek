#include "console.h"
#include "cmisc.h"
#include "io.h"

#define CONSOLE_INPUT_BUFFER_SIZE 100
#define CONSOLE_ARGV_BUFFER_SIZE 16

static int echo(int argc, char** argv);
static int md(int argc, char** argv);

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
                putchar('\n');
                break;
            }
            if (buffer_index < sizeof(input_buffer)) {
                if(c >= 0x20 && c<0x7f) {
                    putchar(c);
                } else {
                    printf("[%x]", c);
                }
                input_buffer[buffer_index++] = c;
            }
        }
        input_buffer[buffer_index] = 0;

        if (buffer_index == 0)
            continue;

        if(! strncmp("exit", input_buffer, buffer_index)) {
            puts("Exited!\n");
            break;
        }

        char* argv[CONSOLE_ARGV_BUFFER_SIZE];
        int argc = 0;
        {
            char* saveptr = input_buffer;
            char* word = NULL;
            while((word = strsep(&saveptr, " \t\n\r\v"))) {
                argv[argc++] = word;
            }
            argv[argc] = NULL;
        }

        if (! strncmp("echo", argv[0], buffer_index)) {
            echo(argc, argv);
        // Display memory
        } else if (! strncmp("md", argv[0], buffer_index)) {
            md(argc, argv);
        } else {
            printf("Unknown command: %s\n", argv[0]);
        }
    }
}

static int echo(int argc, char** argv) {
    for (int i=1; i<argc; i++) {
        puts(argv[i]);
        if(i!=argc-1)
            putchar(' ');
    }
    putchar('\n');
    return 0;
}

// display memory
static int md(int argc, char** argv) {
    const size_t WORDS_PER_ROW = 4;

    if(argc < 2)
    {
        puts("Error: Not enough arguments!\n");
        return -1;
    }
    const uint32_t* address = (uint32_t*)string_to_pointer(argv[1]);

    for(int i=0; i<16;i++) {
        printf("%p: ", address);
        for(unsigned j=0; j < WORDS_PER_ROW; j++) {
            printf("%08x ", *(address+j));
        }
        puts(" ");
        for(unsigned j=0; j < WORDS_PER_ROW*sizeof(uint32_t);j++) {
            char c = *((uint8_t*)(address)+j);
            if (c < 0x20 || c >= 0x7F) {
                c = '.';
            }
            putchar(c);
        }

        address += WORDS_PER_ROW;
        putchar('\n');
    }

    return 0;
}
