##  scripts文件说明

|脚本|说明|
|:--:|:--:|
|install_deps.sh|依赖安装脚本(OpenBlas)|
|monitor.sh|监控重启脚本|
|queryServer_install.run|用户软件安装包|
|install_cuda.sh|安装cuda脚本|
|pack.sh|打包发布脚本|

## 打包脚本

```bash
# Current path is 'queryServer'
./scripts/pack.sh
# After this script is executed, a installer will be generate in 'release'
```

## 用户安装脚本

打包完成后，将release文件夹中的queryServer_install.run软件包交给用户。  
用户直接执行./queryServer_install.run即可安装。  
安装完成后插入加密锁，进入queryServer目录，执行./queryServer即可启动服务。  
