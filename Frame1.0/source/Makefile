CC = gcc
CXX = g++
LIB = -lstdc++
CFLAGS = -g -Wall -lpthread  -rdynamic
SO = -fPIC -shared
OBJECTS = server.o connpool.o threadpool.o config.o log.o io.o err.o util.o

server : $(OBJECTS)
	g++  -o server *.o $(CFLAGS) -ldl
server.o : server.c 
	g++ -c  server.c $(CFLAGS)
threadpool.o : threadpool.c  
	g++ -c threadpool.c
connpool.o : connpool.c 
	g++ -c connpool.c $(CFLAGS)
config.o : config.c
	g++ -c config.c $(CFLAGS)
log.o : log.c
	g++ -c log.c $(CFLAGS)
util.o : util.c
	g++ -c util.c $(CFLAGS)
err.o : err.c
	g++ -c err.c
io.o : io.c
	g++ -c io.c
clean: 
	rm server *.o 

