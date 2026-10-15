# Root Makefile for Smart Hospital Management System

all: backend

backend:
	$(MAKE) -C backend

run:
	$(MAKE) -C backend run

frontend:
	npm run dev

clean:
	$(MAKE) -C backend clean

.PHONY: all backend run frontend clean
