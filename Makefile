CC = gcc



lc3-vm:	src\vm.c
	$(CC) $< -o $@

lc3-vm_gdb: src\vm.c
	$(CC) $< -o $@ -g
