# General Purpose C++ Makefile

TARGET := driver
CC	:= g++
CFLAGS	:= -Wall -std=c++11
LFLAGS	:= -lm

CPP_SRCS := $(shell find * -type f -name '*.cpp')
HDRS	:= $(shell find * -type f -name '*.h')

$(TARGET): OBSdata.cpp
	$(CC) $(CPP_SRCS) -o $(TARGET) $(LFLAGS)

clean:
	rm -f $(TARGET)

run:
	./$(TARGET) TestData/dataExample.obs
