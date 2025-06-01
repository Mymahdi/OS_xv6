#include "types.h"
#include "user.h"


int main(void) {
  init_rw_lock();
  int pattern = 0b10011;
  get_rw_pattern(pattern);
  exit();
}
