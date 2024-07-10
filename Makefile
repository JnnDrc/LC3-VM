CC = gcc



vm:	src\vm.c
	$(CC) $< -o $@

vm_gdb: src\vm.c
	$(CC) $< -o $@ -g
