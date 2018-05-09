# 用户手册

<!-- TOC -->

- [用户手册](#用户手册)
    - [1 文件格式说明](#1-文件格式说明)
    - [2 API说明](#2-api说明)
    - [2.1 添加向量至索引](#21-添加向量至索引)
        - [2.1.1 请求](#211-请求)
        - [2.1.2 返回](#212-返回)
        - [2.1.3 成功请求返回值示例](#213-成功请求返回值示例)
        - [2.1.4 失败请求返回值示例](#214-失败请求返回值示例)
        - [2.1.5 当前 API 特有的 ERROR_MESSAGE](#215-当前-api-特有的-error_message)
    - [2.2 查询向量](#22-查询向量)
        - [2.2.1 请求](#221-请求)
        - [2.2.2 返回](#222-返回)
        - [2.2.3 成功请求返回值示例](#223-成功请求返回值示例)
        - [2.2.4 失败请求返回值示例](#224-失败请求返回值示例)
        - [2.2.5 当前 API 特有的 ERROR_MESSAGE](#225-当前-api-特有的-error_message)
    - [2.3 删除向量标识](#23-删除向量标识)
        - [2.3.1 请求](#231-请求)
        - [2.3.2 返回](#232-返回)
        - [2.3.3 成功请求返回值示例](#233-成功请求返回值示例)
        - [2.3.4 失败请求返回值示例](#234-失败请求返回值示例)
        - [2.3.5 当前 API 特有的 ERROR_MESSAGE](#235-当前-api-特有的-error_message)
    - [2.4 删除范围内的向量标识](#24-删除范围内的向量标识)
        - [2.4.1 请求](#241-请求)
        - [2.4.2 返回](#242-返回)
        - [2.4.3 成功请求返回值示例](#243-成功请求返回值示例)
        - [2.4.4 失败请求返回值示例](#244-失败请求返回值示例)
        - [2.4.5 当前 API 特有的 ERROR_MESSAGE](#245-当前-api-特有的-error_message)
    - [2.5 重新配置和加载索引文件](#25-重新配置和加载索引文件)
        - [2.5.1 请求](#251-请求)
        - [2.5.2 返回](#252-返回)
        - [2.5.3 成功请求返回值示例](#253-成功请求返回值示例)
        - [2.5.4 失败请求返回值示例](#254-失败请求返回值示例)
        - [2.5.5 当前 API 特有的 ERROR_MESSAGE](#255-当前-api-特有的-error_message)
    - [2.6 查询范围内向量](#26-查询范围内向量)
        - [2.6.1 请求](#261-请求)
        - [2.6.2 返回](#262-返回)
        - [2.6.3 成功请求返回值示例](#263-成功请求返回值示例)
        - [2.6.4 失败请求返回值示例](#264-失败请求返回值示例)
        - [2.6.5 当前 API 特有的 ERROR_MESSAGE](#265-当前-api-特有的-error_message)
    - [2.7 按照日期查询向量](#27-按照日期查询向量)
        - [2.7.1 请求](#271-请求)
        - [2.7.2 返回](#272-返回)
        - [2.7.3 成功请求返回值示例](#273-成功请求返回值示例)
        - [2.7.4 失败请求返回值示例](#274-失败请求返回值示例)
        - [2.7.5 当前 API 特有的 ERROR_MESSAGE](#275-当前-api-特有的-error_message)
    - [2.8 通用的 ERROR_MESSAGE](#28-通用的-error_message)

<!-- /TOC -->

## 1 文件格式说明

文件中前20个字节为文件头，前12个字节顺序存储向量维数、向量个数、文件格式版本号，存储类型为 `unsigned int`,文件头最后8个字节为空。

文件头后面顺序存储 `unsigned int`类型的向量id，以及对应的 `vector<float>`类型的特征向量feature。

| 名称 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| header[0] | `unsigned int` |  向量维数 |
| header[1] | `unsigned int` | 向量个数 |
| header[2] | `unsigned int` | 文件格式版本号 |
| id | `unsigned int` | 向量唯一标识 |
| feature | `vector<float>` | 特征向量 |

## 2 API说明

## 2.1 添加向量至索引

### 2.1.1 请求

调用地址：http://127.0.0.1/add
请求方式：POST
请求类型：application/json

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | ntotal | `Int` | 需要添加的向量个数 |
| 必选 | data | `Object` | 向量唯一标识及对应特征向量 |

**请求参数结构示例**

```json
{
	"ntotal": 100,
  	"data":
        {
            "0": [0.1,...,0.5],
            ...,
            "99": [0.1,...,0.5]
        }
}
```

### 2.1.2 返回

返回类型：JSON

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| time_used | `Int` | 整个请求所花费的时间，单位为毫秒 |
| error_message | `String` | 当请求失败时返回错误信息，请求成功时返回请求结果。 |

### 2.1.3 成功请求返回值示例

```json
{
	"time_used": 50,
  	"error_message": "ADD_SUCCESS"
}
```

### 2.1.4 失败请求返回值示例

```json
{
	"time_used": 0,
  	"error_message": "INVALID_NTOTAL"
}
```

### 2.1.5 当前 API 特有的 ERROR_MESSAGE

| HTTP状态代码 | 错误信息 | 说明 |
| :----: | :--: | :------: |
| 400 | INVALID_NTOTAL | 参数ntotal格式不正确 |
| 400 | INVALID_DATA | 参数data格式不正确 |
| 400 | EMPTY_DATA | 参数data中没有向量 |

## 2.2 查询向量

### 2.2.1 请求

调用地址：http://127.0.0.1/search
请求方式：POST
请求类型：application/json

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | qtotal | `Int` | 需要查询的向量个数 |
| 必选 | topk | `Int` | 查询最近邻的个数 |
| 必选 | queries | `Object` | 需要查询的向量标识及对应特征向量 |

**请求参数结构示例**

```json
{
	"qtotal": 10,
    "topk": 5,
  	"queries":
    {
        "q0": [0.1,...,0.5],
        ...,
        "q9": [0.1,...,0.5]
    }
}
```

### 2.2.2 返回

返回类型：JSON

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| result | `Object` | 每个查询向量的k个最近邻距离及标识 |
| time_used | `Int` | 整个请求所花费的时间，单位为毫秒 |
| error_message | `String` | 当请求失败时返回错误信息，请求成功时返回请求结果。 |

### 2.2.3 成功请求返回值示例

```json
{
	"result":
    {
        "q0": {"distance": [77.55, 78.39], "labels": [80, 1]},
        "q1": {"distance": [67.94, 71.17], "labels": [70, 49]},
		...,
        "q9": {"distance": [73.63, 73.78], "labels": [47, 28]}
    }
	"time_used": 50,
  	"error_message": "SEARCH_SUCCESS"
}
```

### 2.2.4 失败请求返回值示例

```json
{
	"time_used": 0,
  	"error_message": "EMPTY_QUERIES"
}
```

### 2.2.5 当前 API 特有的 ERROR_MESSAGE

| HTTP状态代码 | 错误信息 | 说明 |
| :----: | :--: | :------: |
| 400 | INVALID_QTOTAL | 参数qtotal格式不正确 |
| 400 | INVALID_TOPK | 参数topk格式不正确 |
| 400 | INVALID_QUERIES | 参数queries格式不正确 |
| 400 | EMPTY_QUERIES | 参数queries中没有向量 |

## 2.3 删除向量标识

### 2.3.1 请求

调用地址：http://127.0.0.1/delete
请求方式：POST
请求类型：application/json

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | ids | `Array` | 需要删除的向量标识列表 |

**请求参数结构示例**

```json
{
    "ids": [1,...,5]
}
```

### 2.3.2 返回

返回类型：JSON

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| time_used | `Int` | 整个请求所花费的时间，单位为毫秒 |
| ndelete | `Int` | 成功删除的向量个数 |
| error_message | `String` | 当请求失败时返回错误信息，请求成功时返回请求结果。 |

### 2.3.3 成功请求返回值示例

```json
{
	"time_used": 10,
    "ndelete": 10,
  	"error_message": "DELETE_SUCCESS"
}
```

### 2.3.4 失败请求返回值示例

```json
{
	"time_used": 0,
    "ndelete": 0,    
  	"error_message": "INVALID_IDS"
}
```

### 2.3.5 当前 API 特有的 ERROR_MESSAGE

| HTTP状态代码 | 错误信息 | 说明 |
| :----: | :--: | :------: |
| 400 | INVALID_IDS | 参数ids格式不正确 |
| 400 | EMPTY_DATA | 参数ids为空 |

## 2.4 删除范围内的向量标识

### 2.4.1 请求

调用地址：http://127.0.0.1/deleteRange
请求方式：POST
请求类型：application/json

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | start | `Int` | 需要删除的起始向量标识 |
| 必选 | end | `Int` | 需要删除的结尾向量标识 |

**请求参数结构示例**

```json
{
    "start": 1,
    "end": 5
}
```

### 2.4.2 返回

返回类型：JSON

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| time_used | `Int` | 整个请求所花费的时间，单位为毫秒 |
| ndelete | `Int` | 成功删除的向量个数 |
| error_message | `String` | 当请求失败时返回错误信息，请求成功时返回请求结果。 |

### 2.4.3 成功请求返回值示例

```json
{
	"time_used": 10,
    "ndelete": 10,    
  	"error_message": "DELETERANGE_SUCCESS"
}
```

### 2.4.4 失败请求返回值示例

```json
{
	"time_used": 0,
    "ndelete": 0,    
  	"error_message": "INVALID_RANGE"
}
```

### 2.4.5 当前 API 特有的 ERROR_MESSAGE

| HTTP状态代码 | 错误信息 | 说明 |
| :----: | :--: | :------: |
| 400 | INVALID_RANGE | 参数start或end格式不正确 |

## 2.5 重新配置和加载索引文件

### 2.5.1 请求

调用地址：http://127.0.0.1/reconfig
请求方式：POST
请求类型：application/json

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | reconfigFilePath | `String` | 需要重新加载的配置文件路径 |

**请求参数结构示例**

```json
{
    "reconfigFilePath": "config.json"
}
```

### 2.5.2 返回

返回类型：JSON

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| time_used | `Int` | 整个请求所花费的时间，单位为毫秒 |
| error_message | `String` | 当请求失败时返回错误信息，请求成功时返回请求结果。 |

### 2.5.3 成功请求返回值示例

```json
{
	"time_used": 350,
  	"error_message": "RECONFIG_SUCCESS"
}
```

### 2.5.4 失败请求返回值示例

```json
{
	"time_used": 0,
  	"error_message": "INVALID_RECONFIG_FILEPATH"
}
```

### 2.5.5 当前 API 特有的 ERROR_MESSAGE

| HTTP状态代码 | 错误信息 | 说明 |
| :----: | :--: | :------: |
| 400 | INVALID_RECONFIG_FILEPATH | 参数reconfigFilePath格式不正确 |
| 400 | RELOAD_FAIL | 重新加载数据失败 |
| 400 | READD_FAIL | 重新构建索引结构失败 |

## 2.6 查询范围内向量

### 2.6.1 请求

调用地址：http://127.0.0.1/searchRange
请求方式：POST
请求类型：application/json

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | nq | `Int` | 需要查询的向量个数 |
| 必选 | radius | `Number` | 查询最近邻的范围阈值 |
| 必选 | queries | `Object` | 需要查询的向量标识及对应特征向量 |

**请求参数结构示例**

```json
{
	"nq": 10,
    "radius": 0.5,
  	"queries":
    {
        "q0": [0.1,...,0.5],
        ...,
        "q9": [0.1,...,0.5]
    }
}
```

### 2.6.2 返回

返回类型：JSON

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| result | `Object` | 每个查询向量的最近邻距离及标识 |
| time_used | `Int` | 整个请求所花费的时间，单位为毫秒 |
| error_message | `String` | 当请求失败时返回错误信息，请求成功时返回请求结果。 |

### 2.6.3 成功请求返回值示例

```json
{
	"result":
    {
        "q0": {"distance": [77.55, 78.39], "labels": [80, 1]},
        "q1": {"distance": [67.94, 71.17], "labels": [70, 49]},
		...,
        "q9": {"distance": [73.63, 73.78], "labels": [47, 28]}
    }
	"time_used": 50,
  	"error_message": "SEARCH_RANGE_SUCCESS"
}
```

### 2.6.4 失败请求返回值示例

```json
{
	"time_used": 0,
  	"error_message": "EMPTY_QUERIES"
}
```

### 2.6.5 当前 API 特有的 ERROR_MESSAGE

| HTTP状态代码 | 错误信息 | 说明 |
| :----: | :--: | :------: |
| 400 | INVALID_NQ | 参数nq格式不正确 |
| 400 | INVALID_RADIUS | 参数radius格式不正确 |
| 400 | INVALID_QUERIES | 参数queries格式不正确 |
| 400 | EMPTY_QUERIES | 参数queries中没有向量 |

## 2.7 按照日期查询向量

### 2.7.1 请求

调用地址：http://127.0.0.1/searchDays
请求方式：POST
请求类型：application/json

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | qtotal | `Int` | 需要查询的向量个数 |
| 必选 | topk | `Number` | 查询最近邻的个数 |
| 必选 | queries | `Object` | 需要查询的向量标识及对应特征向量 |
| 必选 | days | `List` | 需要查询的日期列表 |

**请求参数结构示例**

```json
{
	"qtotal": 10,
    "topk": 5,
  	"queries":
    {
        "q0": [0.1,...,0.5],
        ...,
        "q9": [0.1,...,0.5]
    },
    "days":["20180424","20180425"]
}
```

### 2.7.2 返回

返回类型：JSON

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| result | `Object` | 每个查询向量的最近邻距离及标识 |
| time_used | `Int` | 整个请求所花费的时间，单位为毫秒 |
| error_message | `String` | 当请求失败时返回错误信息，请求成功时返回请求结果。 |

### 2.7.3 成功请求返回值示例

```json
{
	"result":
    {
        "q0": {"distance": [77.55, 78.39], "labels": [80, 1]},
        "q1": {"distance": [67.94, 71.17], "labels": [70, 49]},
		...,
        "q9": {"distance": [73.63, 73.78], "labels": [47, 28]}
    }
	"time_used": 50,
  	"error_message": "SEARCHDAYS_SUCCESS"
}
```

### 2.7.4 失败请求返回值示例

```json
{
	"time_used": 0,
  	"error_message": "SEARCHDAYS_FAIL"
}
```

### 2.7.5 当前 API 特有的 ERROR_MESSAGE

| HTTP状态代码 | 错误信息 | 说明 |
| :----: | :--: | :------: |
| 400 | INVALID_QTOTAL | 参数qtotal格式不正确 |
| 400 | INVALID_DAYS | 参数days格式不正确 |
| 400 | INVALID_QUERIES | 参数queries格式不正确 |
| 400 | EMPTY_QUERIES | 参数queries中没有向量 |
| 400 | INVALID_TOPK | 参数topk格式不正确 |
| 400 | NOTEXIST_INDEX | 要查询日期的索引文件不存在 |

## 2.8 通用的 ERROR_MESSAGE

| HTTP状态代码 | 错误信息 | 说明 |
| :----: | :--: | :------: |
| 400 | MISSING_ARGUMENTS | 缺少某个必选参数 |
| 400 | INVALID_DIMENSION | 参数data中向量维数不匹配 |