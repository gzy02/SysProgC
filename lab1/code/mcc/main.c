#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "compile.h"
#include <argp.h>

// globals
Stm_t root = 0;
extern int yyparse();
extern void yyrestart(FILE *);
char cmd[1024];
char asm_file_name[] = "/tmp/ustcsseXXXXXX.s";

static char doc[] = "A simple compiler mcc";

// enum ARCH {
//     X64,
//     RISCV
// };

// Structure used to parse command line arguments
struct arguments {
    enum {
        COMPILE,
        ASSEMBLY,
    } mode;
    char *arch;
    char *output_name;
    int verbose;
    char *input_file;
};

// Command line parameter list
static struct argp_option options[] = {
    {" ", 'c', 0, 0, "Produce .o file"},
    {" ", 'S', 0, 0, "Produce .s file"},
    {"output", 'o', "OUTPUT", 0, "Produce a custom output name"},
    {"machine", 'm', "arch", 0, "Select an arch, support X64 and RISCV, default X64"},
    {"verbose", 'v', 0, 0, "Show detailed information during compilation"},
    {0}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'c':
            arguments->mode = ASSEMBLY;
            break;
        case 'S':            
            arguments->mode = COMPILE;
            break;
        case 'o':
            arguments->output_name = arg;
            break;
        case 'm':
            arguments->arch = arg;
            break;
        case 'v':
            arguments->verbose = 1;
            break;
        case ARGP_KEY_ARG:
            if (!arguments->input_file) {
                arguments->input_file = arg;
            } else {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static void verbose_on(int v, char *s) {
    if (v) {
        puts(s);
    }
}

static struct argp argp = { options, parse_opt, "FILE", doc };

int main(int argc, char *argv[]) {
    struct arguments arguments;
    // Set default value
    // arguments.mode = DEFAULT;
    arguments.output_name = NULL; // Default is NULL
    arguments.arch = NULL;
    arguments.verbose = 0;
    arguments.input_file = NULL;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (argc > 1) {
        FILE *file = fopen(arguments.input_file, "r");
        if (!file) {
            perror("fopen");
            return 1;
        }
        yyrestart(file);
    }

    // Call the parser
    if (yyparse() || !root) {
        // yyparse() returns a non-zero value to indicate a parsing error
        fprintf(stderr, "Parsing failed.\n");
        return 1;
    }

    // compile
    int fd = mkstemps(asm_file_name, 2);

    if (arguments.arch != NULL) {
        if (!strcmp(arguments.arch, "X64")) {
            compile(root, fd);
        } else if (!strcmp(arguments.arch, "RISCV")) {
            riscv_compile(root, fd);
        } 
    } else {
        compile(root, fd);
    }

    char obj_file_name[sizeof(asm_file_name)];                         // Create a new string with the same size as the original string
    strncpy(obj_file_name, asm_file_name, sizeof(asm_file_name) - 1);  // Copy original string to new string
    char *dot_position = strrchr(obj_file_name, '.');                  // Find the position of the last '.'
    if (dot_position != NULL) {
        *(dot_position + 1) = 'o';                              // Replace the character after '.' with 'o'
        *(dot_position + 2) = '\0';                             // Make sure the string ends
    }

    char *tmpname = arguments.input_file;
    // char cpcmd[128];
    switch (arguments.mode)
    {
    case COMPILE:
        if (!arguments.output_name) {
            tmpname[strlen(tmpname) - 1] = 's';
        }
        sprintf(cmd, "cp %s ./%s", asm_file_name, arguments.output_name ? arguments.output_name : tmpname);
        verbose_on(arguments.verbose, cmd);
        system(cmd);
        break;
    case ASSEMBLY:
        if (arguments.arch != NULL) {
            if (!strcmp(arguments.arch, "X64")) {
                sprintf(cmd, "gcc -c %s -o %s", asm_file_name, obj_file_name);
            } else if (!strcmp(arguments.arch, "RISCV")) {
                sprintf(cmd, "riscv64-linux-gnu-gcc -static -c %s -o %s", asm_file_name, obj_file_name);
            } 
        } else {
            sprintf(cmd, "gcc -c %s -o %s", asm_file_name, obj_file_name);
        }
        verbose_on(arguments.verbose, cmd);
        system(cmd);
        if (!arguments.output_name) {
            tmpname[strlen(tmpname) - 1] = 'o';
        }
        sprintf(cmd, "cp %s ./%s", obj_file_name, arguments.output_name ? arguments.output_name : tmpname);
        verbose_on(arguments.verbose, cmd);
        system(cmd);
        break;        
    default:
        if (arguments.arch != NULL) {
            if (!strcmp(arguments.arch, "X64")) {
                sprintf(cmd, "gcc %s -o %s", asm_file_name, obj_file_name);
            } else if (!strcmp(arguments.arch, "RISCV")) {
                sprintf(cmd, "riscv64-linux-gnu-gcc -static %s -o %s", asm_file_name, obj_file_name);
            } 
        } else {
            sprintf(cmd, "gcc %s -o %s", asm_file_name, obj_file_name);
        }
        verbose_on(arguments.verbose, cmd);
        system(cmd);
        sprintf(cmd, "cp %s %s", obj_file_name, arguments.output_name ? arguments.output_name : "a.out");
        verbose_on(arguments.verbose, cmd);
        system(cmd);
        break;
    }

    return 0;
}


