CC = gcc
CPP = g++
COPTS = -Wall -O2


all: primo.o


primo.o: primo.cpp
	$(CPP) $(COPTS) -o $@ -c $^


fx2_firmware.o: fx2_firmware.c
	$(CC) $(COPTS) -o $@ -c $^


fx2_firmware.c:
	@echo "#include <stdio.h>  /* For NULL... */" > fx2_firmware.c
	@echo >> fx2_firmware.c
	@echo "const char *_binary_nsl_fx2_firmware_ihx_start[]={" >> fx2_firmware.c
	@bash -c 'while read line ; do echo -e "\t\"$$line\"," >> fx2_firmware.c ; done < fx2_firmware.ihx'
	@echo "\tNULL" >> fx2_firmware.c
	@echo "};" >> fx2_firmware.c
	@echo "Created fx2_firmware.c"

clean:
	rm -fr *.o fx2_firmware.c *~