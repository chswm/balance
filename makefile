all	:	b cli ser

b	:	balance.o
	g++ -o b balance.o -lpthread
cli	:	cli.o
	g++ -o cli cli.o
ser	:	ser.o
	g++ -o ser ser.o -lpthread



balance.o	:	balance.cpp
	g++ -c balance.cpp
cli.o	:	cli.cpp
	g++ -c cli.cpp 
ser.o	:	ser.cpp
	g++ -c ser.cpp

clean	:
	rm cli.o balance.o ser.o
