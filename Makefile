CPP=g++

INCLUDES= -I/usr/include/fltk-1 -I/usr/local/include/FLU -I/usr/include/png++
LIBDIR=-L/usr/lib64/fltk-1 -L/usr/lib64 -L/usr/local/lib
LDFLAGS=-pipe -Wall
LDLIBS=-lXft -lpthread -lm -lXext -lX11 -lflu -lfltk -lfltk_images -lpng -lz -ljpeg

SOURCES = #imgsav.cpp
OBJECTS = $(SOURCES:.cpp=.o)

RTDGUI = rtdgui

all: $(RTDGUI)


$(RTDGUI): $(OBJECTS)
	$(CPP) $(INCLUDES) $(LIBDIR) $(LDFLAGS) -o rtdgui rtdgui.cpp $(LDLIBS)

.cpp.o: 
	$(CPP) $(INCLUDES) -o $@ -c $< 

clean:
	rm rtdgui *.o
