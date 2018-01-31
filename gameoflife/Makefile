APPS = cgol

OBJECTS = cgol.o ctk.o main.o cgolwin.o
HEADERS = ctk.h cgolwin.h cgol.h

CXXFLAGS  = -g -Wall

CXXFLAGS  += `pkg-config cairo --cflags`
LDFLAGS += `pkg-config cairo --libs`

all: $(APPS)

$(OBJECTS): $(HEADERS)
$(APPS): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	$(RM) *.o $(APPS)
