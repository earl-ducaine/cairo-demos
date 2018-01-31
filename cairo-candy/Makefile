APP_1=cairo-clock
APP_2=wobbly-zini
CC=cc

CFLAGS= -Wall -Os `pkg-config --cflags gtk+-2.0 librsvg-2.0`
LDFLAGS= `pkg-config --libs gtk+-2.0 librsvg-2.0`

SRC_1 = cairo-clock.c
SRC_2 = wobbly-zini.c

OBJ_1 = $(SRC_1:.c=.o)
OBJ_2 = $(SRC_2:.c=.o)

all: $(APP_1) $(APP_2)

$(APP_1):  $(OBJ_1)
	$(CC) $(LDFLAGS) $(OBJ_1) -o $(APP_1)
	strip $(APP_1)

$(APP_2):  $(OBJ_2)
	$(CC) $(LDFLAGS) $(OBJ_2) -o $(APP_2)
	strip $(APP_2)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_1) $(OBJ_2) *~ $(APP_1) $(APP_2)

