#include "Shell.h"
#include <string.h>
#include <unistd.h>

void    shell_constructor(Shell* this)
{
        memset(this->input, 0, MAX_BUFFER);
        memset(this->cwd_buffer, 0, MAX_BUFFER);
        memset(this->temp_buffer, 0, MAX_BUFFER);
        memset(this->prompt, 0, MAX_BUFFER);
        this->prompt[0] = '[';
        getcwd(&this->prompt[1], MAX_BUFFER - 1);
        this->process = true;
}


Shell*  new_shell(char** env)
{
        Shell template =   {
                                    .init               = &shell_constructor,
                                    .read_input         = &shell_read_input,
                                    .listen             = &shell_listen,
                                    .parse_commands     = &shell_parse_commands,
                                    .parse_args         = &shell_parse_args,
                                    .execute_built_in   = &shell_execute_built_in,
                                    .execute_prog       = &shell_execute_prog,
                                    .destroy            = &shell_destroy

                                };
        Shell* result = malloc(sizeof(Shell));
        memcpy(result, &template, sizeof(Shell));
        result->envList = env;
        result->init(result);
        return result;
}

void    helper_construct_arg_list(Shell* this)
{
        Node* curr = this->arguments->head;
        int size = this->arguments->length;
        this->argList = (char**)malloc(sizeof(char*) * (size + 1));
        int i =0;
        while(curr)
        {
            this->argList[i] = (char*)malloc(strlen(curr->value) + 1);
            memset(this->argList[i], 0, strlen(curr->value) + 1);
            strcpy(this->argList[i], curr->value);
            curr = curr->next;
            i++;
        }
        this->argList[i] = NULL;
}

void    helper_destroy_arg_list(Shell* this)
{
        int i =0;
        while(this->argList[i])
        {
            free(this->argList[i]);
            i++;
        }
        free(this->argList);
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

char*   helper_find_path_in_env(Shell* this)
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

char*   helper_construct_env_string(Shell* this)
{
        int k =0;
        int len=0;
        while(this->envList[k])
        {
            len += strlen(this->envList[k]);
            k++;
        }

        char* env_str = (char*)malloc(len + k + 1);
        memset(env_str,0,len+k+1);
        k = 0;
        len = 0;
        while(this->envList[k])
        {
        
            strcat(env_str, this->envList[k]);
            strcat(env_str, "\n");
            k++;
        }
        return env_str;
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
                    memset(r_path, 0, j+strlen(cmnd) + 2);
                    strcpy(r_path, s_path);
                    strcat(r_path, "/");
                    strcat(r_path, cmnd);
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

void    helper_exec_pwd(Shell* this)
{
        char buffer[MAX_BUFFER];
        memset(buffer,0,MAX_BUFFER);
        getcwd(buffer, MAX_BUFFER);
        printf("%s\n", buffer);
}

void    helper_exec_cd(Shell* this)
{
        Node* arg1 = this->arguments->head->next;
        if(!arg1|| strcmp(arg1->value, "~") == 0)
        {
            
            getcwd(this->cwd_buffer, MAX_BUFFER);
            chdir(getenv("HOME"));
        }
        else if(strcmp(arg1->value, "-") == 0)
        {
            memset(this->temp_buffer, 0, MAX_BUFFER);
            getcwd(this->temp_buffer, MAX_BUFFER);
            chdir(this->cwd_buffer);
            strcpy(this->cwd_buffer, this->temp_buffer);

        }
        else
        {
            memset(this->cwd_buffer, 0, MAX_BUFFER);
            getcwd(this->cwd_buffer, MAX_BUFFER);
            chdir(arg1->value);
        }
}

void    helper_exec_setenv(Shell* this)
{
        int i =0;
        int var_len =0;
        Node* arg1 = this->arguments->head->next;
        while(arg1->value[i] != '=')
        {
            i++;
            var_len++;
        }
        int val_len = strlen(arg1->value) - var_len - 1;
        char* var_name = (char*)malloc(var_len + 1);
        char* var_val = (char*)malloc(val_len + 1);

        memset(var_name, 0, var_len + 1);
        memset(var_val, 0, val_len + 1);

        strncpy(var_name, &arg1->value[0], var_len);
        strncpy(var_val, &arg1->value[var_len + 1], val_len);

        setenv(var_name, var_val, 0);

        free(var_name);
        free(var_val);
}

void    helper_exec_unsetenv(Shell* this)
{
        unsetenv(this->arguments->head->next->value);
}

void    helper_exec_which(Shell* this)
{
        char* arg = this->arguments->head->next->value;
        char* builtins[8] = {
                                "quit", 
                                "cd", 
                                "setenv", 
                                "unsetenv",
                                "pwd",
                                "which",
                                "env",
                                NULL    
                            };
        int i =0;
        while(builtins[i])
        {
            if(strcmp(arg, builtins[i]) == 0)
            {
                printf("Built-in\n");
                return;
            }
            i++;
        }
        // if not builtin
        char* path = helper_read_path(helper_find_path_in_env(this), arg);
        printf("%s\n", path);
        free(path);
}

void    helper_exec_env(Shell* this)
{
        char* env_str = helper_construct_env_string(this);
        printf("%s\n", env_str);
        free(env_str);
}

bool    shell_execute_built_in(Shell* this)
{
        char* arg = this->arguments->head->value;
        if(strcmp(arg, "quit") == 0)
        {
            this->process = false;
            return true;
        }
        if(strcmp(arg, "cd") == 0)
        {
            helper_exec_cd(this);
            return true;
        }
        if(strcmp(arg, "setenv") == 0)
        {
            helper_exec_setenv(this);
            return true;
        }
        if(strcmp(arg, "unsetenv") == 0)
        {
            helper_exec_unsetenv(this);
            return true;
        }
        if(strcmp(arg, "pwd") == 0)
        {
            helper_exec_pwd(this);
            return true;
        }
        if(strcmp(arg, "which") == 0)
        {
            helper_exec_which(this);
            return true;
        }
        if(strcmp(arg, "env") == 0)
        {
            helper_exec_env(this);
            return true;
        }
        return false;
        
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
        int i_start=0;
        int i_end=0;
        int cmnd_count =0;
        char* cmnd;
        while(this->input[i_end]!= '\0')
        {
            if(this->input[i_end] == '\n')
            {
                cmnd = (char*)malloc(i_end - i_start + 1);
                memset(cmnd, 0 , i_end - i_start + 1);
                strncpy(cmnd, &this->input[i_start], i_end - i_start + 1);
                this->commands->add(this->commands, cmnd);
                free(cmnd);
                i_start = i_end + 1;
            }
            i_end++;
        }
        printf("i_end: %d... i_start: %d\n", i_end, i_start);
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
         
        while(true)
        {   
            if(command[i_end] == ' ' || command[i_end] == '\0')
            {
                arg = (char*)malloc(i_end - i_start + 1);
                memset(arg, 0, i_end - i_start + 1);
                strncpy(arg, &command[i_start], i_end - i_start);
                printf("ARG: %s\n", arg);
                this->arguments->add(this->arguments, arg);
                i_start = i_end + 1;
                j++;
            }
            if(command[i_end] == '\0')
            {
                break;
            }
            i_end++;

        }
        printf("HERE!\n");
        printf("ARG SIZE: %d\n", this->arguments->length);
        this->arguments->print(this->arguments);
        helper_sub_env_vars(this); // Sub in env vars if any
}

void    shell_execute_prog(Shell* this)
{
        char* cmnd = this->arguments->head->value;
        cmnd = (char*)realloc(cmnd, strlen(this->prog_path) + 1);
        memset(cmnd, 0,  strlen(this->prog_path) + 1);
        strcpy(cmnd, this->prog_path);
        helper_construct_arg_list(this);
        this->pid = fork();
        if(this->pid == 0)
        {
            if(execve(this->argList[0], this->argList, this->envList) == -1)
            {
                perror("lsh");
            }
            exit(EXIT_FAILURE);
        }else
        {
            do{
                
                waitpid(this->pid, &this->status, WUNTRACED);
            } while(!WIFEXITED(this->status) && !WIFSIGNALED(this->status));
            if(WIFSIGNALED(this->status))
            {
                psignal(WTERMSIG(this->status), "Exit signal");
            }
        }
        helper_destroy_arg_list(this);
}

int     shell_listen(Shell* this)
{
        pid_t pid;
        int status;
        int read_ret;
        int input_size;
        struct stat st;
        while(this->process)
        {
            read_ret = this->read_input(this); if (read_ret == 0) return 0;
            printf("You entered: %s\n", this->input);
            this->parse_commands(this);
            this->commands->print(this->commands);
            Node* curr_cmnd = this->commands->head;
            while(curr_cmnd)
            {
                this->parse_args(this, curr_cmnd->value);
                if(this->execute_built_in(this))
                {
                    curr_cmnd = curr_cmnd->next;
                    continue;
                } 
                this->prog_path = helper_read_path(helper_find_path_in_env(this), this->arguments->head->value);
                printf("PATH: %s\n", this->prog_path);
                this->execute_prog(this);
                this->arguments->destroy(this->arguments);
                free(this->prog_path);
                curr_cmnd = curr_cmnd->next;
            }
            this->commands->destroy(this->commands);
        }
}


void shell_destroy(Shell* this)
{

    free(this);
}

int main(int argc, char* argv[], char* env[])
{
    Shell* zsh = new_shell(env);
    zsh->listen(zsh);
    zsh->destroy(zsh);
}