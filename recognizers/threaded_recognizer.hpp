#ifndef LIVEODR_RECOGNIZER_THREADED_RECOGNIZER_HPP
#define LIVEODR_RECOGNIZER_THREADED_RECOGNIZER_HPP

#include "tools/threads/standard_task_thread.hpp"

#include <opencv2/core.hpp>
#include <vector>
#include <string>

namespace liveodr {
    namespace threaded_recognizer {
        template <class ItemTag>
        struct InputItem {
            cv::Mat frame;
            std::vector<cv::Rect> rects;
            ItemTag tag;
        };

        template <class ItemTag>
        struct OutputItem {
            std::vector<std::string> detections;
            ItemTag tag;
        };

        class EmptyClass {};

        template<class Recognizer, class TaskThread>
        class ThreadedRecognizer : virtual public TaskThread {
        public:
            using recognizer_type = Recognizer;
            using input_type = typename TaskThread::input_type;
            using output_type = typename TaskThread::output_type;
            using task_thread_type = TaskThread;

            ThreadedRecognizer() = default;
            ThreadedRecognizer(const Recognizer& recognizer) : recognizer(recognizer) {}
            ThreadedRecognizer(Recognizer&& recognizer) : recognizer(recognizer) { }

            void run(input_type& input, output_type& output) override {
                //output.detections = recognizer.recognize(input.frame, input.rects);
                output.tag = input.tag;
            }

        private:
            Recognizer recognizer;
        };
    }

    template<class Recognizer, class ItemTag = threaded_recognizer::EmptyClass,
             template <class, class> class TaskThreadTemplate = tools::StandardTaskThread>
    using ThreadedRecognizer = threaded_recognizer::ThreadedRecognizer
                                <Recognizer, TaskThreadTemplate<
                                                threaded_recognizer::InputItem<ItemTag>,
                                                threaded_recognizer::OutputItem<ItemTag>>>;
}

#endif /* ifndef LIVEODR_RECOGNIZER_THREADED_RECOGNIZER_HPP */