CFLAGS=-g -Wall $(COPTS)
TARGET=main
LDLIBS=-lpthread -lid3tag
CC=gcc
LIBS=tag.o functions.o sem.o

all : $(TARGET)

main: main.o $(LIBS)
#$(TARGETS): $(TARGETS).o mp3tag.o $(LIBS)
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
#%.o:%.c
#	$(CC) -c -o $@ $^  

clean: 
	rm -f $(TARGET) *.o core*
