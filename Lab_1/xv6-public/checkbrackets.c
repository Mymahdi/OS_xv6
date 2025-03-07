#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


#define OUTPUT_FILE "Result.txt"

// Function to check if brackets are balanced
int check_brackets(const char *str) {
    int count = 0;
    while (*str) {
        if (*str == '{') count++;
        else if (*str == '}') count--;
        if (count < 0) return 0; // More closing than opening
        str++;
    }
    return (count == 0); // Valid if balanced
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf(2, "Usage: checkbrackets <string>\n");
        exit();
    }

    int fd = open(OUTPUT_FILE, O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf(2, "Error: Could not open file\n");
        exit();
    }

    if (check_brackets(argv[1])) {
        write(fd, "Right\n", 6);
    } else {
        write(fd, "Wrong\n", 6);
    }

    close(fd);
    exit();
}
