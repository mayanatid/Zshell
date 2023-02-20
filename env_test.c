#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

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
    closedir(dr);
    return false;

}

char* read_path(char* path, char* cmnd)
{
    char s_path[1024];
    memset(s_path, 0, 1024);
    int i = 5; // Skip 'PATH=' prefix
    int j = 0;
    
    while(i < (int)strlen(path))
    {
        if(path[i] == ':')
        {
            if(cmnd_in_dir(s_path, cmnd))
            {
                // printf("Found in %s\n", s_path);
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

char** parse_buffer_commands(char* buffer)
{
    int i =0;
    int j =0;
    int i_start=0;
    int i_end=0;
    int cmnd_count =0;
    while(buffer[i]!= '\0')
    {
        // printf("buffer[%d] = %c\n", i, buffer[i]);
        if(buffer[i] == '\n')
        {
            cmnd_count++;
        }
        i++;
    }
    cmnd_count++; // For last cmnd
    // printf("cmnd_count = %d\n", cmnd_count);
    char** cmnd_list = (char**)malloc(sizeof(char*)*(cmnd_count + 1));
    i =0;
    while(buffer[i_end] != '\0')
    {
        if(buffer[i_end] == '\n')
        {
            cmnd_list[j] = (char*)malloc(i_end-i_start+1);
            strncpy(cmnd_list[j], &buffer[i_start], i_end - i_start);
            i_start = i_end + 1;
            j++;
        }
        i_end++;
    }

    cmnd_list[j] = (char*)malloc(i_end-i_start+1);
    memset(cmnd_list[j], 0 , i_end - i_start + 1);
    strncpy(cmnd_list[j], &buffer[i_start], i_end - i_start);
    j++;
    cmnd_list[cmnd_count] = NULL;
    return cmnd_list;
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
            // printf("i_start: %d... i_end: %d\n", i_start, i_end);
            // print_arg_list(argList);
            argList[j] = (char*)malloc(i_end - i_start + 1);
            memset(argList[j], 0, i_end - i_start + 1);
            strncpy(argList[j], &input[i_start], i_end - i_start);
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

void exec_cd(char* cwd_buffer, char* temp_buffer, char** argList)
{
    if(!argList[1] || strcmp(argList[1], "~") == 0)
    {
        
        getcwd(cwd_buffer, MAX_BUFFER);
        chdir(getenv("HOME"));
    }
    else if(strcmp(argList[1], "-") == 0)
    {
        memset(temp_buffer, 0, MAX_BUFFER);
        getcwd(temp_buffer, MAX_BUFFER);
        chdir(cwd_buffer);
        strcpy(cwd_buffer, temp_buffer);

    }
    else
    {
        memset(cwd_buffer, 0, MAX_BUFFER);
        getcwd(cwd_buffer, MAX_BUFFER);
        chdir(argList[1]);
    }
}

int find_path_in_env(char* env[])
{
    int i =0;
    while(env[i])
    {
        if(strncmp(env[i], "PATH", 4) == 0)
        {
            return i;
        }
        i++;
    }
    return i;
}

char* construct_env_string(char* env[])
{
    int k =0;
    int len=0;
    while(env[k])
    {
        len += strlen(env[k]);
        k++;
    }

    char* env_str = (char*)malloc(len + k + 1);
    k = 0;
    len = 0;
    while(env[k])
    {
    
        strcat(env_str, env[k]);
        strcat(env_str, "\n");
        k++;
    }
    return env_str;


}

void exec_prog(char** argList, char** env)
{
    pid_t pid;
    int status;
    //char* envList[] = {"HOME=/root", getenv("PATH"), NULL};
    pid = fork();
    if(pid == 0)
    {
        if(execve(argList[0], argList, env) == -1)
        {
            perror("lsh");
        }
        exit(EXIT_FAILURE);

    }else
        {
            do 
                {
                    waitpid(pid, &status, WUNTRACED);
                } while(!WIFEXITED(status) && !WIFSIGNALED(status));
        }

}


int main(int ac, char* argv[], char* env[])
{
    if(ac < 0)
    {
        return 0;
    }
    if(strcmp(argv[0], "BLAH") == 0)
    {
        return 0;
    }
    
    

    char buffer[MAX_BUFFER];
    char cwd_buffer[MAX_BUFFER];
    char temp_buffer[MAX_BUFFER];
    memset(cwd_buffer, 0, MAX_BUFFER);
    getcwd(cwd_buffer, MAX_BUFFER);
    bool process = true;
    // return 0;
    while(process)
    {   
        pid_t pid;
        int status;
        int scanf_ret;
        memset(buffer, 0, MAX_BUFFER);
        // printf("MA - my_zsh>");
        // fflush(stdout);
        scanf_ret = read(STDIN_FILENO, buffer, MAX_BUFFER);
        int size = strlen(buffer);
        // printf("SIZE:%d\n", size);
        memset(buffer + size - 1, 0, MAX_BUFFER - (size - 1));
        // buffer[size-1] = '\0';
        // printf("buffer: %s\n", buffer);
        // scanf_ret = scanf(" %[^\n]",buffer);
        if(scanf_ret == 0) return 0;

        char** cmnd_list = parse_buffer_commands(buffer);
        // print_arg_list(cmnd_list);
        int j_cmnd =0;
        while(cmnd_list[j_cmnd])
        {
            
            char** argList = construct_arg_list_from_input(cmnd_list[j_cmnd]);
            int path_i = find_path_in_env(env);
            char* path = read_path(env[path_i], argList[0]);
            struct stat st;
            // char* path = read_path(getenv("PATH"), argList[0]);
            if(path == NULL)
            {
                if(strcmp("exit", argList[0]) == 0)
                {
                    process = false;
                }
                else if(strcmp("cd", argList[0]) == 0)
                {
                    exec_cd(cwd_buffer, temp_buffer, argList);
                }
                else if(buffer[0] == '.')
                {
                    exec_prog(argList, env);
                }
                else if(stat(buffer, &st) == 0 && st.st_mode & S_IXUSR)
                {
                    exec_prog(argList, env);   
                }
                else
                {
                    // process = false;
                    printf("Couldn't find command!\n");
                }      
            }
            else if(strcmp(buffer, "env") == 0)
            {
                char* env_str = construct_env_string(env);
                printf("%s\n", env_str);
                free(env_str);
            }
            else
            {
                strcat(path, "/");
                strcat(path, argList[0]);
                argList[0] = (char*)realloc(argList[0], strlen(path) + 1);
                memset(argList[0], 0,  strlen(path) + 1);
                strcpy(argList[0], path);
                ///char* envList[] = {"HOME=/root", getenv("PATH"), NULL};

                pid = fork();
                if(pid == 0)
                {
                    if(execve(argList[0], argList, env) == -1)
                    {
                        perror("lsh");
                    }
                    exit(EXIT_FAILURE);

                }else
                    {
                        do 
                            {
                                waitpid(pid, &status, WUNTRACED);
                            } while(!WIFEXITED(status) && !WIFSIGNALED(status));
                    }
                free_arg_list(argList);
            }
            free(path);
            j_cmnd++;
        }
        
        free_arg_list(cmnd_list);
    }
    
        

    return 0;
}