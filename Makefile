CC=/usr/local/arm-linux-4.1.1/bin/arm-linux-gcc -lpthread -lm

CFLAGS+= -mcpu=iwmmxt -mtune=iwmmxt -mabi=aapcs-linux  \
		-Iinclude/ -DDEBUG_BUILD

LFLAGS+=-Llib/ -lpxadev 

.PHONY: all compile install-host install-target clean clean-local \
	uninstall-host uninstall-target

all: compile install-host install-target 

compile: lib/libpxadev.so bin/main

lib/libpxadev.so: driver/camera.o driver/overlay2.o
	$(CC) $(CFLAGS) -shared -W1,-soname,libpxadev.so -o lib/libpxadev.so $^

bin/main: main.c
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^

install-host:

install-target:

clean: clean-local uninstall-host uninstall-target

clean-local:
	-rm -f lib/* driver/*.o
	-rm -f bin/*

uninstall-host:
	-rm -f $(PXA_HOST_LIB_DIR)/libpxadev.so

uninstall-target:
	$(SUDO) rm -f $(PXA_TARGET_BIN_DIR)/camera
	$(SUDO) rm -f $(PXA_TARGET_LIB_DIR)/libpxadev.so
