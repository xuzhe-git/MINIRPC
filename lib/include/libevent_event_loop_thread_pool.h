#ifndef MNRPC_LIBEVENT_EVENT_LOOP_THREAD_POOL_H
#define MNRPC_LIBEVENT_EVENT_LOOP_THREAD_POOL_H

#include <vector>
#include <memory>
#include "libevent_event_loop.h"
#include "libevent_event_loop_thread.h"

namespace MINIRPC {
    class LibeventEventLoopThreadPool {
    public:
        LibeventEventLoopThreadPool(LibeventEventLoop *baseLoop, int numThreads);

        ~LibeventEventLoopThreadPool();

        void start();

        LibeventEventLoop *getNextLoop();

    private:
        LibeventEventLoop *baseLoop_;
        bool started_;
        int numThreads_;
        int next_;
        std::vector<std::unique_ptr<LibeventEventLoopThread>> threads_;
        std::vector<LibeventEventLoop *> loops_;
    };
}
#endif //MNRPC_LIBEVENT_EVENT_LOOP_THREAD_POOL_H
