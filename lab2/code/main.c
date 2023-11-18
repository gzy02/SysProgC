#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#define true 1

Cmd_t root = 0;
extern int yyparse();

int main()
{
    while (true)
    {
        printf("css-lab@ ");
        yyparse();

        if (root == NULL)
        {
            continue;
        }

        if (root->type == CMD_ATOM)
        {
            struct Cmd_Atom *acmd = (struct Cmd_Atom *)root;
            struct node *node = acmd->node;
            if (!strcmp(node->data, "exit"))
            { // input "exit" cmd
                break;
            }
            else if (!strcmp(node->data, "cd"))
            { // input "cd" cmd
                node = node->next;
                if (node)
                {
                    char cdPath[20];
                    strcpy(cdPath, node->data);
                    if (chdir(cdPath) < 0)
                        fprintf(stderr, "cannot cd %s.\n", cdPath);
                }
            }
            else
            { // other cmds
                if (fork() == 0)
                {
                    // Cmd_print(root);
                    Cmd_run(root);
                }
                wait(0);
            }
        }
        else
        { // Compound commands
            if (fork() == 0)
            {
                // Cmd_print(root);
                Cmd_run(root);
            }
            wait(0);
        }
    }

    return 0;
}
