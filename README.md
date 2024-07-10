# Duplicate File Finder

This is a C program designed to find and manage duplicate files in a directory. It can identify duplicates based on either file names or file sizes. The program provides an option to delete duplicates and displays progress as files are processed.

## Features

- **Find duplicates by name**: Identifies files with the same name in different directories.
- **Find duplicates by size**: Identifies files with the same size and name in different directories.
- **Prompt for deletion**: Allows the user to delete one or both of the duplicate files.
- **Progress bar**: Displays a progress bar to indicate the percentage of file comparisons completed.

## Dependencies

- C standard library (stdlib, string, dirent, sys/stat, unistd, time)

## Compilation

To compile the program, use the following command:

gcc -o duplicate_finder duplicate_finder.c -std=c99 [This option is required to supress any warnings or errors at compile time!]
Usage
To run the program, use the following command at the terminal:

./duplicate_finder <directory> <N|S>
Where:

<directory>: The path to the directory you want to scan for duplicates (relative from current folder or absolute path).
<N|S>: Option to determine the method of finding duplicates:
N - Find duplicates by name.
S - Find duplicates by size.

## Example
./duplicate_finder /path/to/directory N or


./duplicate_finder /path/to/directory N or ./duplicate_finder /path/to/directory S
