#ifndef PEAR_H
#define PEAR_H

#ifdef __cplusplus
extern "C" {
#endif

#define PEAR_ID_MAX 64

typedef char pear_id_t[PEAR_ID_MAX + 1 /* NULL */];

int
pear_launch(int argc, char *argv[], pear_id_t id, const char *name);

#ifdef __cplusplus
}
#endif

#endif // PEAR_H
