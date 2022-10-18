#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "modify_file_read.h"
#include "common.bpf.h"
#include "../../config/time_config.h"

char LICENSE[] SEC("license") = "Dual BSD/GPL";

char localtime_name[] = "/etc/localtime";

char localtime_content[] = "TZif2\0\0\0\0\0\0\0\0\0\0\0\
                            \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\
                            \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\
                            \0\0UTC\0TZif2\0\0\0\0\0\
                            \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\
                            \0\0\0\0\0\0\0\0\0\0\0\0\0\x01\0\0\
                            \0\x04\0\0\0\0\0\0UTC\0\x0aUTC\
                            0.";

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

struct {
  __uint(type, BPF_MAP_TYPE_HASH);
  __uint(max_entries, 8192);
  __type(key, tid_t);
  __type(value, u64);
} content_ps SEC(".maps");

SEC("tracepoint/syscalls/sys_enter_openat")
int handle_enter_openat(struct trace_event_raw_sys_enter *ctx) {
  tid_t tid = bpf_get_current_pid_tgid();

  char comm[16];
  bpf_get_current_comm(comm, 16);

  if (!comm_filter(comm)) {
    return 0;
  }

  char *filename_p = (char *)ctx->args[1];

  char filename[20];
  long success = bpf_probe_read_user(filename, 20, filename_p);

  if (!((__builtin_memcmp(localtime_name, filename, 4) == 0) &&
        (__builtin_memcmp(localtime_name + 4, filename + 4, 4) == 0) &&
        (__builtin_memcmp(localtime_name + 8, filename + 8, 4) == 0) &&
        (localtime_name[12] == filename[12] &&
         localtime_name[13] == filename[13] &&
         localtime_name[14] == filename[14]))) {
    return 0;
  }

  bpf_printk("[sys_enter_openat] Detected reading %s", filename);

  u8 blank = 0;
  bpf_map_update_elem(&tids, &tid, &blank, BPF_ANY);

  return 0;
}

SEC("tracepoint/syscalls/sys_enter_newfstatat")
int handle_enter_newfstatat(struct trace_event_raw_sys_enter *ctx) {
  char comm[16];
  bpf_get_current_comm(comm, 16);
  if (!comm_filter(comm)) {
    return 0;
  }

  tid_t tid = bpf_get_current_pid_tgid();

  long unsigned int *blank_p = bpf_map_lookup_elem(&tids, &tid);
  if (blank_p == NULL) {
    return 0;
  }

  void *stat_p = (void *)ctx->args[2];

  struct stat statbuf;
  long success = bpf_probe_read_user(&statbuf, sizeof(struct stat), stat_p);

  bpf_map_update_elem(&stat_ps, &tid, &stat_p, BPF_ANY);

  return 0;
}

SEC("tracepoint/syscalls/sys_exit_newfstatat")
int handle_exit_newfstatat(struct trace_event_raw_sys_exit *ctx) {
  char comm[16];
  bpf_get_current_comm(comm, 16);
  if (!comm_filter(comm)) {
    return 0;
  }

  tid_t tid = bpf_get_current_pid_tgid();

  long unsigned int *blank_p = bpf_map_lookup_elem(&tids, &tid);
  if (blank_p == NULL) {
    return 0;
  }

  long unsigned int *stat_pp = bpf_map_lookup_elem(&stat_ps, &tid);
  if (stat_pp == NULL) {
    return 0;
  }
  bpf_map_delete_elem(&stat_ps, &tid);

  struct stat *stat_p = (struct stat *)*stat_pp;
  struct stat statbuf;
  long success = bpf_probe_read_user(&statbuf, sizeof(struct stat), stat_p);

  long unsigned int localtime_size = 0x72;
  long int replace_size = localtime_size;
  bpf_printk(
      "[sys_exit_newfstatat] OVERWRITING stat.st_size at %p from %d to %d",
      stat_p, statbuf.st_size, replace_size);
  success = bpf_probe_write_user((char *)stat_p + 48, (char *)&replace_size,
                                 sizeof(long int));
  bpf_printk("[sys_exit_newfstatat] RESULT %d", success);

  return 0;
}

SEC("tracepoint/syscalls/sys_enter_read")
int handle_enter_read(struct trace_event_raw_sys_enter *ctx) {
  char comm[16];
  bpf_get_current_comm(comm, 16);
  if (!comm_filter(comm)) {
    return 0;
  }

  tid_t tid = bpf_get_current_pid_tgid();

  long unsigned int *blank_p = bpf_map_lookup_elem(&tids, &tid);
  if (blank_p == NULL) {
    return 0;
  }

  char *content_p = (void *)ctx->args[1];

  bpf_map_update_elem(&content_ps, &tid, &content_p, BPF_ANY);

  return 0;
}

SEC("tracepoint/syscalls/sys_exit_read")
int handle_exit_read(struct trace_event_raw_sys_exit *ctx) {
  char comm[16];
  bpf_get_current_comm(comm, 16);
  if (!comm_filter(comm)) {
    return 0;
  }

  tid_t tid = bpf_get_current_pid_tgid();

  long unsigned int *blank_p = bpf_map_lookup_elem(&tids, &tid);
  if (blank_p == NULL) {
    return 0;
  }
  bpf_map_delete_elem(&tids, &tid);

  long unsigned int *content_pp = bpf_map_lookup_elem(&content_ps, &tid);
  if (content_pp == NULL) {
    return 0;
  }

  long unsigned int localtime_size = 0x72;
  char *content_p = (char *)*content_pp;

  bpf_printk("[sys_exit_newfstatat] OVERWRITING read buf at %p to UTC",
             content_p);
  bool success =
      bpf_probe_write_user(content_p, localtime_content, localtime_size);
  bpf_printk("[sys_exit_newfstatat] RESULT %d", success);

  return 0;
}
