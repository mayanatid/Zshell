#include <stdio.h>
#include <unistd.h>

int main(int ac, char* av[], char* env[])
{

    printf("env[0] = %s\n", env[0]);
    printf("Main program started\n");
    char* argv[] = { "ls", "-l", NULL};
    char* envp[] = { env[0], NULL };
    if (execve("/usr/bin/ls", argv, envp) == -1)
    perror("Could not execve");
    
    
    return 0;


}