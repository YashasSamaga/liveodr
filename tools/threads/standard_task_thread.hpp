#ifndef LIVEODR_TOOLS_STANDARD_TASK_THREAD_HPP
#define LIVEODR_TOOLS_STANDARD_TASK_THREAD_HPP

#include "thread.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

namespace liveodr::tools {
    template <class InputTaskItem, class OutputTaskItem>
    class StandardTaskThread : virtual public ITaskThread<InputTaskItem, OutputTaskItem> {
    public:
        StandardTaskThread() { start(); }
        ~StandardTaskThread() { stop(); }

        void start() override {
            stop_thread.store(false);
            worker = std::thread(&StandardTaskThread::internal_run, this);
        }

        void stop() override { 
            stop_thread.store(true);
            worker.join();
        }

        std::size_t pending_input() const override {
            std::lock_guard<std::mutex> lock(input.lock);
            return input.queue.size();
        }

        std::size_t pending_output() const override {
            std::lock_guard<std::mutex> lock(output.lock);
            return output.queue.size();
        }

        void enqueue(InputTaskItem& item) override {
            std::unique_lock<std::mutex> lock(input.lock);
            input.queue.push(item);
            input.not_empty.notify_one();
            lock.unlock();            
        }

        void dequeue(OutputTaskItem& item) override {
            std::unique_lock<std::mutex> lock(output.lock);
            output.not_empty.wait(lock, [this](){ return !this->output.queue.empty(); });
            item = output.queue.front();
            output.queue.pop();
        }

        virtual void run(InputTaskItem& input, OutputTaskItem& output) = 0;

    protected:        
        void internal_run() { 
            while(!stop_thread.load()) {
                InputTaskItem input_item;
                
                {
                    std::unique_lock<std::mutex> lock(input.lock);
                    input.not_empty.wait(lock, [this](){ return !this->input.queue.empty(); });
                    input_item = input.queue.front();
                    input.queue.pop();
                }
                
                OutputTaskItem output_item;
                run(input_item, output_item);
            
                {
                    std::unique_lock<std::mutex> lock(output.lock);
                    output.queue.push(output_item);
                    output.not_empty.notify_one();
                }                           
            }
        }

    protected:
        std::thread worker;
        std::atomic<bool> stop_thread;

        struct {
            std::queue<InputTaskItem> queue;
            mutable std::condition_variable not_empty;
            mutable std::mutex lock;
        } input;

        struct {
            std::queue<OutputTaskItem> queue;
            mutable std::condition_variable not_empty;
            mutable std::mutex lock;
        } output;
    };
}

#endif /* ifndef LIVEODR_TOOLS_STANDARD_TASK_THREAD_HPP */