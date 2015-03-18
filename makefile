CC=g++
HEADERS=lexer.hpp includes.hpp parser.hpp
OBJ=lexer.o parser.o main.o
CFLAGS=-g
all:main.bin clean
	
main.bin:$(OBJ)
		$(CC) $(OBJ) -o $@ $(CFLAGS)

%.o:%.cpp $(HEADERS)
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o
