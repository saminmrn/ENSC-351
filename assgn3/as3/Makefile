# Edit this file to compile extra C files into their own programs.
SOURCES =buttons.c LEDs.c joystick.c audioMixer.c PlaySoundHandler.c BeatBox.c intervalTimer.c Printing.c main.c 
TARGET = beatbox

PUBDIR = $(HOME)/cmpt433/public/myApps/
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB
CFLAGS = -g -std=c99 -D _POSIX_C_SOURCE=200809L -Wshadow
# CFLAGS +=  -Werror
# -pg for supporting gprof profiling.
# CFLAGS += -pg

all: $(SOURCES) 
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET)  $(LFLAGS) -lpthread -lasound

clean:
	rm -f $(OUTDIR)/$(TARGET)

# Copy the sound files to public directory.
wav:
	mkdir -p $(PUBDIR)/beatbox-wav-files/
	cp -r beatbox-wav-files/* $(PUBDIR)/beatbox-wav-files/