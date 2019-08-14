# Makefile for the Smart Battery refurbishment utils
# by Frank Rysanek <Frantisek.Rysanek@post.cz>

CC=gcc
# the -O2 optimization is necessary - the programs won't compile without it
CFLAGS=-O2

PROGS=read_batt reset-bq2092 reset-bq2040 bq2040_capacity eeprom
all: $(PROGS)

clean:
	rm -f $(PROGS)

tgz:
	rm -f ./i2c-progs.tgz
	tar cvzf i2c-progs.tgz *.c Makefile README.eeprom battery.sh help.txt schematic.txt
