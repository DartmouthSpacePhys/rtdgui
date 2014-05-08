CPP=g++

INCLUDES= -I/usr/include/fltk-1 -I/usr/local/include/FLU
LIBDIR=-L/usr/lib64/fltk-1 -L/usr/lib64 -L/usr/local/lib
LDFLAGS=-pipe -Wall
LDLIBS=-lXft -lpthread -lm -lXext -lX11 -lflu -lfltk -lfltk_images -lpng -lz -ljpeg


all:
	$(CPP) $(INCLUDES) $(LIBDIR) $(LDFLAGS) -o rtdgui rtdgui.cpp $(LDLIBS)
