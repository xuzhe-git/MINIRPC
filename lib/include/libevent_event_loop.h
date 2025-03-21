#ifndef MNRPC_LIBEVENT_EVENT_LOOP_H
#define MNRPC_LIBEVENT_EVENT_LOOP_H

#include <event2/event.h>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include <event2/event.h>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace MINIRPC {
    class LibeventEventLoop {
    public:
        LibeventEventLoop();

        ~LibeventEventLoop();

        void loop();

        void quit();

        void runInLoop(const std::function<void()> &cb);

        void queueInLoop(const std::function<void()> &cb);

        bool isInLoopThread() const;

        void assertInLoopThread() const;

        // 添加 getEventBase 方法
        struct event_base *getEventBase() const;

    private:
        void doPendingFunctors();

        struct event_base *base_;
        bool looping_;
        bool quit_;
        std::mutex mutex_;
        std::queue<std::function<void()>> pendingFunctors_;
        std::thread::id threadId_;
    };
}
#endif //MNRPC_LIBEVENT_EVENT_LOOP_H
