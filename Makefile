################################################################################
###                       Makefile for Kalytera project                      ###
###                          Made by Hafid HOUSNI                            ###
################################################################################

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -O3 -g -D__NO_INLINE__
LDLIBS=-lSDL2 -lSDL2_image -lm `sdl2-config --cflags --libs`
LDFLAGS = -fsanitize=address
GTK=`pkg-config --cflags --libs gtk+-3.0` -export-dynamic


all: build

build: kalytera-ocr kalytera-solver Kalytera-XOR

kalytera-ocr: setup pixel_operations sobel preprocess binarize loader hough main-ocr
	$(CC) $(CFLAGS) $(GTK) bin/*.o $(LDLIBS) -o kalytera-ocr

kalytera-solver: setup
	$(CC) $(CFLAGS) src/solver/*.c -o solver
	
Kalytera-XOR : setup
	$(CC) $(CFLAGS) src/XOR/*.c -o xor

sobel:
	$(CC) $(CFLAGS) $(LDLIBS) -o bin/sobel.o -c src/sobel.c

preprocess: deskew noisereduction

deskew:
	$(CC) $(CFLAGS) $(LDLIBS) -o bin/deskew.o -c src/deskew.c

noisereduction:
	$(CC) $(CFLAGS) $(LDLIBS) -o bin/noisereduction.o -c src/noisereduction.c

binarize:
	$(CC) $(CFLAGS) $(LDLIBS) -o bin/binarize.o -c src/binarize.c

loader:
	$(CC) $(CFLAGS) $(LDLIBS) -o bin/loader.o -c src/loader.c

pixel_operations:
	$(CC) $(CFLAGS) $(LDLIBS) -o bin/pixel_operations.o -c src/pixel_operations.c

main-ocr:
	$(CC) $(CFLAGS) $(LDLIBS) $(GTK) -o bin/main.o -c src/main.c

hough:
	$(CC) $(CFLAGS) $(LDLIBS) $(GTK) -o bin/hough.o -c src/hough.c

setup : fix_libs setup_dirs


setup_dirs:
	mkdir -p tmp
	mkdir -p bin
	mkdir -p output

fix_libs:
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/run/current-system/sw/lib/

clean:
	rm kalytera-ocr
	rm solver
	rm -rf tmp
	rm -rf bin
	rm -rf output
