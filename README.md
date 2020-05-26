# libevent
 学习libevent。使用版本：libevent-2.0.20-stable

# libevent安装：
1. 在官网上下载对应版本的包
2. tar -zxvf /your path/下载的包.tar.gz解压到当前目录
3. cd 解压后的目录
4. ./configure
5. make
6. sudo make install
7. 在/usr/local/lib目录下应该可以看见大量的动态链接库了,这时运行ln -s /usr/local/lib/libevent-1.4.so.2 /usr/lib/libevent-1.4.so.2命令

# 编译
gcc 文件 -levent
g++ 文件 -levent
(-levent是为了链接动态库）

# 多文件编译
chenxinyu@DESKTOP-0TM873U:/mnt/d/cProject/libeventTest/httpServer$ g++ -c User.cpp

chenxinyu@DESKTOP-0TM873U:/mnt/d/cProject/libeventTest/httpServer$ g++ httpServerMe.o User.o -o test -levent

chenxinyu@DESKTOP-0TM873U:/mnt/d/cProject/libeventTest/httpServer$ ./test

# 参考：
https://aceld.gitbooks.io/libevent/content/               libevent深入浅出

http://www.wangafu.net/~nickm/libevent-2.0/doxygen/html/  API

https://www.wandouip.com/t5i261706/                       博文
