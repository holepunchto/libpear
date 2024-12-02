#include "../include/pear.h"

int
main(int argc, char *argv[]) {
  static pear_key_t key = "keet";
  static const char *name = "keet";

  return pear_launch(argc, argv, key, name);
}
