# usage

## Before you start

将docker文件夹中的Dockerfile文件中的git clone后面的地址中的USERNAME和PASSWORD改为你的远程仓库用户名和密码。

## Prerequisites

- [docker-17](https://docs.docker.com/install/linux/docker-ce/ubuntu/#install-docker-ce-1)
- [nvidia-docker](https://github.com/nvidia/nvidia-docker/wiki/Installation-(version-2.0))

## GPU

```bash
#current path is SearchServer/docker
# build image
$ docker build -t searcher:gpu -f Dockerfile .
# start container
$ nvidia-docker run -p 2333:2333 searcher:gpu
```
