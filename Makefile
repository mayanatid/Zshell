PROJ = my_zsh
CC = gcc
CFLAGS = -Wall -Wextra -Werror


$(PROJ): env_test.c
	$(CC) $(CFLAGS) -o $@ $^

fclean:
	rm $(PROJ)