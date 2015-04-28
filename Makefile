# /********************************************************************************************
# 
# oooooooooo.   o8o                        oooooooooooo oooooooooooo  ooooooooooooo 
# `888'   `Y8b  `"'                        `888'     `8 `888'     `8  8'   888   `8 
#  888     888 oooo  ooo. .oo.    .oooo.    888          888               888      
#  888oooo888' `888  `888P"Y88b  `P  )88b   888oooo8     888oooo8          888      
#  888    `88b  888   888   888   .oP"888   888    "     888    "          888      
#  888    .88P  888   888   888  d8(  888   888          888               888      
# o888bood8P'  o888o o888o o888o `Y888""8o o888o        o888o             o888o
#                                                                
# Bina FFT: An AVX Intrinsics Based Radix-2 FFT Routine
# Copyright (C) 2012 - Ayan Shafqat, All rights reserved.
# 
# 
# The MIT License:
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
# 
# 	 /\_/\    <~~~~~~~~~~~~~~~>
# 	( o.o ) <      ENJOY!     |
# 	 > ^ <    ^~~~~~~~~~~~~~~~^
# 
# ********************************************************************************************/
CC = gcc
CFLAGS = -v -mavx -Os -c -Wall -std="c99" -I./
EXECUTABLE = BenchmarkBina
SOURCES = main.c bit_reverser.c BinaFFT.c 
OBJECTS = $(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)
	rm -rf $(EXECUTABLE)