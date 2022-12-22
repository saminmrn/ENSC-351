# Edit this file to compile extra C files into their own programs.
SOURCES = LEDs.c joystick.c sampling.c main.c 
SOURCES2 = noworky.c
TARGET = main
TARGET2= noworky

PUBDIR = $(HOME)/cmpt433/public/myApps/light_sampler
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Wshadow
# CFLAGS +=  -Werror
# -pg for supporting gprof profiling.
# CFLAGS += -pg

all: $(SOURCES) $(SOURCES2)
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET) -lpthread
	$(CC_C) $(CFLAGS) $(SOURCES2) -o $(OUTDIR)/$(TARGET2) -lpthread

clean:
	rm -f $(OUTDIR)/$(TARGET)
	rm -f $(OUTDIR)/$(TARGET2)