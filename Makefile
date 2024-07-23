CC = gcc


lc3-vm:	src\vm.c
	$(CC) $< -o $@

.PHONY: gdb
gdb: src\vm.c
	$(CC) $< -o lc3-vm_gdb -g
