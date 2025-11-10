#include "../include/pear.h"

int
main(int argc, char *argv[]) {
  pear_path = PEAR_PATH;

  static pear_id_t id = "keet";
  static const char *name = "keet";

  return pear_launch(argc, argv, id, name);
}
