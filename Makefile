CPP=g++

INCLUDES= -I/usr/include/fltk-1 -I/usr/local/include/FLU -I/usr/include/png++
LIBDIR=-L/usr/lib64/fltk-1 -L/usr/lib64 -L/usr/local/lib
LDFLAGS=-pipe -Wall
LDLIBS=-lXft -lpthread -lm -lXext -lX11 -lflu -lfltk -lfltk_images -lpng -lz -ljpeg

SOURCES = #imgsav.cpp
OBJECTS = $(SOURCES:.cpp=.o)

RTDGUI = rtdgui
RTDGUI_TM1_HF = rtdgui_TM1_HF

all: $(RTDGUI) $(RTDGUI_TM1_HF)


$(RTDGUI): $(OBJECTS)
	$(CPP) $(INCLUDES) $(LIBDIR) $(LDFLAGS) -o rtdgui rtdgui.cpp $(LDLIBS)

$(RTDGUI_TM1_HF): $(OBJECTS)
	$(CPP) $(INCLUDES) $(LIBDIR) $(LDFLAGS) -o rtdgui_TM1_HF rtdgui_TM1_HF.cpp $(LDLIBS)


.cpp.o: 
	$(CPP) $(INCLUDES) -o $@ -c $< 

clean:
	rm rtdgui rtdgui_TM1_HF *.o
