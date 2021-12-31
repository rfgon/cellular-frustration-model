PROG = main.out
CC = g++
CPPFLAGS = -std=c++14 -Wall -O2
OBJS = main.o
SRC_DIR = src/

$(PROG): $(OBJS)
	$(CC) $(OBJS) -o $(PROG)

main.o:
	$(CC) $(CPPFLAGS) -c $(SRC_DIR)main.cpp

clean:
	rm $(PROG) $(OBJS)
