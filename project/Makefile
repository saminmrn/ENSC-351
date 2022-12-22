SOURCES = main.c oled.c example_app.c SSD1306_OLED.c I2C.c heartrate.c accel.c audioCommand.c buttons.c joystick.c main_timer.c timer.c server.c
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc
CFLAGS = -g -std=gnu99 -D _POSIX_C_SOURCE=200809L 
all: beatbox 
beatbox:
		$(CC_C) $(CFLAGS) $(SOURCES) $(LFLAGS) -pthread -lm 
clean:
		rm -f 