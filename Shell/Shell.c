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

int    shell_read_input(Shell* this)
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
                strncpy(cmnd, &this->input[i_start], i_end - i_start + 1);
                this->commands->add(this->commands, cmnd);
                free(cmnd);
                i_start = i_end + 1;
                j++;
            }
            i++;
        }
        cmnd = (char*)malloc(i_end-i_start+1);
        strncpy(cmnd, &this->input[i_start], i_end - i_start + 1);
        this->commands->add(this->commands, cmnd);
}

void    shell_parse_args(Shell* this)
{

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
        while(this->process)
        {
            read_ret = this->read_input(this); if (read_ret == 0) return 0;
            this->parse_commands(this);
            Node* temp = this->commands->head;
            while(temp)
            {

                temp = temp->next;
            }
        }
}
