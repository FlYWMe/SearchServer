# 配置说明

|选项|参数类型|说明|
|:--|:--|:--|
|listenip|ipv4||
|port|int||
|httpThreads|int|处理http请求|
|logConfigParams|object|日志级别开关配置|
|dataFilePath|string|需要加载的数据文件路径|
|dimension|int|特征维数|
|searchFactory|string|faiss的索引结构类型|
|usegpu|bool|是否使用GPU|

```json
{
    "httpParams": {
        "httpThreads": 2,
        "listenIP": "0.0.0.0",
        "port": 2333
    },
    "logConfigParams": {
        "debugEnabled": false,
        "infoEnabled": true,
        "warningEnabled": false
    },
    "searchToolParams": {
        "dataFilePath": "data.bin",
        "dimension": 512,
        "searchFactory": "IDMap,Flat",
        "usegpu": true
    }
}
```
