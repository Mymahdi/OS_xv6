#include "types.h"
#include "stat.h"
#include "user.h"


int is_palindrome(int n) {
  int original = n, rev = 0;
  while (n > 0) {
    rev = rev * 10 + n % 10;
    n /= 10;
  }
  return original == rev;
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf(1, "Usage: testpal <number>\n");
    exit();
  }

  int num = atoi(argv[1]);

  if (is_palindrome(num)) {
    printf(1, "%d is already a palindrome.\n", num);
  } else {
    next_palindrome(num);
  }

  exit();
}