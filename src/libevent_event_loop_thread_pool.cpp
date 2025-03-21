#include "libevent_event_loop_thread_pool.h"

namespace MINIRPC {


    LibeventEventLoopThreadPool::LibeventEventLoopThreadPool(LibeventEventLoop *baseLoop, int numThreads)
            : baseLoop_(baseLoop), started_(false), numThreads_(numThreads), next_(0) {}

    LibeventEventLoopThreadPool::~LibeventEventLoopThreadPool() {}

    void LibeventEventLoopThreadPool::start() {
        baseLoop_->assertInLoopThread();
        started_ = true;
        for (int i = 0; i < numThreads_; ++i) {
            auto thread = std::make_unique<LibeventEventLoopThread>();
            loops_.push_back(thread->startLoop());
            threads_.push_back(std::move(thread));
        }
    }

    LibeventEventLoop *LibeventEventLoopThreadPool::getNextLoop() {
        baseLoop_->assertInLoopThread();
        LibeventEventLoop *loop = baseLoop_;
        if (!loops_.empty()) {
            loop = loops_[next_];
            ++next_;
            if (static_cast<size_t>(next_) >= loops_.size()) {
                next_ = 0;
            }
        }
        return loop;
    }
}