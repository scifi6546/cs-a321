/************************
 * Nick Alexeev         *
 * Edward Hazelton      *
 * Prof. C. Lauter      *
 * CSCE A321            *
 * Assignment #1        *
 * Due: July 9th, 2020  *
 ************************/
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
int return_code=1;
#include "my_stdlib.h"

int binarySearch(char arr[], int start_line, int stop_line, int target);

int getPrefix(char arr[], int location);

int main(int argc, char **argv) {
    int fd, target, target_location;
    char *file, *file_path, *cityname;
    off_t size;
    if (argc < 3) {
        error("Too few arguments");
        return 1;
    }
    // Get file path
    file_path = argv[1];
    if (file_path[0] != '/') {
        error("File path needs to start_line with a slash (/)\n");
        return 1;
    }
    // Get the target phone number prefix
    target = to_int(argv[2]);
    if (target <= 0 || target > 999999) {
        error("Target phone number prefix is not a 6-digit number\n");
        return 1;
    }
    // Try to open the file
    fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        error("Could not open up file");
	error(file_path);
	error("\n");
        return 1;
    }
    // Get the size of the file
    size = lseek(fd, 0, SEEK_END);
    if (size == (off_t) -1) {
        error("Could not seek to the end of the file");
        return 1;
    }
    // Try to map the file to memory
    file = (char *) mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
    if (file == MAP_FAILED) {
        error("Could not map the file into memory\n");
        return 1;
    }
    // Do the binary search for the target prefix
    target_location = binarySearch(file, 0, size / 32, target);
    if (target_location < 0) {
        error("Could not find that prefix.\n");
        return 1;
    }
    // Quick checking if it worked.
    cityname = file + (target_location<<5) + 6;
    print(cityname);
    if (munmap(file, size) < 0) {
        error("Could not unmap the file from memory: ");
        error(strerror(errno));
        error("\n");
        return 1;
    }
    if (close(fd) < 0) {
	error("cound not close file: ");
        error(strerror(errno));
        error("\n");
        return 1;
    }
    munmap(file,size);
    close(fd);
    return 0;
}

int binarySearch(char arr[], int start_line, int stop_line, int target) {
    int center, center_prefix;
    if (stop_line > start_line) {
        center = (start_line + stop_line)/2;
        center_prefix = getPrefix(arr, center);
        if (center_prefix == target) {
            return center;
        }
        if (center_prefix < target) {
            return binarySearch(arr, center+1, stop_line, target);
        }
        return binarySearch(arr, start_line,center, target);
    }
    return -1;
}

int getPrefix(char arr[], int location) {
    char str[7];
    for (int i = 0; i < 6; i++) {
        str[i] = arr[i + (location*32)];
    }
    str[6] = '\0';
    return to_int(str);
}
