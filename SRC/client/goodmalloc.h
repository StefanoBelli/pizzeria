#ifndef GOODMALLOC_H
#define GOODMALLOC_H

#include <stdlib.h>

#define good_free(ptr) \
  if (ptr != NULL) {   \
    free(ptr);         \
    ptr = NULL;        \
  }

#define good_malloc(ptr, type, size)                        \
  if ((ptr = (type*)malloc(size * sizeof(type))) == NULL) { \
    fputs("memory exhausted", stderr);                      \
    exit(EXIT_FAILURE);                                     \
  }

#endif