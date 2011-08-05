CROSS ?= arm-elf-
CC = $(CROSS)gcc
LD = $(CROSS)ld
SRC = ../../../..
LOADADDR = 0x42000000 
TYPE = iBoot
MODEL = n18ap
OBJCOPY = $(CROSS)objcopy
OBJECTS = entry.o main.o common.o commands.o task.o lock.o bdev.o image.o nvram.o filesystem.o memory.o functions.o framebuffer.o sha.o
CFLAGS = -I. -I./.. -I./../.. -I./$(SRC)/include -nostdlib -mlittle-endian
LDFLAGS = -Ttext=$(LOADADDR) -nostdlib -lc -lm -lgcc

%.o: $(SRC)/%.S
	$(CC) -c $(<) -o $(@) $(INCLUDES) $(CFLAGS) 

%.o: $(SRC)/%.c
	$(CC) -c $(<) -o $(@) $(INCLUDES) $(CFLAGS) 

all: payload

payload: payload.elf

payload.elf: $(OBJECTS)
	$(CC) -o $(@) $(OBJECTS) $(LDFLAGS)
	$(OBJCOPY) -O binary $(@) $(MODEL).iboot.payload
	rm -rf *.elf
	rm -rf *.o

clean:
	rm -rf *.o *.elf $(MODEL).iboot.payload
	
