APPS    = cairo-fb

CFLAGS  = -Wall
LDFLAGS = -lm

CFLAGS  += `pkg-config cairo --cflags`
LDFLAGS += `pkg-config cairo --libs`

all: $(APPS) 

clean:
	rm -f $(APPS)
