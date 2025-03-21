// 异步写日志队列
// Asynchronous log writing queue

#ifndef MNRPC_LOCKQUEUE_H
#define MNRPC_LOCKQUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template<typename T>
class cLockQueue{
public:
    void push(const T &data){
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_condition_variable.notify_one();
    }

    T pop(){
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty()){
            m_condition_variable.wait(lock);
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition_variable;
};

#endif //MNRPC_LOCKQUEUE_H
