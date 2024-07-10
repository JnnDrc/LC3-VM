
# Hex Compiler

This small program compiles a plain text file with
hex numbers, to a raw file with these hex values

## use

for use it you just need a plain text(or any file you want)
with ASCII character representing hex values, this input
should be specified as the first argument.  

Expected argument set : hexc \[path_to_input\](OPTIONAL\)\[path_to_output\]  

if the output file is not specified, the output will be
\[input_name_without_extension.o\]  

`OBS: the max hex value supported is 0xFFFF`

## Syntax

the syntax for the compiler has only two things,s
hex numbers and comments, a hex number should be
written without the '0x' prefix.  
comments can be written in 2 ways, side and line,
side comments are written with at least one space
between the value and the comment, and doesn't
need any special notation, line comments are written
placing a '#' at the first character of the line,
this turns the entire line a comment

```htx
#i'm a comment line  
F030 i'm a side comment
#
EE3A
```

`OBS: the max size of a line is 256`

## Example

```htx
# program  
5020    # AND R0 R0 x0  
E002    # LEA R0 2  
F022    # TRP x22  
F025    # HLT  
#memory  
0048    # H  
0065    # e  
006C    # l  
006C    # l  
006F    # o  
002C    # ,  
0020    #  
0057    # W  
006F    # o  
0072    # r  
006C    # l  
0064    # d  
0021    # !  
000A    # \n  
0000    # \0  
```

`OBS: the .htx format, used here, stands for hex text, it's just for not be confused with normal .txt files,
any plain/raw text file will work correctly`
