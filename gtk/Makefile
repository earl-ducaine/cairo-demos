APPS =				\
	hello			\
	mouse_events

CFLAGS  = -g -Wall

CFLAGS  += `pkg-config gtk+-2.0 --cflags`
LDFLAGS += `pkg-config gtk+-2.0 --libs`

all: $(APPS)

clean:
	$(RM): $(APPS)
