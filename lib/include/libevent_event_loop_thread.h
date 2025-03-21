#ifndef MNRPC_LIBEVENT_EVENT_LOOP_THREAD_H
#define MNRPC_LIBEVENT_EVENT_LOOP_THREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include "libevent_event_loop.h"

namespace MINIRPC {
    class LibeventEventLoopThread {
    public:
        LibeventEventLoopThread();

        ~LibeventEventLoopThread();

        LibeventEventLoop *startLoop();

    private:
        void threadFunc();

        LibeventEventLoop *loop_;
        bool exiting_;
        std::thread thread_;
        std::mutex mutex_;
        std::condition_variable cond_;
    };
}
#endif //MNRPC_LIBEVENT_EVENT_LOOP_THREAD_H
