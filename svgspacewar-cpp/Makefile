APPS = svgspacewar

CXXFLAGS  = -Os -Wall

CXXFLAGS  += `pkg-config gtk+-2.0 --cflags`
LDFLAGS += `pkg-config gtk+-2.0 --libs`

all: $(APPS)

clean:
	$(RM) $(APPS)
