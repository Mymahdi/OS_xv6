// In user/testrwlock.c
#include "types.h"
#include "stat.h"
#include "user.h"

// Helper function to convert int to binary string for printing (optional)
char* itob(int n, char* s) {
    int i = 31; // Assuming 32-bit int
    while (i >= 0 && !((n >> i) & 1) && n !=0 ) i--; // Find MSB
    if (n==0) {s[0] = '0'; s[1] = '\0'; return s;}

    int j = 0;
    while(i >= 0) {
        s[j++] = ((n >> i--) & 1) ? '1' : '0';
    }
    s[j] = '\0';
    return s;
}


int main(void) {
    printf(1, "Initializing Readers-Writers Lock...\n");
    if (init_rw_lock() < 0) {
        printf(2, "Failed to initialize rw_lock\n");
        exit();
    }
    printf(1, "Lock initialized. Shared resource value: 0\n");

    // Define some patterns based on the problem's convention
    // 19 -> 10011_b -> Operations: R, R, W, W
    // 5  -> 101_b   -> Operations: R, W
    // 3  -> 11_b    -> Operations: W
    // 2  -> 10_b    -> Operations: R
    int pattern_rrww = 19;
    int pattern_rw = 5;
    int pattern_w = 3;
    int pattern_r = 2;
    char buffer[33];


    if (fork() == 0) { // Child 1: Reader, Reader, Writer, Writer
        printf(1, "Child 1 (PID %d) starting with pattern %d (0b%s -> RRWW)\n", getpid(), pattern_rrww, itob(pattern_rrww, buffer));
        get_rw_pattern(pattern_rrww);
        printf(1, "Child 1 (PID %d) finished pattern.\n", getpid());
        exit();
    }

    if (fork() == 0) { // Child 2: Reader, Writer
        printf(1, "Child 2 (PID %d) starting with pattern %d (0b%s -> RW)\n", getpid(), pattern_rw, itob(pattern_rw, buffer));
        get_rw_pattern(pattern_rw);
        printf(1, "Child 2 (PID %d) finished pattern.\n", getpid());
        exit();
    }

    if (fork() == 0) { // Child 3: Writer
        printf(1, "Child 3 (PID %d) starting with pattern %d (0b%s -> W)\n", getpid(), pattern_w, itob(pattern_w, buffer));
        get_rw_pattern(pattern_w);
        printf(1, "Child 3 (PID %d) finished pattern.\n", getpid());
        exit();
    }
    
    if (fork() == 0) { // Child 4: Reader
        printf(1, "Child 4 (PID %d) starting with pattern %d (0b%s -> R)\n", getpid(), pattern_r, itob(pattern_r, buffer));
        get_rw_pattern(pattern_r);
        printf(1, "Child 4 (PID %d) finished pattern.\n", getpid());
        exit();
    }
    
    if (fork() == 0) { // Child 5: Another RRWW to test writer preference against new readers
        printf(1, "Child 5 (PID %d) starting with pattern %d (0b%s -> RRWW)\n", getpid(), pattern_rrww, itob(pattern_rrww, buffer));
        get_rw_pattern(pattern_rrww);
        printf(1, "Child 5 (PID %d) finished pattern.\n", getpid());
        exit();
    }


    // Parent waits for all children
    for (int i = 0; i < 5; i++) {
        wait();
    }
    printf(1, "All children finished. Final shared resource value check (from parent, not synchronized): inspect kernel logs for actual final value.\n");

    exit();
}