all: client_pg3.o server_pg3.o iofunctions_pg3.o
	gcc -o client_pg3 client_pg3.o iofunctions_pg3.o
	gcc -o server_pg3 server_pg3.o iofunctions_pg3.o

iofunctions_pg3.o: iofunctions_pg3.c
	gcc -c iofunctions_pg3.c

client_pg3.o: client_pg3.c
	gcc -c client_pg3.c

server_pg3.o: server_pg3.c
	gcc -c server_pg3.c
	
clean:
	rm server_pg3 client_pg3 *.o