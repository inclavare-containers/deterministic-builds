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
