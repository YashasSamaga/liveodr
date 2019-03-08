#ifndef LIVEODR_TOOLS_THREADS_FAKE_TASK_THREAD_HPP
#define LIVEODR_TOOLS_THREADS_FAKE_TASK_THREAD_HPP

#include "thread.hpp"

namespace liveodr::tools {
    template <class InputTaskItem, class OutputTaskItem>
    class FakeTaskThread : virtual public ITaskThread<InputTaskItem, OutputTaskItem> {
    public:
        FakeTaskThread() { }
        ~FakeTaskThread() { }

        void start() override { }

        void stop() override { }

        std::size_t pending_input() const override {
            return 0;
        }

        std::size_t pending_output() const override {
            return output.queue.size();
        }

        void enqueue(InputTaskItem& input_item) override {
            OutputTaskItem output_item;
            run(input_item, output_item);

            std::unique_lock<std::mutex> lock(output.lock);
            output.queue.push(output_item);
            output.not_empty.notify_one();
        }

        void dequeue(OutputTaskItem& item) override {
            std::unique_lock<std::mutex> lock(output.lock);
            output.not_empty.wait(lock, [this](){ return !this->output.queue.empty(); });
            item = output.queue.front();
            output.queue.pop();
        }

        virtual void run(InputTaskItem& input, OutputTaskItem& output) = 0;

    protected:
        struct {
            std::queue<OutputTaskItem> queue;
            mutable std::condition_variable not_empty;
            mutable std::mutex lock;
        } output;    };
}

#endif /* ifndef LIVEODR_TOOLS_THREADS_FAKE_TASK_THREAD_HPP */