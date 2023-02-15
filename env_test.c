#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>

#define MAX_BUFFER 1024

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

char** construct_arg_list(int ac, char* av[])
{
    char** argList = (char**)malloc(sizeof(char*) * (ac));
    int i =1;
    while(i < ac)
    {
        argList[i-1] = (char*)malloc(strlen(av[i]) + 1);
        memset(argList[i-1], 0, strlen(av[i]) + 1);
        strcpy(argList[i-1], av[i]);
        i++;
    }
    argList[i - 1] = NULL;
    return argList;
}

int get_number_arguments(char* input)
{
    int arg_count = 0;
    int i = 0;
    if(input[0] == '\0')
    {
        return 0;
    }
    arg_count++;
    while (input[i] != '\0')
    {
        if(input[i] == ' ')
        {
            arg_count++;
        }
        i++;
    }
    return arg_count;
}

char** construct_arg_list_from_input(char* input)
{
    int arg_count = get_number_arguments(input);
    char** argList = (char**)malloc(sizeof(char*) * (arg_count + 1));
    int i_start= 0;
    int i_end = 0;
    int j = 0;

    while(true)
    {
        if(input[i_end] == ' ' || input[i_end] == '\0')
        {
            argList[j] = (char*)malloc(i_end - i_start + 1);
            memset(argList[j], 0, i_end - i_start + 1);
            strncpy(argList[j], &input[i_start], i_end);
            i_start = i_end + 1;
            j++;
        }
        if(input[i_end] == '\0')
        {
            argList[j] = NULL;
            return argList;
        }
        i_end++;
    }

    return NULL;

}

void print_arg_list(char** argList)
{
    int i = 0;
    printf("{");
    while(argList[i])
    {
        printf("%s, ", argList[i]);
        i++;
    }
    printf("}\n");
}

void free_arg_list(char** argList)
{
    int i =0;
    while(argList[i])
    {
        free(argList[i]);
        i++;
    }
    free(argList);
}

int main(int ac, char* av[], char* env[])
{
    char buffer[MAX_BUFFER];
    // char** argList_av = construct_arg_list(ac, av);
    // char* path = read_path(getenv("PATH"), argList_av[0]);
    // if(!path)
    // {
    //     printf("Couldn't find command!\n");
    //     return 0;
    // }

    while(true)
    {   
        pid_t pid, wpid;
        int status;

        pid = fork();
        if(pid == 0)
        {
            memset(buffer, 0, MAX_BUFFER);
            printf("MA - my_zsh>");
            scanf("%[^\n]s", buffer);
            char** argList = construct_arg_list_from_input(buffer);
            char* path = read_path(getenv("PATH"), argList[0]);

            if(!path)
            {
                if(strcmp("quit", argList[0]) == 0)
                {
                    return 1;
                }
                printf("Couldn't find command!\n");
                return 1;
            }

            strcat(path, "/");
            strcat(path, argList[0]);
            argList[0] = (char*)realloc(argList[0], strlen(path) + 1);
            memset(argList[0], 0,  strlen(path) + 1);
            strcpy(argList[0], path);
            char* envList[] = {"HOME=/root", getenv("PATH"), NULL};
            if(execve(argList[0], argList, envList) == -1)
            {
                perror("lsh");
            }
            free_arg_list(argList);
            free(path);
            exit(EXIT_FAILURE);

        }else
            {
                do {
                    wpid = waitpid(pid, &status, WUNTRACED);
                } while(!WIFEXITED(status) && !WIFSIGNALED(status));
            }
    }

    // strcat(path, "/");
    // strcat(path, argList[0]);
    // argList[0] = (char*)realloc(argList[0], strlen(path) + 1);
    // memset(argList[0], 0,  strlen(path) + 1);
    // strcpy(argList[0], path);
    // char* envList[] = {"HOME=/root", getenv("PATH"), NULL};
    // execve(argList[0], argList, envList);
    
    
    // free_arg_list(ac, argList);
    // free(path);
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