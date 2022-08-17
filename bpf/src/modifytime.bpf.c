// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "modifytime.h"
#include "../../config/time_config.h"

char LICENSE[] SEC("license") = "Dual BSD/GPL";

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
} dir_entries SEC(".maps");

bool comm_filter(char *comm)
{
    if (!(__builtin_memcmp("cc1\0", comm, 4) == 0) 
    && !(__builtin_memcmp("get\0", comm, 4) == 0)
    && !(__builtin_memcmp("date", comm, 4) == 0)
    ) {
        return 0;
    }
    
    return 1;
}

SEC("tracepoint/syscalls/sys_enter_gettimeofday")
int handle_enter_gettimeofday(struct trace_event_raw_sys_enter *ctx)
{
    tid_t tid = bpf_get_current_pid_tgid();

    char comm[16];
    bpf_get_current_comm(comm, 16);

    if (!comm_filter(comm))
    {
        return 0;
    }

    struct __kernel_old_timeval *d_entry = (struct __kernel_old_timeval *) ctx->args[0]; 
    if (d_entry == NULL)
    {
        return 0;
    }
    bpf_printk("enter tid: %lld comm: %s", tid, comm);
    bpf_printk("enter timeval address: %p", d_entry);

    long success = bpf_map_update_elem(&dir_entries, &tid, &d_entry, BPF_NOEXIST);
    bpf_printk("** UPDATE ELEM RESULT %d", success);

    return 0;
}

SEC("tracepoint/syscalls/sys_exit_gettimeofday")
int handle_exit_gettimeofday(struct trace_event_raw_sys_exit *ctx)
{
    tid_t tid = bpf_get_current_pid_tgid();

    char comm[16];
    bpf_get_current_comm(comm, 16);

    if (!comm_filter(comm))
    {
        return 0;
    }


    bpf_printk("BPF triggered from TID %lld PID %d, COMM %s.", tid, (int)(tid >> 32), comm);

    long unsigned int * d_addr  = bpf_map_lookup_elem(&dir_entries, &tid);
    if (d_addr == NULL)
    {
        return 0;
    }

    struct __kernel_old_timeval * d_entry = (struct __kernel_old_timeval *) *d_addr;

    bpf_printk("timeval address: 0x%016x %p", d_entry, d_entry);

    struct __kernel_old_timeval retval;

    long success = bpf_probe_read_user(&retval, sizeof(struct __kernel_old_timeval), d_entry);

    bpf_printk("** RESULT %d,read %d, %d", success, retval.tv_sec, retval.tv_usec);

    bpf_map_delete_elem(&dir_entries, &tid);

    struct __kernel_old_timeval replace;

    replace.tv_sec = MODIFIED_TIMESTAMP;
    replace.tv_usec = 0;

    bpf_printk("** sys_enter_getdents64 ** OVERWRITING %p, %p, %d", (char *) d_entry, (char *) &replace, sizeof(struct __kernel_old_timeval));

    success = bpf_probe_write_user((char *) d_entry, (char *) &replace, sizeof(struct __kernel_old_timeval));
    bpf_printk("** RESULT %d", success);

    return 0;
}


SEC("tracepoint/syscalls/sys_enter_clock_gettime")
int handle_enter_clock_gettime(struct trace_event_raw_sys_enter *ctx)
{
    tid_t tid = bpf_get_current_pid_tgid();

    char comm[16];
    bpf_get_current_comm(comm, 16);

    if (!comm_filter(comm))
    {
        return 0;
    }

    struct timespec64 *d_entry = (struct timespec64 *) ctx->args[1]; 
    if (d_entry == NULL)
    {
        return 0;
    }
    bpf_printk("enter tid: %lld comm: %s", tid, comm);
    bpf_printk("enter timeval address: %p", d_entry);

    long success = bpf_map_update_elem(&dir_entries, &tid, &d_entry, BPF_NOEXIST);
    bpf_printk("** UPDATE ELEM RESULT %d", success);

    return 0;
}

SEC("tracepoint/syscalls/sys_exit_clock_gettime")
int handle_exit_clock_gettime(struct trace_event_raw_sys_exit *ctx)
{
    tid_t tid = bpf_get_current_pid_tgid();

    char comm[16];
    bpf_get_current_comm(comm, 16);

    if (!comm_filter(comm))
    {
        return 0;
    }


    bpf_printk("BPF triggered from TID %lld PID %d, COMM %s.", tid, (int)(tid >> 32), comm);

    long unsigned int * d_addr  = bpf_map_lookup_elem(&dir_entries, &tid);
    if (d_addr == NULL)
    {
        return 0;
    }

    struct timespec64 * d_entry = (struct timespec64 *) *d_addr;

    bpf_printk("timeval address: 0x%016x %p", d_entry, d_entry);

    struct timespec64 retval;

    long success = bpf_probe_read_user(&retval, sizeof(struct timespec64), d_entry);

    bpf_printk("** RESULT %d,read %d, %d", success, retval.tv_sec, retval.tv_nsec);

    bpf_map_delete_elem(&dir_entries, &tid);

    struct timespec64 replace;

    replace.tv_sec = MODIFIED_TIMESTAMP;
    replace.tv_nsec = 0;

    bpf_printk("** sys_enter_getdents64 ** OVERWRITING %p, %p, %d", (char *) d_entry, (char *) &replace, sizeof(struct timespec64));

    success = bpf_probe_write_user((char *) d_entry, (char *) &replace, sizeof(struct timespec64));
    bpf_printk("** RESULT %d", success);

    return 0;
}
