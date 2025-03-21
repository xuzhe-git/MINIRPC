#include "libevent_event_loop.h"
#include <iostream>

namespace MINIRPC {

    LibeventEventLoop::LibeventEventLoop()
            : base_(event_base_new()), looping_(false), quit_(false), threadId_(std::this_thread::get_id()) {
        if (!base_) {
            std::cerr << "Failed to create event base!" << std::endl;
        }
    }

    LibeventEventLoop::~LibeventEventLoop() {
        if (base_) {
            event_base_free(base_);
        }
    }

    void LibeventEventLoop::loop() {
        looping_ = true;
        quit_ = false;
        while (!quit_) {
            doPendingFunctors();
            event_base_loop(base_, EVLOOP_ONCE);
        }
        looping_ = false;
    }

    void LibeventEventLoop::quit() {
        quit_ = true;
        if (base_) {
            event_base_loopbreak(base_);
        }
    }

    void LibeventEventLoop::runInLoop(const std::function<void()> &cb) {
        if (isInLoopThread()) {
            cb();
        } else {
            queueInLoop(cb);
        }
    }

    void LibeventEventLoop::queueInLoop(const std::function<void()> &cb) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pendingFunctors_.push(cb);
        }
        if (base_) {
            event_base_loopbreak(base_);
        }
    }

    bool LibeventEventLoop::isInLoopThread() const {
        return std::this_thread::get_id() == threadId_;
    }

    void LibeventEventLoop::assertInLoopThread() const {
        if (!isInLoopThread()) {
            std::cerr << "Not in the loop thread!" << std::endl;
            abort();
        }
    }

    void LibeventEventLoop::doPendingFunctors() {
        std::vector<std::function<void()>> functors;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            while (!pendingFunctors_.empty()) {
                functors.push_back(std::move(pendingFunctors_.front()));
                pendingFunctors_.pop();
            }
        }
        for (const auto &functor: functors) {
            functor();
        }
    }

    struct event_base *LibeventEventLoop::getEventBase() const {
        return base_;
    }

}