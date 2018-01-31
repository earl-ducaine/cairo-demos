MYCFLAGS= $(shell pkg-config --cflags cairo libpng) -Wall -g -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing
LDLIBS	= $(shell pkg-config --libs cairo libpng) -g

all: kapow

%.o:%.c
	$(CC) $(CFLAGS) $(MYCFLAGS) -c $^

kapow : kapow.o

clean :
	rm kapow.o kapow
