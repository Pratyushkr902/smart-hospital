# Root Makefile for Smart Hospital Management System

all: backend

backend:
	$(MAKE) -C backend

server:
	$(MAKE) -C backend server

run:
	$(MAKE) -C backend run

frontend:
	npm run dev

clean:
	$(MAKE) -C backend clean

.PHONY: all backend server run frontend clean
