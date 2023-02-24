#include "Shell.h"
#include <string.h>
#include <unistd.h>

void    shell_constructor(Shell* this)
{
        memset(this->input, 0, MAX_BUFFER);
        this->process = true;
}


Shell*  new_shell(char** env)
{
        Shell template =   {
                                    .init           = &shell_constructor,
                                    .read_input     = &shell_read_input,
                                    .listen         = &shell_listen,
                                    .parse_commands = &shell_parse_commands,
                                    .parse_args     = &shell_parse_args,
                                    .parse_input    = &shell_parse_input,
                                    .execute_prog   = &shell_execute_prog,
                                    .destroy        = &shell_destroy

                                };
        Shell* result = malloc(sizeof(Shell));
        memcpy(result, &template, sizeof(Shell));
        result->envList = env;
        result->init(result);
        return result;
}

void    helper_sub_env_vars(Shell* this)
{
        Node* arg = this->arguments->head;
        while(arg)
        {
            if(arg->value[0] == '$')
            {
                char* var = getenv(&arg->value[1]);
                long len = strlen(var) +1;
                arg->value = (char*)realloc(arg->value,len);
                memset(arg->value, 0, len);
                strcpy(arg->value, var);
            }
            arg = arg->next;
        }
}

char*    helper_find_path_in_env(Shell* this)
{
        int i =0;
        while(this->envList[i])
        {
            if(strncmp(this->envList[i], "PATH", 4) == 0)
            {
                return this->envList[i];
            }
            i++;
        }
        return NULL;
}

bool    helper_cmd_in_dir(char* dir, char* cmnd)
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

char*   helper_read_path(char* path, char* cmnd)
{
        char s_path[1024];
        memset(s_path, 0, 1024);
        int i = 5; // Skip 'PATH=' prefix
        int j = 0;
        
        while(i < (int)strlen(path))
        {
            if(path[i] == ':')
            {
                if(helper_cmd_in_dir(s_path, cmnd))
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

bool    helper_check_if_built_in(Shell* this)
{
        char* arg = this->arguments->head->value;
        if(strcmp(arg, "quit") == 0)
        {
            return true;
        }
        if(strcmp(arg, "cd") == 0)
        {
            return true;
        }
        if(strcmp(arg, "setenv") == 0)
        {
            return true;
        }
        if(strcmp(arg, "unsetenv") == 0)
        {
            return true;
        }
        
        
}

int     shell_read_input(Shell* this)
{
        int read_ret = read(STDIN_FILENO, this->input, MAX_BUFFER);
        int input_size = strlen(this->input);
        memset(this->input + input_size - 1, 0, MAX_BUFFER - (input_size - 1)); // Set ending values to \0
        return read_ret;
}

void    shell_parse_commands(Shell* this)
{
        this->commands = new_ll();
        int i =0;
        int j =0;
        int i_start=0;
        int i_end=0;
        int cmnd_count =0;
        char* cmnd;
        while(this->input[i]!= '\0')
        {
            if(this->input[i] == '\n')
            {
                cmnd = (char*)malloc(i_end - i_start + 1);
                memset(cmnd, 0 , i_end - i_start + 1);
                strncpy(cmnd, &this->input[i_start], i_end - i_start + 1);
                this->commands->add(this->commands, cmnd);
                free(cmnd);
                i_start = i_end + 1;
                j++;
            }
            i++;
        }
        cmnd = (char*)malloc(i_end-i_start + 1);
        memset(cmnd, 0 , i_end - i_start + 1);
        strncpy(cmnd, &this->input[i_start], i_end - i_start + 1);
        this->commands->add(this->commands, cmnd);
}

void    shell_parse_args(Shell* this, char* command)
{
        this->arguments = new_ll();
        int i_start= 0;
        int i_end = 0;
        int j = 0;
        char* arg;
        while(command[i_end] != '\0')
        {
            if(command[i_end] == ' ' || command[i_end] == '\0')
            {
                arg = (char*)malloc(i_end - i_start + 1);
                memset(arg, 0, i_end - i_start + 1);
                strncpy(arg, &command[i_start], i_end - i_start);
                this->arguments->add(this->arguments, arg);
                i_start = i_end + 1;
                j++;
            }
        }
        helper_sub_env_vars(this); // Sub in env vars if any
}

void    shell_parse_input(Shell* this)
{
        this->parse_commands(this);

}


int     shell_listen(Shell* this)
{
        pid_t pid;
        int status;
        int read_ret;
        int input_size;
        char* path;
        struct stat st;
        while(this->process)
        {
            read_ret = this->read_input(this); if (read_ret == 0) return 0;
            this->parse_commands(this);
            Node* curr_cmnd = this->commands->head;
            while(curr_cmnd)
            {
                this->parse_args(this, curr_cmnd->value);
                path = helper_read_path(helper_find_path_in_env(this), this->arguments->head->value);
                this->arguments->destroy(this->arguments);
                curr_cmnd = curr_cmnd->next;
            }
            this->commands->destroy(this->commands);
        }
}
