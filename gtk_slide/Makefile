SRCS   = $(wildcard *.c)
OBJS   = $(SRCS:.c=.o)

LIBS   +=`pkg-config --libs gtk+-2.0`
CFLAGS +=`pkg-config --cflags gtk+-2.0` -g
CC     = gcc

all: dep gtk_slide
gtk_slide: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
	@echo

clean:	
	rm -f *~ gtk_slide .depend *.o *.orig
dep: 
	$(MAKE) .depend
.depend: $(SRCS) *.c *.h
	$(CC) -MM $(CFLAGS) $(SRCS) 1>.depend
