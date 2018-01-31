WARN_FLAGS=-g -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls
CFLAGS=${WARN_FLAGS} `pkg-config --cflags cairo`
LDFLAGS=${WARN_FLAGS} `pkg-config --libs cairo`

all: fdclock

fdface.o:  fdface.c fdface.h

fdhand.o: fdhand.c fdhand.h

fdlogo.o: fdlogo.c fdlogo.h

fdclock: main.o fdface.o fdhand.o fdlogo.o
	${CC} ${LDFLAGS} -o $@ $^ -framework Carbon

clean:
	rm -f fdclock *.o
