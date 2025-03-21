#ifndef MNRPC_ZOOKEEPERUTIL_H
#define MNRPC_ZOOKEEPERUTIL_H

#include <zookeeper/zookeeper.h>
#include <string>
#include <semaphore.h>

namespace MINIRPC {

    class cZookeeper {
    public:
        cZookeeper();

        ~cZookeeper();

        // 连接zkserver
        //Connect zkserver
        void start();

        // 根据path创建znode结点
        //Create znode node according to path
        void create(const char *path, const char *data, int datalen, int state = 0);

        // path，获取值
        //Path, get value
        std::string GetData(const char *path);

    private:
        //zk的客户端句柄
        //ZK's client handle
        zhandle_t *m_zhandle;
    };
}
#endif //MNRPC_ZOOKEEPERUTIL_H
