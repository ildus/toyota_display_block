ARDUINO_DIR = /home/ildus/Arduino/arduino-1.6.12

BOARD_TAG    = atmegang
BOARD_SUB    = atmega8
ARDUINO_LIBS = StandardCplusplus
ARDUINO_SKETCHBOOK = ${ARDUINO_DIR}
CXXFLAGS_STD = -std=c++11
#AVRDUDE_ARD_BAUDRATE = 9600
#MONITOR_BAUDRATE = 9600
BOARDS_TXT = boards.txt

include /usr/share/arduino/Arduino.mk
