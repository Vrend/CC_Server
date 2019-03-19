TARGET = server

CC = g++

CFLAGS = -g -Wall -pthread

DEPS = server.h local_functions.h dependencies.h structs.h constants.h

OBJ = server.o local_functions.o

all: $(TARGET)

%o: %cpp $(DEPS)
		$(CC) -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^

clean:
		$(RM) $(TARGET) *.o
