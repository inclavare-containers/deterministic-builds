// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "modify_file_timestamp.h"
#include "../../config/time_config.h"

char LICENSE[] SEC("license") = "Dual BSD/GPL";

struct file_times {
    long int st_atime;
	long unsigned int st_atime_nsec;
	long int st_mtime;
	long unsigned int st_mtime_nsec;
	long int st_ctime;
	long unsigned int st_ctime_nsec;
};

struct
{
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
} rb SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 8192);
    __type(key, tid_t);
    __type(value, u64);
} stat_ps SEC(".maps");

bool comm_filter(char *comm)
{
    if (!(__builtin_memcmp("cc1\0", comm, 4) == 0)) {
        return 0;
    }
    return 1;
}

SEC("tracepoint/syscalls/sys_enter_newfstatat")
int handle_enter_newfstatat(struct trace_event_raw_sys_enter *ctx)
{
    tid_t tid = bpf_get_current_pid_tgid();

    char comm[16];
    bpf_get_current_comm(comm, 16);
    if (!comm_filter(comm))
    {
        return 0;
    }

    void * stat_p = (void *) ctx->args[2];

    struct stat statbuf;
    long success = bpf_probe_read_user(&statbuf, sizeof(struct stat), stat_p);

    bpf_map_update_elem(&stat_ps, &tid, &stat_p, BPF_ANY);

    return 0;
}

SEC("tracepoint/syscalls/sys_exit_newfstatat")
int handle_exit_newfstatat(struct trace_event_raw_sys_exit *ctx)
{
    struct task_struct *task;
    struct event       *e;

    bool should_modify = true;

    char comm[16];
    bpf_get_current_comm(comm, 16);
    if (!comm_filter(comm))
    {
        return 0;
    }

    task = (struct task_struct *)bpf_get_current_task();
    tid_t tid = bpf_get_current_pid_tgid();

    long unsigned int * stat_pp  = bpf_map_lookup_elem(&stat_ps, &tid);
    if (stat_pp == NULL)
    {
        return 0;
    }
    bpf_map_delete_elem(&stat_ps, &tid);

    struct stat * stat_p = (struct stat *) *stat_pp;
    struct file_times file_times_buf;
    long success = bpf_probe_read_user(&file_times_buf, sizeof(struct file_times), stat_p + 72);

    struct file_times replace_file_times;
    replace_file_times.st_atime = MODIFIED_FILE_TIMESTAMP;
    replace_file_times.st_atime_nsec = 0;
    replace_file_times.st_mtime = MODIFIED_FILE_TIMESTAMP;
    replace_file_times.st_mtime_nsec = 0;
    replace_file_times.st_ctime = MODIFIED_FILE_TIMESTAMP;
    replace_file_times.st_ctime_nsec = 0;

    bpf_printk("[sys_exit_newfstatat] OVERWRITING stat.(st_atime, st_mtime, st_ctime) at %p from (%d, %d, %d) to (%d, %d, %d)",
    stat_p, file_times_buf.st_atime, file_times_buf.st_mtime, file_times_buf.st_ctime, 
    replace_file_times.st_atime, replace_file_times.st_mtime, replace_file_times.st_ctime);
    success = bpf_probe_write_user((char *) stat_p + 72, (char *) &replace_file_times, 32);
    bpf_printk("[sys_exit_newfstatat] RESULT %d", success);

    return 0;
}
