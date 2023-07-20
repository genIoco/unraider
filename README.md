# unraider happy

### 说明

本项目源码基于softoroom破解补丁提取而来，[详见原文](https://softoroom.org/ptopic89043.html)

## 当前支持版本

unRAID 6.11.5、unRAID 6.12.2、unRAID 6.12.3

## 开心教程

请支持正版！！

请支持正版！！

请支持正版！！

-   删除了补丁具体使用方法，因为我们还是需要支持正版
-   本文源码信息仅作为技术交流使用，有能力的朋友还是需要自己研究

## unRAID 6.12.2 开心版说明

​		由于unRAID 6.12.2更新了验证方法，unRAID 6.11.5补丁无法通用，通过对最新开心版补丁的逆向分析，发现作者使用了`subhook`拦截`RSA_public_decrypt`函数调用，因此我重新修改了源码并重新编译为`unraider`，供参考使用。**经测试unRAID 6.12.2可以直接升级至unRAID 6.12.3而不需要重新破解。**

![image-20230718195332209](https://img-blog.csdnimg.cn/564865524d7f43edb64df7e6ece50cfc.png)

## 原文链接

https://softoroom.org/ptopic89043.html
