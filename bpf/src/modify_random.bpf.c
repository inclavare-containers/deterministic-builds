#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "modify_file_read.h"
#include "common.bpf.h"
#include "../../config/time_config.h"

char LICENSE[] SEC("license") = "Dual BSD/GPL";

char random_name[] = "/dev/random";
char urandom_name[] = "/dev/urandom";
char replace_buf[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

struct read_prop {
  void *buf;
  size_t count;
};

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
  __type(value, struct read_prop);
} read_props SEC(".maps");

struct {
  __uint(type, BPF_MAP_TYPE_HASH);
  __uint(max_entries, 8192);
  __type(key, tid_t);
  __type(value, u64);
} random_bufs SEC(".maps");

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

  if (((__builtin_memcmp(random_name, filename, 4) == 0) &&
       (__builtin_memcmp(random_name + 4, filename + 4, 4) == 0) &&
       (random_name[9] == filename[9] && random_name[10] == filename[10] &&
        random_name[11] == filename[11]))) {
    bpf_printk("[sys_enter_openat] Detected reading %s", filename);

    u8 blank = 0;
    bpf_map_update_elem(&tids, &tid, &blank, BPF_ANY);
  }

  if (((__builtin_memcmp(urandom_name, filename, 4) == 0) &&
       (__builtin_memcmp(urandom_name + 4, filename + 4, 4) == 0) &&
       (__builtin_memcmp(urandom_name + 8, filename + 8, 4) == 0))) {
    bpf_printk("[sys_enter_openat] Detected reading %s", filename);

    u8 blank = 0;
    bpf_map_update_elem(&tids, &tid, &blank, BPF_ANY);
  }

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

  struct read_prop a_read_prop;
  a_read_prop.buf = (void *)ctx->args[1];
  a_read_prop.count = (size_t)ctx->args[2];

  bpf_map_update_elem(&read_props, &tid, &a_read_prop, BPF_ANY);

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

  long unsigned int *a_read_prop_p = bpf_map_lookup_elem(&read_props, &tid);
  if (a_read_prop_p == NULL) {
    return 0;
  }

  struct read_prop a_read_prop = *(struct read_prop *)a_read_prop_p;

  bpf_printk("[sys_exit_read] OVERWRITING read buf at %p size %d to 0",
             a_read_prop.buf, a_read_prop.count);
  bool success = bpf_probe_write_user(a_read_prop.buf, replace_buf, 8);
  bpf_printk("[sys_exit_read] RESULT %d", success);

  return 0;
}

SEC("tracepoint/syscalls/sys_enter_getrandom")
int handle_enter_getrandom(struct trace_event_raw_sys_enter *ctx) {
  char comm[16];
  bpf_get_current_comm(comm, 16);

  if (!comm_filter(comm)) {
    return 0;
  }

  tid_t tid = bpf_get_current_pid_tgid();

  void *random_buf = (void *)ctx->args[0];
  bpf_map_update_elem(&random_bufs, &tid, &random_buf, BPF_ANY);

  return 0;
}

SEC("tracepoint/syscalls/sys_exit_getrandom")
int handle_exit_getrandom(struct trace_event_raw_sys_exit *ctx) {
  char comm[16];
  bpf_get_current_comm(comm, 16);
  if (!comm_filter(comm)) {
    return 0;
  }

  tid_t tid = bpf_get_current_pid_tgid();

  long unsigned int *random_buf_p = bpf_map_lookup_elem(&random_bufs, &tid);
  if (random_buf_p == NULL) {
    return 0;
  }
  bpf_map_delete_elem(&random_bufs, &tid);

  char *random_buf = (char *)*random_buf_p;

  bpf_printk("[sys_exit_getrandom] OVERWRITING random buf at %p size 8 to 0",
             random_buf);
  bool success = bpf_probe_write_user(random_buf, replace_buf, 8);
  bpf_printk("[sys_exit_getrandom] RESULT %d", success);

  return 0;
}
