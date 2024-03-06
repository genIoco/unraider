# unraider happy

### 说明

本项目源码基于softoroom破解补丁提取而来，[详见原文](https://softoroom.org/ptopic89043.html)

## 当前支持版本

unRAID <= 6.12.10

## 开心教程

请支持正版！！

请支持正版！！

请支持正版！！

-   删除了补丁具体使用方法，因为我们还是需要支持正版
-   本文源码信息仅作为技术交流使用，有能力的朋友还是需要自己研究
-   有彩蛋

## unRAID 6.12.2 开心版说明

​		由于unRAID 6.12.2更新了验证方法，unRAID 6.11.5补丁无法通用，通过对最新开心版补丁的逆向分析，发现作者使用了`subhook`拦截`RSA_public_decrypt`函数调用，因此我重新修改了源码并重新编译为`unraider`，供参考使用。**经测试unRAID 6.12.2版本补丁可支持到unRAID 6.12.6而不需要重新破解，详见[#1](https://github.com/genIoco/unraider/issues/1)。**

![image-20230718195332209](https://img-blog.csdnimg.cn/564865524d7f43edb64df7e6ece50cfc.png)

## unRAID 6.12.8 开心版说明

由于unRAID 6.12.8更新了验证方法，unRAID 6.12.2补丁无法通用，通过对最新开心版补丁的逆向分析，重新修改了源码并重新编译为`unraider`，供参考使用，编译时请选择6.12.8选项进行编译，各位且用且珍惜。**经测试unRAID 6.12.8版本补丁可支持到unRAID 6.12.10而不需要重新破解。**
## 原文链接

https://softoroom.org/ptopic89043.html
