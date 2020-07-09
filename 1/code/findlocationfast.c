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

int binarySearch(char arr[], int start_line, int stop_line, int target);

int getPrefix(char arr[], int location);

int main(int argc, char **argv) {
    int fd, target, target_location;
    char *file, *file_path, *cityname;
    off_t size;

    if (argc < 3) {
        fprintf(stderr, "Too few arguments: %s <memory name>\n", argv[0]);
        return 1;
    }

    // Get file path
    file_path = argv[1];

    if (file_path[0] != '/') {
        fprintf(stderr, "File path needs to start_line with a slash (/)\n");
        return 1;
    }

    // Get the target phone number prefix
    target = atoi(argv[2]);

    if (target <= 0 || target > 999999) {
        fprintf(stderr, "Target phone number prefix is not a 6-digit number\n");
        return 1;
    }

    // Try to open the file
    fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "Could not open up file %s: %s\n", file_path, strerror(errno));
        return 1;
    }

    // Get the size of the file
    size = lseek(fd, 0, SEEK_END);

    if (size == (off_t) -1) {
        fprintf(stderr, "Could not seek to the end of the file: %s\n", strerror(errno));
        return 1;
    }

    // Try to map the file to memory
    file = (char *) mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);

    if (file == MAP_FAILED) {
        fprintf(stderr, "Could not map the file into memory: %s\n", strerror(errno));
        return 1;
    }

    // Do the binary search for the target prefix
    target_location = binarySearch(file, 0, size / 32, target);

    if (target_location < 0) {
        fprintf(stderr, "Could not find that prefix.\n");
        return 1;
    }




    // Quick checking if it worked.
    cityname = file + (target_location<<5) + 6;

    fprintf(stdout, "found: %s\n", cityname);






    if (munmap(file, size) < 0) {
        fprintf(stderr, "Could not unmap the file from memory: %s\n", strerror(errno));
        return 1;
    }

    if (close(fd) < 0) {
        fprintf(stderr, "Could not close the file: %s\n", strerror(errno));
        return 1;
    }
}

int binarySearch(char arr[], int start_line, int stop_line, int target) {

    int center, center_prefix;


    if (stop_line > start_line) {
        
        center = (start_line + stop_line)/2;

        center_prefix = getPrefix(arr, center);

        if (center_prefix == target) {
            return center;
        }

	printf("prefix: %i target: %i center: %i start: %i stop: %i\n",center_prefix,target,center,start_line,stop_line);
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
    
    return atoi(str);
}
