# libdbus-1-dev
DBUS1_CFLAGS := `pkg-config --cflags dbus-1`
DBUS1_LIBS := `pkg-config --libs dbus-1`

# libglib2.0-dev
GLIB_CFLAGS := `pkg-config --cflags glib-2.0`
GLIB_LIBS := `pkg-config --libs glib-2.0`

CFLAGS := $(DBUS1_CFLAGS) $(GLIB_CFLAGS) -g
LIBS := $(DBUS1_LIBS) $(GLIB_LIBS)

TARGET := gsettings-monitor
OBJS := main.o gsettings_blob.o monitor.o proc.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

main.o: gsettings_blob.h monitor.h proc.h
gsettings_blob.o: gsettings_blob.h
monitor.o: monitor.h
proc.o: proc.h

clean:
	rm -f *.o