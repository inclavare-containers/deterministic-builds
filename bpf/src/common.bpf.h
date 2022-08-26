
__inline__ __attribute__((always_inline)) bool comm_filter(char *comm) {
  if ((__builtin_memcmp("cc1\0", comm, 4) == 0)) {
    return 1;
  }
  if ((__builtin_memcmp("cc1p", comm, 4) == 0) &&
      (__builtin_memcmp("lus\0", comm + 4, 4) == 0)) {
    return 1;
  }
  if ((__builtin_memcmp("lto1", comm, 4) == 0) &&
      (__builtin_memcmp("-wpa", comm + 4, 4) == 0)) {
    return 1;
  }
  if ((__builtin_memcmp("g++\0", comm, 4) == 0)) {
    return 1;
  }
  if ((__builtin_memcmp("gcc\0", comm, 4) == 0)) {
    return 1;
  }
  return 0;
}
