#include <stdio.h>
#include <unistd.h>

int main()
{
    execl("/bin/sh", "-c", "cd", (const char *)0);
}