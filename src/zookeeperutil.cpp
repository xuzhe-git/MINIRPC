#include "zookeeperutil.h"
#include "mnrpcapplication.h"
#include <semaphore.h>
#include <iostream>

namespace MINIRPC {

    void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
        if (type == ZOO_SESSION_EVENT) {
            if (state == ZOO_CONNECTED_STATE) {
                sem_t *sem = (sem_t *) zoo_get_context(zh);
                sem_post(sem);
            }
        }
    }


    cZookeeper::cZookeeper() : m_zhandle(nullptr) {

    }

    cZookeeper::~cZookeeper() {
        if (!m_zhandle) {
            zookeeper_close(m_zhandle);
        }
    }

    void cZookeeper::start() {
        std::string host = cMnrpcApplication::GetInstance().GetConfig().Load("zookeeperip");
        std::string port = cMnrpcApplication::GetInstance().GetConfig().Load("zookeeperport");
        std::string connstr = host + ":" + port;

        m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);

        if (m_zhandle == nullptr) {
            std::cout << "zookeeper_init error!" << std::endl;
            exit(EXIT_FAILURE);
        }

        sem_t sem;
        sem_init(&sem, 0, 0);
        zoo_set_context(m_zhandle, &sem);

        sem_wait(&sem);
        std::cout << "zookeeper_init success!" << std::endl;
    }

    void cZookeeper::create(const char *path, const char *data, int datalen, int state) {
        char path_buffer[128];
        int bufferlen = sizeof(path_buffer);
        int flag;
        flag = zoo_exists(m_zhandle, path, 0, nullptr);
        if (flag == ZNONODE) {
            flag = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
            if (flag == ZOK) {
                std::cout << "znode create success, path:" << path << std::endl;
            } else {
                std::cout << "flag:" << flag << std::endl;
                std::cout << "znode create error, path:" << path << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    std::string cZookeeper::GetData(const char *path) {

        char buffer[64];
        int bufferlen = sizeof(buffer);
        int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
        if (flag != ZOK) {
            std::cout << "get znode error, path:" << path << std::endl;
            return "";
        } else {
            return buffer;
        }
    }
}