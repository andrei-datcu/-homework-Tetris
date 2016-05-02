# Completati va rog urmatoarele 3 linii cu informatiile voastre personale.
# Daca aveti mai multe prenume, alegeti doar unul dintre ele.
NUME=Datcu
PRENUME=Andrei
GRUPA=315CC
# ^ Doar cele trei de sus.

CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lcurses -lSDL_mixer -lSDL

.PHONY: build run examples pack clean

build:
	$(CC) $(CFLAGS) $(LDFLAGS) src/*.c -o bin/Tetris

run: build
	cp src/*.wav bin/
	cp src/savegame.dat bin/savegame.dat
	cd bin && ./Tetris

libs:
	sudo apt-get install libsdl1.2-dev
	sudo apt-get install libsdl-mixer1.2-dev

install: libs run

examples:
	$(CC) $(CFLAGS) $(LDFLAGS) example/exemplu1.c -o bin/exemplu1
	$(CC) $(CFLAGS) $(LDFLAGS) example/exemplu2.c -o bin/exemplu2

pack: clean
	zip -r $(GRUPA)_$(NUME)_$(PRENUME)_tema3.zip *

clean:
	rm -rf bin/* *.zip
	clear

