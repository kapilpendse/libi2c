DEBUG_MA = N

ifeq ($(DEBUG_MA), Y)
	CFLAGS += -g -DDEBUG
	CC = gcc
	AR = ar
	STRIP =
else
	CFLAGS += -O2
	CC = arm-none-linux-gnueabi-gcc
	AR = arm-none-linux-gnueabi-ar
	STRIP = arm-none-linux-gnueabi-strip
endif

CFLAGS += -Wall -std=c99

LIB = libi2c.a

$(LIB): libi2c.o
	$(AR) -rcv $(LIB) *.o

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(LIB)
	cp $(LIB) ../../build/lib
	mkdir -p ../../build/include/libi2c
	cp libi2c.h ../../build/include/libi2c

clean:
	@rm -f $(LIB)
	@find . \( -name '*.[oas]' -o -name '*.ko' -o -name '.*.cmd' -o -name '*~' \
        -o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \) \
        -type f -print | xargs rm -f

