CC=g++

compile : daemon client
daemon: daemon.o
	$(CC) -o daemon daemon.cpp
client: client.o
	$(CC) -o client client.cpp
daemon.o:daemon.cpp
	$(CC) -c daemon.cpp
client.o: daemon.cpp
	$(CC) -c client.cpp
clean :
	rm daemon client *.o
::
