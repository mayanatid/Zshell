#include "Shell.h"
#include <stdio.h>

void    shell_constructor(Shell* this)
{
        memset(this->input, 0, MAX_BUFFER);
        memset(this->cwd_buffer, 0, MAX_BUFFER);
        memset(this->temp_buffer, 0, MAX_BUFFER);
        memset(this->prompt, 0, MAX_BUFFER);
        helper_set_prompt(this, getcwd(this->temp_buffer, MAX_BUFFER));
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

void    shell_execute_prog(Shell* this)
{
        char* cmnd = this->arguments->head->value;
        cmnd = (char*)realloc(cmnd, strlen(this->prog_path) + 1);
        memset(cmnd, 0,  strlen(this->prog_path) + 1);
        strcpy(cmnd, this->prog_path);
        helper_construct_arg_list(this);
        // this->arguments->print(this->arguments);
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
        free(this->prog_path);
}

bool    shell_execute_built_in(Shell* this)
{
        char* arg = this->arguments->head->value;
        struct stat st;
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
        if(arg[0] == '.')
        {
            this->prog_path = helper_copy_string(arg);
            this->execute_prog(this);
            return true;
        }
        if(stat(this->input, &st) == 0 && st.st_mode & S_IXUSR)
        {
            this->prog_path = helper_copy_string(arg);
            this->execute_prog(this);
            return true;   
        }
        return false;
        
}

int     shell_read_input(Shell* this)
{
        memset(this->input, 0 , MAX_BUFFER);
        printf("%s", this->prompt);
        fflush(stdout);
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
        char* cmnd;
        while(this->input[i_end]!= '\0')
        {
            if(this->input[i_end] == '\n' || this->input[i_end] == ';')
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
        // printf("i_end: %d... i_start: %d\n", i_end, i_start);
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
                // printf("ARG: %s\n", arg);
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
        // printf("HERE!\n");
        // printf("ARG SIZE: %d\n", this->arguments->length);
        // this->arguments->print(this->arguments);
        helper_sub_env_vars(this); // Sub in env vars if any
}

int     shell_listen(Shell* this)
{
        int read_ret;
        while(this->process)
        {
            read_ret = this->read_input(this); if (read_ret == 0) return 0;
            // printf("You entered: %s\n", this->input);
            this->parse_commands(this);
            // this->commands->print(this->commands);
            Node* curr_cmnd = this->commands->head;
            while(curr_cmnd)
            {
                this->parse_args(this, curr_cmnd->value);
                if(this->execute_built_in(this))
                {
                    // printf("Built in\n");
                    curr_cmnd = curr_cmnd->next;
                    continue;
                } 
                this->prog_path = helper_read_path(helper_find_path_in_env(this), this->arguments->head->value);
                if(!this->prog_path) 
                {
                    fprintf(stderr, "Command Not Found!\n");
                    break;
                }
                // printf("PATH: %s\n", this->prog_path);
                this->execute_prog(this);
                this->arguments->destroy(this->arguments);
                curr_cmnd = curr_cmnd->next;
            }
            this->commands->destroy(this->commands);
        }
        return 1;
}

void shell_destroy(Shell* this)
{

    free(this);
}

