program: client.c writer.c logging.c
		gcc -o client client.c -I -Wall -pedantic
		gcc -o writer writer.c -I -Wall -pedantic -lpthread