all:
	gcc server.c -o server -lpthread -lcurses
	gcc client.c -o client -lpthread -lcurses
