# EasyNet C++跨平台事件驱动型网络框架

## EasyNet是基于Muduo改造过来的，Muduo是一个Linux服务端高性能、事件驱动型的网络框架，类似于Java中的Netty，python中的Twisted.但是Muduo仅支持Linux

## EasyNet现支持的平台 Windows、Linux、Vxworks多种常用的操作系统，稍加改动就可以支持Macos，android等操作系统

## EasyNet相对于Muduo的几点区别
- 定时器的设计，Muduo使用了Linux独有的Timerfd实现的高性能定时器，Windows等其他平台并没有相应的api，故EasyNet使用了最小堆
  实现的，最小堆的顶点是所有定时器中最先超时的，所以每次只用检查最小堆顶点的定时器事件

- EasyNet在Muduo的ProtobufCodec和FieldLength编解码的基础上新增加了基于行的Codec编解码方式，有很多时候需要我们需要每次解析一行数据

- EasyNet同时支持boost和C++11，很多老的编译器是不支持C++11标准的，而EasyNet中使用了部分C++新标准中的功能，比如智能指针，函数对象相关(Function, Bind)等，基于此，EasyNet跟根据编译器的版本选择使用C++11或者Boost

- EasyNet基于protobuf实现了RPC功能

## TODO List
- 添加日志功能
- 目前IO复用是用的是比较低效的Select，后面Linux中会加入Epoll机制，这样fd管理会更加高效
- 实现固定长度的消息编解码方式
- 对MacOS等其他操作系统的支持
- 加入对UDP协议的支持


## 感谢
- Muduo是一个高性能的网络编程框架，代码简洁，屏蔽了网络编程对应用层的开发难度，应用层只用关心业务逻辑
- 感谢陈硕的高性能服务端网络编程一书，这本书阐述了Muduo的实现细节，也讲述了现代C++开发过程中的常用手段和注意事项，特别是其基于对象的工程实践给了我很多启发，再次感谢





