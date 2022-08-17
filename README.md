# Deterministic Builds

Deterministic Builds uses eBPF to intercept syscalls, to build same binaries from same source.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Usage](#usage)
- [Dependencies](#dependencies)
- [Lisence](#license)

## Prerequisites

To compile eBPF programs and preload shared objects, you will need `clang`, `libelf`, `zlib`, `build-essential`.

On Ubuntu:
```
apt install clang libelf1 libelf-dev zlib1g-dev build-essential
```

## Usage

### Work with Docker

1. Check and turn off [vDSO](https://man7.org/linux/man-pages/man7/vdso.7.html), making it possible for eBPF programs to intercept syscalls about time. You can run `turn_off_vdso.sh` as `root`. It will add `vdso=0` flag to `GRUB_CMDLINE_LINUX_DEFAULT` in `/etc/default/grub`, and it run a reboot to make it take effect. You can also do this manually.
2. Build and Run Docker as `root`.
```shell
docker build -t deterministic_builds .
docker run -it deterministic_builds
```
3. Start eBPF interception in the host OS, run script as `root`.
```shell
./start.sh
```
4. Run compile in Docker.
5. Stop eBPF interception.
```shell
./stop.sh
```

## Dependencies

The eBPF program in this project is developed based on libbpf-bootstap.

- [libbpf](https://github.com/libbpf/libbpf.git)
- [libbpf-bootstrap](https://github.com/libbpf/libbpf-bootstrap)

## License

[Apache 2.0](../LICENSE)
