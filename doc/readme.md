# MNRPC

该项目为基于 C++ 开发的轻量级高性能远程过程调用（RPC）框架，借助 libevent 实现高效的事件驱动网络通信，采用 Google Protocol Buffers 进行数据序列化，具备跨平台、高并发处理能力，适用于分布式系统中的服务调用。

### 依赖环境

- **CMake**：版本要求 3.16 及以上

- **Protobuf**：Google Protocol Buffers 库

- **libevent**：高性能的事件通知库

### 结构

doc：文档资料

lib：项目编译生成的库文件

build：项目的构建目录，存放 CMake 生成的中间文件和编译过程中产生的临时文件

bin：存放编译生成的可执行文件

test：项目的测试代码

example：示例代码

src：源目录

### 构建

运行项目根目录 autobuild.sh

