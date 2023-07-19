# unraider happy

## 当前支持版本

unRAID 6.11.5、unRAID 6.12.2、unRAID 6.12.3

## 开心教程

请支持正版！！

请支持正版！！

请支持正版！！

-   使用unraid官方创建工具`Unraid.USB.Creator`将原版unraid写入U盘中。
-   将`unraider-x.x.x`复制到U盘`/config`目录，**并重命名为`unraider`**。
-   使用`UnraidTool.exe`获取U盘GUID，当然这一步并不是必须的因为破解补丁中存在自动获取GUID代码。

![image-20230714212359594](https://img-blog.csdnimg.cn/1870d23af2e24c2db16346a1e6e3eaeb.png)

-   修改`/config`目录下的`go`文件，注意将`<<HERE YOUR GUID>>`修改为上一步的GUID。

    ```
    #!/bin/bash
    export UNRAID_GUID= <<HERE YOUR GUID>>
    export UNRAID_NAME=Tower
    export UNRAID_DATE=1654646400
    export UNRAID_VERSION=Pro
    /lib64/ld-linux-x86-64.so.2 /boot/config/unraider
    # Start the Management Utility
    /usr/local/sbin/emhttp &     
    ```

-   开心！！

## unRAID 6.12.2 开心版说明

​		由于unRAID 6.12.2更新了验证方法，unRAID 6.11.5补丁无法通用，通过对最新开心版补丁的逆向分析，发现作者使用了`subhook`拦截`RSA_public_decrypt`函数调用，因此我重新修改了源码并重新编译为`unraider`，供参考使用。**经测试unRAID 6.12.2可以直接升级至unRAID 6.12.3而不需要重新破解。**

![image-20230718195332209](https://img-blog.csdnimg.cn/564865524d7f43edb64df7e6ece50cfc.png)

## 原文链接

https://softoroom.org/ptopic89043.html
