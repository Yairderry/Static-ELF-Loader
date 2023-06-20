# Loader
This program is a static loader for running 32-bit ELF formatted executables without dynamic library code.

## Tasks
### Task 0: 
Implement an iterator over program headers in a 32-bit ELF executable file.

### Task 1: 
Print the information residing in the program headers of a 32-bit ELF file, mimicking the functionality of readelf -l.

### Task 2: 
Write the actual loader, mapping relevant segments of the executable into memory, and executing the loaded program.

How to Run:
Compile the loader program using the provided command:

`make`

Run the loader program with the path to a 32-bit ELF executable as the command line argument:

`./loader <executable_file> <arg1> <arg2> ...`

The loader will display the program headers' information, map the relevant segments to memory, and execute the loaded program.
