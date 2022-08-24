#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "modify_time.h"
#include "../../config/time_config.h"

char LICENSE[] SEC("license") = "Dual BSD/GPL";

struct {
  __uint(type, BPF_MAP_TYPE_RINGBUF);
  __uint(max_entries, 256 * 1024);
} rb SEC(".maps");

struct {
  __uint(type, BPF_MAP_TYPE_HASH);
  __uint(max_entries, 8192);
  __type(key, tid_t);
  __type(value, u64);
} time_p_map SEC(".maps");

bool comm_filter(char *comm) {
  if (!(__builtin_memcmp("cc1\0", comm, 4) == 0)) {
    return 0;
  }
  return 1;
}

SEC("tracepoint/syscalls/sys_enter_gettimeofday")
int handle_enter_gettimeofday(struct trace_event_raw_sys_enter *ctx) {
  tid_t tid = bpf_get_current_pid_tgid();

  char comm[16];
  bpf_get_current_comm(comm, 16);

  if (!comm_filter(comm)) {
    return 0;
  }

  struct __kernel_old_timeval *time_p =
      (struct __kernel_old_timeval *)ctx->args[0];
  if (time_p == NULL) {
    return 0;
  }

  long success = bpf_map_update_elem(&time_p_map, &tid, &time_p, BPF_NOEXIST);

  return 0;
}

SEC("tracepoint/syscalls/sys_exit_gettimeofday")
int handle_exit_gettimeofday(struct trace_event_raw_sys_exit *ctx) {
  tid_t tid = bpf_get_current_pid_tgid();

  char comm[16];
  bpf_get_current_comm(comm, 16);

  if (!comm_filter(comm)) {
    return 0;
  }

  long unsigned int *time_pp = bpf_map_lookup_elem(&time_p_map, &tid);
  if (time_pp == NULL) {
    return 0;
  }

  struct __kernel_old_timeval *time_p = (struct __kernel_old_timeval *)*time_pp;
  struct __kernel_old_timeval retval;

  long success =
      bpf_probe_read_user(&retval, sizeof(struct __kernel_old_timeval), time_p);

  bpf_map_delete_elem(&time_p_map, &tid);

  struct __kernel_old_timeval replace_time;

  replace_time.tv_sec = MODIFIED_CURRENT_TIMESTAMP;
  replace_time.tv_usec = 0;

  bpf_printk("[sys_exit_gettimeofday] OVERWRITING struct timeval at \
    %p from (%d, %d) to (%d, %d)",
             time_p, retval.tv_sec, retval.tv_usec, replace_time.tv_sec,
             replace_time.tv_usec);

  success = bpf_probe_write_user((char *)time_p, (char *)&replace_time,
                                 sizeof(struct __kernel_old_timeval));
  bpf_printk("[sys_exit_gettimeofday] RESULT %d", success);

  return 0;
}

SEC("tracepoint/syscalls/sys_enter_clock_gettime")
int handle_enter_clock_gettime(struct trace_event_raw_sys_enter *ctx) {
  tid_t tid = bpf_get_current_pid_tgid();

  char comm[16];
  bpf_get_current_comm(comm, 16);

  if (!comm_filter(comm)) {
    return 0;
  }

  struct timespec64 *time_p = (struct timespec64 *)ctx->args[1];
  if (time_p == NULL) {
    return 0;
  }

  long success = bpf_map_update_elem(&time_p_map, &tid, &time_p, BPF_NOEXIST);

  return 0;
}

SEC("tracepoint/syscalls/sys_exit_clock_gettime")
int handle_exit_clock_gettime(struct trace_event_raw_sys_exit *ctx) {
  tid_t tid = bpf_get_current_pid_tgid();

  char comm[16];
  bpf_get_current_comm(comm, 16);

  if (!comm_filter(comm)) {
    return 0;
  }

  long unsigned int *time_pp = bpf_map_lookup_elem(&time_p_map, &tid);
  if (time_pp == NULL) {
    return 0;
  }

  struct timespec64 *time_p = (struct timespec64 *)*time_pp;

  struct timespec64 retval;

  long success =
      bpf_probe_read_user(&retval, sizeof(struct timespec64), time_p);

  bpf_map_delete_elem(&time_p_map, &tid);

  struct timespec64 replace_time;

  replace_time.tv_sec = MODIFIED_CURRENT_TIMESTAMP;
  replace_time.tv_nsec = 0;

  bpf_printk("[sys_exit_clock_gettime] OVERWRITING struct timespec64 at \
    %p from (%d, %d) to (%d, %d)",
             time_p, retval.tv_sec, retval.tv_nsec, replace_time.tv_sec,
             replace_time.tv_nsec);

  success = bpf_probe_write_user((char *)time_p, (char *)&replace_time,
                                 sizeof(struct timespec64));
  bpf_printk("[sys_exit_clock_gettime] RESULT %d", success);

  return 0;
}
