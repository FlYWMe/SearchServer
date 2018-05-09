# 规范

<!-- TOC -->

- [规范](#规范)
    - [1 C++代码规范(Fellow Google)](#1-c代码规范fellow-google)
    - [2 Git提交规范](#2-git提交规范)
    - [3 版本号管理](#3-版本号管理)
    - [4 API](#4-api)

<!-- /TOC -->

## 1 C++代码规范(Fellow Google)

1. 源文件名/变量命名：小驼峰命名法，一般不使用缩写
2. 类名使用大驼峰命名法
3. 命名空间一律小写
4. 代码注释与文档使用[Doxygen][Doxygen]

参考[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)  
参考[Google C++ Style Guide(cn)](http://zh-google-styleguide.readthedocs.io/en/latest/)  
参考[doxygen/manual cpp](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html#cppblock)  

## 2 Git提交规范

1. 每个人在自己的分支开发，开发完成后自测，自测后提交。提交信息命名为`add,fix,update`，添加功能时使用`add`，修复bug时使用`fix`，更新已有功能时或不符合前两种时使用`update`
```bash
# 例如
git commit -m "fix http包长度bug"
git commit -m "add base64解码"
git commit -m "update 更新README.md"

# 推送到远程仓库
$ git push
```

2. 发起`PR`经`Review`之后合并入`develop`分支

## 3 版本号管理

1. 使用[语义版本][Semantic Versioning]，版本号`MAJOR.MINOR.PATCH`

|Version|Detial|
|:-|:-|
|MAJOR version| when you make incompatible API changes|
|MINOR version| when you add functionality in a backwards-compatible manner|
|PATCH version| when you make backwards-compatible bug fixes|
**Additional labels for pre-release and build metadata are available as extensions to the MAJOR.MINOR.PATCH format.**

## 4 API

[Microsoft REST API Guidelines](https://github.com/Microsoft/api-guidelines/blob/vNext/Guidelines.md)

[Semantic Versioning]:https://semver.org/
[Doxygen]:https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html

