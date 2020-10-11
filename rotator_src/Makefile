MCU=atmega324p
CPP=avr-g++
CC=avr-gcc
OBJCOPY=avr-objcopy
PROJECT=rotator
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues -std=c99 -Wl,-u,vfprintf  -Wl,-u,vfscanf  -lprintf_flt -lscanf_flt -lm
DUDE=avrdude
SUDO=sudo

$(PROJECT).hex : $(PROJECT).elf
	$(OBJCOPY) -R .eeprom -O ihex $(PROJECT).elf $(PROJECT).hex

$(PROJECT).elf : $(PROJECT).o usart.o as5040.o l298n.o
	$(CC) $(CFLAGS) -Wl,-Map=rotator.map $^ -o $@

$(PROJECT).o : $(PROJECT).c
	$(CC) $(CFLAGS) -Os -c $^ -o $@

usart.o : usart.c
	$(CC) $(CFLAGS) -Os -c $^ -o $@

as5040.o : as5040.c
	$(CC) $(CFLAGS) -Os -c $^ -o $@

l298n.o : l298n.c
	$(CC) $(CFLAGS) -Os -c $^ -o $@

load : $(PROJECT).hex
	$(SUDO) $(DUDE) -F -V -c dragon_isp -p m324p -P usb -U flash:w:$^ -B 10

fuse :
	$(DUDE) -c dragon_isp -P usb -p m32 -U lfuse:w:0xff:m -B 10

clean :
	rm *.o *.hex *.elf *.map

