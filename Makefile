CC=gcc
LIB=-pthread

server1: server1.o
	$(CC) server1.o -o server1 $(LIB)

server1.o: server1.c
	$(CC) -c server1.c $(LIB)

server2: server2.o
	$(CC) server2.o -o server2 $(LIB)

server2.o: server2.c
	$(CC) -c server2.c $(LIB)

client1: client1.o
	$(CC) client1.o -o client1 $(LIB)

client1.o: client1.c
	$(CC) -c client1.c $(LIB)
	
client2: client2.o
	$(CC) client2.o -o client2 $(LIB)

client2.o: client2.c
	$(CC) -c client2.c $(LIB)

all: server1 server2 client1 client2 
 
clean:
	rm -f *.o server1 server2 client1 client2 

