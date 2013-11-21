#############################################################
# friso chinese word segmentation makefile.		    #
#		do not use it for commercial use.	    #
# @author	chenxin 				    #
# @email	chenxin619315@gmail.com   		    #
#############################################################

#complie
CC = gcc
#include directory
INCLUDE = .
#complie flags for devolep
CFLAGS = -g -Wall
#complile flags for products
FFLAGS = -O2 -Wall -fPIC

all: ltpro.c ltpro.h tst-pro.c
	$(CC) $(CFLAGS) ltpro.c tst-pro.c

#clean all the object files.
.PHONY: clean
clean:
	find . -name \*.out  | xargs rm -f
