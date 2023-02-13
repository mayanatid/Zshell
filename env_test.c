#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>

bool cmnd_in_dir(char* dir, char* cmnd)
{
    // printf("files in %s:\n", dir);
    DIR *dr;
    struct dirent *en;
    dr = opendir(dir);
    if(dr)
    {
        while((en = readdir(dr)) != NULL)
        {
            // printf("%s\n", en->d_name);
            if(strcmp(cmnd, en->d_name) == 0)
            {
                return true;
            }
        }
    }
    return false;

}

void read_path(char* path)
{
    char s_path[1024];
    memset(s_path, 0, 1024);
    int i = 0;
    int j = 0;

    while(path[i] != '\0')
    {
        if(path[i] == ':')
        {
            if(cmnd_in_dir(s_path, "ls"))
            {
                printf("ls found in %s\n", s_path);
            }
            memset(s_path, 0, 1024);
            j=0;
        }
        else{
            s_path[j] = path[i];
            j++;
        }
        i++;
        
    }
}

int main(int ac, char* av[], char* env[])
{

    read_path(getenv("PATH"));
    // cmnd_in_dir("/home/docode/.goenv/versions/1.18.7/bin", "ls");

    // printf("env[0] = %s\n", env[0]);
    // printf("Main program started\n");
    // printf("PATH=%s\n", getenv("PATH"));
    // char* argv[] = { "ls", "-l", NULL};
    // char* envp[] = { env[0], NULL };
    // if (execve(argv[0], argv, envp) == -1)
    // perror("Could not execve");
    
    
    return 0;


}