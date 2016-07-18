
#ifndef CNOID_UTIL_THREAD_POOL_H
#define CNOID_UTIL_THREAD_POOL_H

#include <queue>
#include <boost/thread.hpp>
#include <boost/function.hpp>

namespace cnoid {

class ThreadPool
{
private:
    std::queue<boost::function<void()> > queue;
    boost::thread_group group;
    boost::mutex mutex;
    boost::condition_variable condition;
    boost::condition_variable finishCondition;
    int numActiveThreads;
    int size_;
    bool isDestroying;
        
public:
    ThreadPool(int size = 1) {
        size_ = size;
        numActiveThreads = 0;
        isDestroying = false;
        for(int i = 0; i < size; ++i){
            group.create_thread(boost::bind(&ThreadPool::run, this));
        }
    }

    ~ThreadPool() {
        {
            boost::mutex::scoped_lock lock(mutex);
            isDestroying = true;
            condition.notify_all();
        }
        group.join_all();
    }

    int size() const { return size_; }
        
    void start(boost::function<void()> f) {
        boost::mutex::scoped_lock lock(mutex);
        queue.push(f);
        condition.notify_one();
    }

    void wait(){
        boost::mutex::scoped_lock lock(mutex);
        while(!queue.empty() || numActiveThreads > 0){
            finishCondition.wait(lock);
        }
    }
        
private:
    void run() {
        while(true){
            boost::function<void()> f;
            {
                boost::mutex::scoped_lock lock(mutex);

                while (queue.empty() && !isDestroying){
                    condition.wait(lock);
                }
                if(!queue.empty()){
                    f = queue.front();
                    queue.pop();
                    ++numActiveThreads;
                }
            }
            if(f){
                f();
                {
                    boost::mutex::scoped_lock lock(mutex);
                    --numActiveThreads;
                    if(numActiveThreads == 0){
                        finishCondition.notify_all();
                    }
                }
            } else {
                break;
            }
        }
    }
};

}
    
#endif
