PROJ = my_zsh
CC = gcc
CFLAGS = -Wall -Wextra -Werror
SOURCES = main.c Node/Node.c LinkedList/LinkedList.c HelperFns/HelperFns.c Shell/Shell.c


$(PROJ): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

fclean:
	rm $(PROJ)