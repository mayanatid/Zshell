#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    char *user = getenv("USER"),
        *hosttype = getenv("HOSTTYPE"),
        *hostname = getenv("HOSTNAME");
    printf("USER: %s\n", user ? user : "null");
    printf("HOSTTYPE: %s\n", hosttype ? hosttype : "null");
    printf("HOSTNAME: %s\n", hostname ? hostname : "null");

    setenv("USER", "namantam1", 1);
    printf("updated USER: %s\n", getenv("USER"));

    return 0;
}