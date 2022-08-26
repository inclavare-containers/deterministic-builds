# eBPF programs

## Usage

### Make

```shell
cd src
make modify_time
# or
make
```

### CMake

```shell
cd src
mkdir build
cd build
cmake ..
make
```

## Programs

## `modify_time`

`modify_time` intercepts `gettimeofday` and `clock_gettime` to return the specified timestamp.

## `modify_file_read`

`modify_file_read` intercepts the reading syscalls of `/etc/localtime`, making them always return the content of `/usr/share/zoneinfo/UTC`.

## `modify_file_name`

`modify_file_name` intercepts `openat` syscall to modify the names of read program files suffixed with ".c", ".cc", ".cpp" or ".cxx" to empty, making `__FILE__` macro return a fixed value.

## `preload_filter`

`preload_filter` intercepts the reading syscalls of `/etc/ld.so.preload` when loading an executable file. The reading syscalls return origin content of `/etc/ld.so.preload` when the specified processes calling them, or they return empty content.
