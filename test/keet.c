#include "../include/pear.h"

int
main(int argc, char *argv[]) {
  static pear_id_t id = "keet";
  static const char *name = "keet";
  static const char *link = "pear://oeeoz3w6fjjt7bym3ndpa6hhicm8f8naxyk11z4iypeoupn6jzpo";

  return pear_launch(argc, argv, id, name, link);
}
