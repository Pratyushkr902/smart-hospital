# Makefile for Smart Hospital Management System (C & C++20)

CC = clang
CXX = clang++

CFLAGS = -Wall -Wextra -O2
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

C_SRCS = c_ds/heap.c c_ds/linked_list.c c_ds/hash_table.c
C_OBJS = $(C_SRCS:.c=.o)

CPP_SRCS = main.cpp
CPP_OBJS = $(CPP_SRCS:.cpp=.o)

TARGET = smart_hospital_pbl

all: $(TARGET)

c_ds/%.o: c_ds/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I. -c $< -o $@

$(TARGET): $(C_OBJS) $(CPP_OBJS)
	$(CXX) $(CXXFLAGS) $(C_OBJS) $(CPP_OBJS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) c_ds/*.o *.o

.PHONY: all run clean
