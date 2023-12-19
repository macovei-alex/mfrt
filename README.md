# Trashy brainfuck-like language translator

This is my version of a translator for a custom programming language similar to brainfuck, which I will call frt ( from fart). The translator will be called mfrt.
This is meant to be a personal side project for fun and it's not meant for industrial use.
The translator is written in C for Windows platforms. It translates the brainfuck code into C code, then use gcc to compile the code.

## Compilation syntax:
The command 'make' is run from the terminal to build the translator. After that, a file with the '.bf' can be translated to C code using the 'mfrt' command. Compilation options:
1. '-v' verbose; the C code generated will be printed in the terminal at generation
2. '-r' run; runs the executable generated after compiling with gcc the C code generated with the translator

## Language description:
The first few lines of code will be generated no matter what. They look like this:

```C
#include <stdio.h>

#define max (ptr - array > highest ? ptr - array : highest)
#define min (ptr - array < lowest> ? ptr - array : lowest)

int main()
{
    int array[1024] = {0};
    int* ptr = array + 511;
    
    int lowest = ptr - array, highest = ptr - array;

```

The C code will then be generated based on the frt instructions. There is no way to declare variables, functions or anything of this kind.

## Language syntax:
1. '<' and '>' for moving around in the integer array to the left and right, respectively; this movement will update the 'lowest' and 'highest' variables if necessary using the macros defined above
2. '+' and '-' for manipulating the value at the current location
3. '_' for printing the integer at the current position as an integer (together with the newline character)
4. '.' for printing the integer at the current position as a character (without the newline character)
5. '@' for printing every integer between 'lowest' and 'highest' (every position that has been accessed at least once) as integers, spaced using a space and with a newline character at the end
6. '#' for printing every integer between 'lowest' and 'highest' (every position that has been accessed at least once) as characters, not spaced and without a newline chacarcter at the end
7. '\' for printing the newline character
8. ',' for reading an integer at the current position
9. '[' and ']' for defining a for loop
    - the '[' character will insert the beginning of the for loop
    - the ']' will terminat the loop
    - the first characters after the '[' are digits of the number for the for loop condition ('for(size_t i = 0; i < number; i++)')
    - after the number, the character '|' will be inserted to signify the begging of the code inside the for loop

## Requirements
To compile this program you need the MinGW compiler for C (and the make executable that should be included). You compile and link all the necessary files by running the make command inside the build directory. If the build is successful you shoudl see the mfrt.exe program in the bin directory.