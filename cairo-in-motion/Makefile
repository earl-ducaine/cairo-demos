APP=cairo-in-motion
CC=c++

#CFLAGS= -Wall -Os `pkg-config --cflags cairomm-1.0 libsvg-cairo gtkmm-2.4 libglademm-2.4`
CFLAGS= -Wall -g2 `pkg-config --cflags cairomm-1.0 libsvg-cairo gtkmm-2.4 libglademm-2.4`

LDFLAGS=	`pkg-config --libs cairomm-1.0 libsvg-cairo gtkmm-2.4 libglademm-2.4`

SRC = main.cpp \
	  cairo-samples.cpp \
	  sample-list-store.cpp

OBJ = $(SRC:.cpp=.o)

all: $(APP)

$(APP):  $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(APP)
	#strip $(APP)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) *~ $(APP)

