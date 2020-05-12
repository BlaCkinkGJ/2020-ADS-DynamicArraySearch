CC=C:\Program Files (x86)\CodeBlocks\MinGW\bin\x86_64-w64-mingw32-gcc.exe
CFLAGS=-g -Wall -Werror
# LDFLAGS=
# LDLIBS=
SRCS=main.c dynamic-array.c
OBJS=main.o dynamic-array.o
TARGET=a

all: $(TARGET)

clean:
	del *.o 
	del $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)