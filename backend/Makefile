# Makefile for Smart Hospital Management System (C & C++20)

CC = clang
CXX = clang++

CFLAGS = -Wall -Wextra -O2
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

C_SRCS = c_ds/heap.c c_ds/linked_list.c c_ds/hash_table.c
C_OBJS = $(C_SRCS:.c=.o)

PBL_TARGET = smart_hospital_pbl
SERVER_TARGET = smart_hospital_server

all: $(PBL_TARGET) $(SERVER_TARGET)

c_ds/%.o: c_ds/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I. -c $< -o $@

$(PBL_TARGET): $(C_OBJS) main.o
	$(CXX) $(CXXFLAGS) $(C_OBJS) main.o -o $(PBL_TARGET)

$(SERVER_TARGET): $(C_OBJS) server.o
	$(CXX) $(CXXFLAGS) $(C_OBJS) server.o -lsqlite3 -o $(SERVER_TARGET)

run: $(PBL_TARGET)
	./$(PBL_TARGET)

server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

clean:
	rm -f $(PBL_TARGET) $(SERVER_TARGET) c_ds/*.o *.o

.PHONY: all run server clean
