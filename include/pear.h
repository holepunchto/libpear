#ifndef PEAR_H
#define PEAR_H

#ifdef __cplusplus
extern "C" {
#endif

#define PEAR_KEY_MAX 64

typedef char pear_key_t[PEAR_KEY_MAX + 1 /* NULL */];

int
pear_launch (int argc, char *argv[], pear_key_t key, const char *name);

#ifdef __cplusplus
}
#endif

#endif // PEAR_H
