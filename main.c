#include <stdlib.h>
#include "Shell/Shell.h"

int main(int argc, char* argv[], char* env[])
{
    if(argc > 1)
    {
        fprintf(stderr, "too many arguments");
        return EXIT_FAILURE;
    }
    if(argv[1])
    {
        fprintf(stderr, "too many arguments");
        return EXIT_FAILURE;
    }

    Shell* zsh = new_shell(env);
    zsh->listen(zsh);
    zsh->destroy(zsh);

    return EXIT_SUCCESS;
}