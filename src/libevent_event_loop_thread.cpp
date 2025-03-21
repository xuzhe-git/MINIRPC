#include "libevent_event_loop_thread.h"

namespace MINIRPC {

    LibeventEventLoopThread::LibeventEventLoopThread()
            : loop_(nullptr), exiting_(false) {}

    LibeventEventLoopThread::~LibeventEventLoopThread() {
        exiting_ = true;
        if (loop_) {
            loop_->quit();
            thread_.join();
        }
    }

    LibeventEventLoop *LibeventEventLoopThread::startLoop() {
        std::thread t([this] {
            threadFunc();
        });
        thread_.swap(t);

        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (loop_ == nullptr) {
                cond_.wait(lock);
            }
        }
        return loop_;
    }

    void LibeventEventLoopThread::threadFunc() {
        LibeventEventLoop loop;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            loop_ = &loop;
            cond_.notify_one();
        }
        loop.loop();
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = nullptr;
    }

}