CC := gcc
CFLAGS := -Wall -Wextra


lc3-vm:	src/vm.c
	$(CC) $< -o $@

hexc: src/hexc/hexc.c
	$(CC) $< -o $@
