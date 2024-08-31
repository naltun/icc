/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define CODE_BUF_SIZE (BUF_SIZE * 10)

#define TMP_BIN_FILE "/tmp/icc_tmp.out"
#define TMP_SRC_FILE "/tmp/icc_tmp.c"

#define HELPMSG ".h - show this help message         \n" \
                ".p - print the code you have entered\n" \
                ".q - quit                           \n" \
                ".r - reset code                     \n" \
                ".v - print icc version                "

#define INCLUDES "#include <stdio.h>  \n" \
                 "#include <stdlib.h> \n" \
                 "#include <string.h> \n" \
                 "#include <unistd.h> \n"

#define CODE_TEMPLATE "%s           \n" \
                      "%s           \n" \
                      "int main() { \n" \
                      "    %s       \n" \
                      "    return 0;\n" \
                      "}            \n"

#define CC (getenv("CC") ? getenv("CC") : "cc")

#define VERSION "0.1.0beta"

void
cleanup_files()
{
    (void)unlink(TMP_BIN_FILE);
    (void)unlink(TMP_SRC_FILE);
}

int
compile_code()
{
    char cmd[BUF_SIZE];
    snprintf(cmd, sizeof(cmd), "%s -o %s %s", CC, TMP_BIN_FILE, TMP_SRC_FILE);

    return system(cmd);
}

void
exec_code()
{
    if (system(TMP_BIN_FILE) == -1) {
        perror("system");
        cleanup_files();
        exit(EXIT_FAILURE);
    }
}

void
handler(int sig)
{
    cleanup_files();
    exit(EXIT_SUCCESS);
}

void
write_code(const char *functions, const char *main_code)
{
    FILE *fd = fopen(TMP_SRC_FILE, "w");
    if (!fd) {
        perror("fopen");
        cleanup_files();
        exit(EXIT_FAILURE);
    }

    fprintf(fd, CODE_TEMPLATE, INCLUDES, functions, main_code);
    fclose(fd);
}

int
main()
{
    char usr_input[BUF_SIZE];
    char functions[CODE_BUF_SIZE];
    char main_code[CODE_BUF_SIZE];
    char codebuf[CODE_BUF_SIZE];

    int inside_function = 0;
    int main_code_ready = 0;

    signal(SIGINT, handler);

    printf(" _                | Interactive C Compiler v%s \n"
           "(_)               |                            \n"
           " _    ___    ___  | Type '.h' for help         \n"
           "| |  / __|  / __| |   and '.q' to quit         \n"
           "| | | (__  | (__  |                            \n"
           "|_|  \\___|  \\___| | Write your C code below\n\n", VERSION);

    while (1) {
        printf(">>> ");

        if (!fgets(usr_input, sizeof(usr_input), stdin))
            break;

        /* START parse dot commands */
        if (strncmp(usr_input, ".h", 2) == 0) {
            printf("%s\n", HELPMSG);
            continue;
        } else if (strncmp(usr_input, ".p", 2) == 0) {
            printf(CODE_TEMPLATE, INCLUDES, functions, main_code);
            continue;
        } else if (strncmp(usr_input, ".q", 2) == 0) {
            cleanup_files();
            exit(EXIT_SUCCESS);
        } else if (strncmp(usr_input, ".r", 2) ==0) {
            codebuf[0] = '\0';
            functions[0] = '\0';
            main_code[0] = '\0';
            continue;
        } else if (strncmp(usr_input, ".v", 2) == 0) {
            printf("icc v%s\n", VERSION);
            continue;
        }
        /* END parse dot commands */


        /* Append user input to the program being constructed if not a newline character */
        if (strncmp(usr_input, "\n", 1) != 0)
            strncat(codebuf, usr_input, CODE_BUF_SIZE - strlen(codebuf) - 1);

        /* Check if we're inside a function definition */
        if (strstr(usr_input, "{") != NULL)
            inside_function++;
        else if (strstr(usr_input, "}") != NULL)
            inside_function--;

        /* If not inside a function, validate user code */
        if (inside_function == 0) {
            /* Check if we have a complete function */
            if (strstr(codebuf, "(") != NULL &&
                strstr(codebuf, ")") != NULL &&
                strstr(codebuf, "{") != NULL) {
                strncat(functions, codebuf, CODE_BUF_SIZE - strlen(functions) - 1);
                codebuf[0] = '\0';
            /* Check if we are working with structs */
            } else if (strstr(codebuf, "struct") != NULL) {
                if (strstr(codebuf, "{") != NULL)
                    strncat(functions, codebuf, CODE_BUF_SIZE - strlen(functions) - 1);
                else
                    strncat(main_code, codebuf, CODE_BUF_SIZE - strlen(main_code) - 1);
                codebuf[0] = '\0';
            /* Check if we have code that is not a function */
            } else if (strstr(codebuf, ";") != NULL || main_code_ready) {
                strncat(main_code, codebuf, CODE_BUF_SIZE - strlen(main_code) - 1);
                codebuf[0] = '\0';
                main_code_ready = 0;
            } else if (strstr(codebuf, "return") != NULL) {
                /* Return statement in main code */
                main_code_ready = 1;
            }
        }

        write_code(functions, main_code);

        if (compile_code() == 0)
            exec_code();
        else
            printf("Compilation error\n");
    }

    cleanup_files();
    return EXIT_SUCCESS;
}
