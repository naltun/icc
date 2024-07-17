#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define CODE_BUF_SIZE (BUF_SIZE * 10)

#define TMP_SRC_FILE "/tmp/icc_tmp.c"
#define TMP_BIN_FILE "/tmp/icc_tmp.out"

#define INCLUDES "#include <stdio.h>  \n" \
                 "#include <stdlib.h> \n" \
				 "#include <string.h> \n" \
				 "#include <unistd.h> \n"

#define CC (getenv("CC") ? getenv("CC") : "cc")

#define VERSION "0.1.0beta"

int
compile_code()
{
    char cmd[BUF_SIZE];
    snprintf(cmd, sizeof(cmd), "%s -o %s %s", CC, TMP_BIN_FILE, TMP_SRC_FILE);

    return system(cmd);
}

void
handler(int sig)
{
    exit(EXIT_SUCCESS);
}

void
exec_code()
{
    if (system(TMP_BIN_FILE) == -1) {
        perror("system");
        exit(EXIT_FAILURE);
    }
}

void
write_code(const char *functions, const char *main_code)
{
    FILE *fd = fopen(TMP_SRC_FILE, "w");
    if (!fd) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fprintf(fd, "%s           \n" /* header files                                 */
                "             \n"
                "%s           \n" /* user-defined functions                       */
                "             \n"
                "int main() { \n"
                "    %s       \n" /* user-defined variables, function calls, etc. */
                "             \n"
                "    return 0;\n"
                "}            \n", INCLUDES, functions, main_code);
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
        if (strncmp(usr_input, ".h", 2) == 0)
            printf("Todo...\n");
        else if (strncmp(usr_input, ".q", 2) == 0)
			exit(EXIT_SUCCESS);
        else if (strncmp(usr_input, ".v", 2) == 0)
            printf("icc v%s\n", VERSION);
        /* END parse dot commands */

        /* Append user input to program being constructed */
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
            /* Check if we have code or a statement that is not a function */
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

    (void)unlink(TMP_SRC_FILE);
    (void)unlink(TMP_BIN_FILE);

    return EXIT_SUCCESS;
}
