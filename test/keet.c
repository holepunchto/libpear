#include "../include/pear.h"

int
main(int argc, char *argv[]) {
  static pear_id_t id = "keet";
  static const char *name = "keet";
  static const char *link = "pear://keet";

  return pear_launch(argc, argv, id, name, link);
}
