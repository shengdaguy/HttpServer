CC = gcc
CXX = g++
LIB = -lstdc++
CFLAGS = -g -Wall -lpthread
SO = -fPIC -shared

http_service.so : http_service.c http_service.h http_data.h
	g++ http_service.c $(SO) -o http_service.so

clean: 
	rm  *.so

