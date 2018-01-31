CFLAGS = -Wall -g
CPPFLAGS = $(shell pkg-config --cflags gtk+-2.0 cairo)
LDLIBS = $(shell pkg-config --libs gtk+-2.0 cairo)
LDFLAGS = -g

target = sproing
objs = sproing.o

$(target) : $(objs)

clean :
	rm $(target) $(objs)
