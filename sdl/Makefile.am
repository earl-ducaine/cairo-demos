bin_PROGRAMS=demo
demo_SOURCES=main.c

#  uncomment the following if sdl requires the math library
demo_LDFLAGS=`sdl-config --libs` `pkg-config --libs cairo`  -lsvg -lsvg-cairo
AM_CFLAGS = `sdl-config --cflags` `pkg-config --cflags cairo` -I/usr/local/include

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk

