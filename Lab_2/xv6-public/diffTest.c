#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAX_LINE 512

int
read_line(int fd, char *buf, int max)
{
  int i = 0;
  char c;
  while (i < max - 1) {
    int n = read(fd, &c, 1);
    if (n < 0) return -1;
    if (n == 0) break;
    buf[i++] = c;
    if (c == '\n') break;
  }
  buf[i] = '\0';
  return i;
}

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    printf(1, "Usage: test file1 file2\n");
    exit();
  }

  int diff_count = diff(argv[1], argv[2]);
  if (diff_count == -1) {
    printf(1, "Error: Could not open one or both files\n");
    exit();
  }

  if (diff_count == 0) {
    printf(1, "Success: The files are fully same !\n");
    exit();
  }

  printf(1, "Files differ in %d characters. Line differences:\n", diff_count);

  int fd1 = open(argv[1], O_RDONLY);
  int fd2 = open(argv[2], O_RDONLY);
  if (fd1 < 0 || fd2 < 0) {
    printf(1, "Error: Could not reopen files for detailed comparison\n");
    if (fd1 >= 0) close(fd1);
    if (fd2 >= 0) close(fd2);
    exit();
  }

  char line1[MAX_LINE], line2[MAX_LINE];
  int line_num = 0;

  while (1) {
    line_num++;

    int n1 = read_line(fd1, line1, MAX_LINE);
    int n2 = read_line(fd2, line2, MAX_LINE);

    if (n1 < 0 || n2 < 0) {
      printf(1, "Error: Failed to read files\n");
      break;
    }

    if (n1 == 0 && n2 == 0) break;

    if (n1 == 0 && n2 > 0) {
      if (line2[n2-1] == '\n') line2[n2-1] = '\0';
      printf(1, "Line %d:\n", line_num);
      printf(1, "  file1: (end of file)\n");
      printf(1, "  file2: '%s'\n", line2);
      continue;
    }
    if (n2 == 0 && n1 > 0) {
      if (line1[n1-1] == '\n') line1[n1-1] = '\0';
      printf(1, "Line %d:\n", line_num);
      printf(1, "  file1: '%s'\n", line1);
      printf(1, "  file2: (end of file)\n");
      continue;
    }

    int len1 = n1 > 0 && line1[n1-1] == '\n' ? n1-1 : n1;
    int len2 = n2 > 0 && line2[n2-1] == '\n' ? n2-1 : n2;
    int same = (len1 == len2);
    if (same) {
      for (int i = 0; i < len1; i++) {
        if (line1[i] != line2[i]) {
          same = 0;
          break;
        }
      }
    }

    if (!same) {
      if (len1 > 0 && line1[len1-1] == '\n') line1[len1-1] = '\0';
      if (len2 > 0 && line2[len2-1] == '\n') line2[len2-1] = '\0';

      printf(1, "Line %d:\n", line_num);
      printf(1, "  file1: '%s'\n", line1);
      printf(1, "  file2: '%s'\n", line2);

      printf(1, "  Differences in line %d:\n", line_num);
      int max_len = (len1 > len2) ? len1 : len2;
      for (int i = 0; i < max_len; i++) {
        if (i >= len1) {
          printf(1, "    Position %d: (end of file1) vs '%c'\n", i+1, line2[i]);
        } else if (i >= len2) {
          printf(1, "    Position %d: '%c' vs (end of file2)\n", i+1, line1[i]);
        } else if (line1[i] != line2[i]) {
          printf(1, "    Position %d: '%c' vs '%c'\n", i+1, line1[i], line2[i]);
        }
      }
    }
  }

  close(fd1);
  close(fd2);
  exit();
}