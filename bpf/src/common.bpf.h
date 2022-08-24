
bool comm_filter(char *comm) {
  if (!(__builtin_memcmp("cc1\0", comm, 4) == 0)) {
    return 0;
  }
  return 1;
}
