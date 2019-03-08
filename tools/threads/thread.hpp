#ifndef LIVEODR_TOOLS_THREADS_THREAD_HPP
#define LIVEODR_TOOLS_THREADS_THREAD_HPP

#include <cstddef>

namespace liveodr::tools {
    template <class InputTaskItem, class OutputTaskItem>
    class ITaskThread {
    public:
        using input_type = InputTaskItem;
        using output_type = OutputTaskItem;

        virtual void start();
        virtual void stop();

        virtual std::size_t pending_input() const = 0;
        virtual std::size_t pending_output() const = 0;

        virtual void enqueue(InputTaskItem&) = 0;
        virtual void dequeue(OutputTaskItem&) = 0;
        virtual void run(InputTaskItem&, OutputTaskItem&) = 0;
    };
}

#endif /* ifndef LIVEODR_TOOLS_THREADS_THREAD_HPP */