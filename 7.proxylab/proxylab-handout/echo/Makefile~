CC = gcc
CFLAGS = -O2 -g

LIB = -lpthread

all: echo client

echo: echo.c csapp.o
	$(CC) $(CFLAGS) -o echo echo.c csapp.o $(LIB)

csapp.o: csapp.c csapp.h 
	$(CC) $(CFLAGS) -c csapp.c

client: client.c csapp.o
	$(CC) $(CFLAGS) -o client client.c csapp.o $(LIB)

clean:
	rm -f *.o echo *~

