#include "../Shell/Shell.h"

void    helper_set_prompt(Shell* this, char* prompt)
{
    memset(this->prompt, 0, MAX_BUFFER);
    this->prompt[0] = '[';
    strcpy(&this->prompt[1], prompt);
    strcat(this->prompt, "]>");
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
                if(!var) return;
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
            if(path[i] == ':' || path[i] == '\0')
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

        memset(this->temp_buffer,0,MAX_BUFFER);
        getcwd(this->temp_buffer, MAX_BUFFER);
        printf("%s\n", this->temp_buffer);
}

void    helper_exec_cd(Shell* this)
{
        Node* arg1 = this->arguments->head->next;
        if(!arg1|| strcmp(arg1->value, "~") == 0)
        {
            
            getcwd(this->cwd_buffer, MAX_BUFFER);
            chdir(getenv("HOME"));
            helper_set_prompt(this, getenv("HOME"));

        }
        else if(strcmp(arg1->value, "-") == 0)
        {
            memset(this->temp_buffer, 0, MAX_BUFFER);
            getcwd(this->temp_buffer, MAX_BUFFER);
            chdir(this->cwd_buffer);
            strcpy(this->cwd_buffer, this->temp_buffer);

            memset(this->temp_buffer, 0 , MAX_BUFFER);
            helper_set_prompt(this, getcwd(this->temp_buffer, MAX_BUFFER));

        }
        else
        {   memset(this->temp_buffer, 0, MAX_BUFFER);
            memset(this->cwd_buffer, 0, MAX_BUFFER);
            getcwd(this->cwd_buffer, MAX_BUFFER);
             if(chdir(arg1->value) < 0)
            {
                fprintf(stderr, "Can't find directory\n");
            }
            else
            {
                memset(this->temp_buffer, 0 , MAX_BUFFER);
                helper_set_prompt(this, getcwd(this->temp_buffer, MAX_BUFFER));
            }
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
        char* builtins[9] = {
                                "quit", 
                                "cd", 
                                "setenv", 
                                "unsetenv",
                                "pwd",
                                "which",
                                "env",
                                "echo",
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

void    helper_exec_echo(Shell* this)
{
        printf("%s\n", this->arguments->head->next->value);
}

char*   helper_copy_string(char* src)
{
            char* dest = (char*)malloc(strlen(src) +1);
            memset(dest, 0, strlen(src) +1);
            strcpy(dest, src);
            return dest;
}
