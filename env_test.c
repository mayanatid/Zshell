#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>

bool cmnd_in_dir(char* dir, char* cmnd)
{
    DIR *dr;
    struct dirent *en;
    dr = opendir(dir);
    if(dr)
    {
        while((en = readdir(dr)) != NULL)
        {
            if(strcmp(cmnd, en->d_name) == 0)
            {
                closedir(dr);
                return true;
            }
        }
    }
    return false;

}

char* read_path(char* path, char* cmnd)
{
    char s_path[1024];
    memset(s_path, 0, 1024);
    int i = 0;
    int j = 0;

    while(path[i] != '\0')
    {
        if(path[i] == ':')
        {
            if(cmnd_in_dir(s_path, cmnd))
            {
                char* r_path = (char*)malloc(j + 1);
                memset(r_path, 0, j+1);
                strcpy(r_path, s_path);
                return r_path;
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
    return NULL;
}

int main(int ac, char* av[], char* env[])
{


    char* path = read_path(getenv("PATH"), av[1]);
    if(!path)
    {
        printf("Couldn't find command!\n");
        return 0;
    }
    // printf("Found ls in %s\n", path);
    strcat(path, "/");
    strcat(path, av[1]);
    char* arglist[] = {path, "ls", NULL};
    char* envList[] = {"HOME=/root", getenv("PATH"), NULL};
    execve(arglist[0], arglist, envList);
    
    
    
    free(path);
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