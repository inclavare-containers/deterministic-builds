# Test Environment Set up

Docker

Run as root

```shell
docker build -t test/compile .
docker run -it test/compile
docker exec -it <CONTAINER ID> /bin/bash
```

For users in China

```
docker build -t test/compile -f Dockerfile.cn .
```

## References

[An introduction to deterministic builds with C/C++](https://blog.conan.io/2019/09/02/Deterministic-builds-with-C-C++.html)
