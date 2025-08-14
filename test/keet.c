#include "../include/pear.h"

int
main(int argc, char *argv[]) {
  static pear_id_t id = "keet";
  static const char *name = "keet";
  static const char *link = "pear://ma9zo8zmfat3ih314mne1q47shrfho1odyfm9methgnn7b866w6y";

  return pear_launch(argc, argv, id, name, link);
}
