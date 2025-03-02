#ifndef SH_H
#define SH_H

#define CMD_LEN 128
#define HISTORY_SIZE 10

// Declare history variables
extern char history[HISTORY_SIZE][CMD_LEN];
extern int history_index;
extern int history_count;

// Function declarations
void add_to_history(const char *cmd);
void print_history(void);
int getcmd(char *buf, int nbuf);

#endif // SH_H
