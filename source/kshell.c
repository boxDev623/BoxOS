#include <kshell.h>
#include <vga.h>
#include <kcsl.h>
#include <string.h>

#define MAX_SHELL_BUFFER_SIZE 256

typedef struct
{
    char buffer[MAX_SHELL_BUFFER_SIZE];
}
shell_t;

shell_t shell;

void kshell_initialize(void)
{
    char *user = "user";
    char *host = "boxos";
    kprintf("%s@%s $ ", user, host);

    kcsl_set_cursor_position(kcsl_get_column(), kcsl_get_row());
}

void kshell_write_char(char ch)
{
    int len = strlen(shell.buffer);

    if (len < MAX_SHELL_BUFFER_SIZE)
    {
        shell.buffer[len] = ch;
        shell.buffer[len+1] = '\0';

        kprintf("%c", ch);

        kcsl_set_cursor_position(kcsl_get_column(), kcsl_get_row());
    }
}

void kshell_backspace(void)
{
    int len = strlen(shell.buffer);
    if (len > 0)
    {
        shell.buffer[len - 1] = '\0';
        kcsl_backspace();

        kcsl_set_cursor_position(kcsl_get_column(), kcsl_get_row());
    }
}

void kshell_execute_command(void)
{
    if (strlen(shell.buffer) <= 0)
        return;
    else if (strcmp(shell.buffer, "help pls") == 0)
    {
        kprintf("\nyes uwu\n");
    }
    else if (strcmp(shell.buffer, "clear") == 0)
    {
        kcsl_clear();

        kcsl_set_column(0);
        kcsl_set_row(0);
    }
    else
    {
        kprintf("\nInvalid command ");
        kcsl_set_color(VGA_COLOR_LIGHT_RED);
        kprintf("%s\n", shell.buffer);
        kcsl_set_color(VGA_COLOR_LIGHT_GREY);
    }

    shell.buffer[0] = '\0';
    
    char *user = "user";
    char *host = "boxos";
    kprintf("%s@%s $ ", user, host);

    kcsl_set_cursor_position(kcsl_get_column(), kcsl_get_row());
}