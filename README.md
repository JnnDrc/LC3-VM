
# LC-3 Virtual Machine

This project is a virtual machine for the LC-3 architecture,
this was made/inspired in these articles  
[by Andrei Ciobanu](https://www.andreinc.net/2021/12/01/writing-a-simple-vm-in-less-than-125-lines-of-c#virtual-machines)  
[by Justin Meiners & Ryan Pendleton](https://www.jmeiners.com/lc3-vm/)

## Compile

Just type `make` in console and input.
if you want to use gdb, use `make gdb`

## Run

To run a program in VM type `lc3-vm [program] [--optional_params]`
the program is a bin file, generally .o/.obj.
There are some example programs in _programs_ or
You can use hexc to compile your own programs.

The optional params are listed in the VM's manual
or in typing `lc3-vm --help`.

## Manuals

There's some manuals in the docs folder

* [VMLC3.md](docs\VMLC3.md) : VM's manual in english
* [VMLC3-PT.md](docs\VMLC3-PT.md) : VM's manual in portuguese
* [HEXC.md](docs\HEXC.md) : hexc's manual (in english)

## Known issues

none _currently_
