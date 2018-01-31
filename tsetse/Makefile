APPS    = tsetse

CFLAGS  = -Wall
LDFLAGS = -lm

CFLAGS  += `pkg-config cairo --cflags`
LDFLAGS += `pkg-config cairo --libs`

all: $(APPS) 

clean:
	$(RM) $(APPS)
