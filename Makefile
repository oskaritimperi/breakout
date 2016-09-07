CC = gcc
RAYLIB = ../raylib
INCDIR = -I$(RAYLIB)/src
CFLAGS += -Wall $(INCDIR)
LDFLAGS += $(LIBDIR)
LIBRAYLIB = $(RAYLIB)/release/linux/libraylib.a
LIBGLFW3 = $(RAYLIB)/src/external/glfw3/lib/linux/libglfw3.a
LDLIBS += $(LIBRAYLIB) $(LIBGLFW3) -lm -ldl -lpthread -lX11 -lXrandr \
	-lXinerama -lXi -lXxf86vm -lXcursor

breakout: main.c timer.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean
clean:
	rm -f breakout
