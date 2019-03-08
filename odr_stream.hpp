#ifndef LIVEODR_ODR_STREAM_HPP
#define LIVEODR_ODR_STREAM_HPP

#include "tools/threads/standard_task_thread.hpp"
#include "recognizers/threaded_recognizer.hpp"

#include <opencv2/core.hpp>

#include <string>
#include <memory>

namespace liveodr {
    namespace odr_stream {
        namespace {
            template <class ItemTag>
            struct InputItem {
                cv::Mat frame;
                ItemTag tag;
            };

            template <class ItemTag>
            struct OutputItem {
                cv::Mat frame;
                std::vector<std::pair<cv::Rect, std::string>> detections;
                ItemTag tag;
            };

            class EmptyClass {};
        }

        template <class Detector, class Tracker, class Recognizer, class ItemTag, class TaskThread>
        class ODRStream : virtual public TaskThread {
        public:
            using detector_type = Detector;
            using tracker_type = Tracker;
            using recognizer_type = Recognizer;
            using threaded_recognizer_type = ThreadedRecognizer<Recognizer, std::size_t>;
            using input_type = typename TaskThread::input_type;
            using output_type = typename TaskThread::output_type;
            using task_thread_type = TaskThread;

            ODRStream() { 
                state = state_type::detecting;
            }

            ~ODRStream() { }

            const auto& get_detector() const {
                return detector;
            }

            const auto& get_tracker() const {
                return tracker;
            }

            /*const auto& get_recognizer() const {
                return recognizer->get_recognizer();
            }*/

        private:
            void detect() {
                tracker.clear();
                tracker.update(current_frame);
                for(auto&& rect : detector.process(current_frame)) {
                    tracker.add(rect & cv::Rect(0, 0, current_frame.cols, current_frame.rows));
                }
            }

            void track() {
                tracker.update(current_frame);
            }

            void run(input_type& input, output_type& output) override {
                auto start = std::chrono::steady_clock::now();
            
                input.frame.copyTo(current_frame);

                state_type current_state = state;

                /* state process */
                switch(current_state) {
                    case state_type::ignore: break;
                    case state_type::detecting: detect(); break;
                    case state_type::tracking: track(); break;
                }

                /* post processing */
                switch(current_state) {
                    case state_type::ignore:
                        --frame_count;
                        if(frame_count == 0)
                            state = state_type::detecting;
                    break;
                    case state_type::detecting:
                        if(tracker.count() == 0) {
                            state = state_type::ignore;
                            frame_count = 2;
                        } else {
                            state = state_type::tracking;
                            frame_count = 4;
                        }                
                    break;
                    case state_type::tracking:
                        --frame_count;
                        if(frame_count == 0)
                            state = state_type::detecting;
                    break;
                }

                input.frame.copyTo(output.frame);
                output.tag = input.tag;

                typename threaded_recognizer_type::input_type rec_input;
                rec_input.frame = output.frame;
                for(const auto& item : tracker.get()) {
                    if(item.active) {
                        rec_input.rects.push_back(item.rect);
                        rec_input.tag = frame_number;
                        recognizer.enqueue(rec_input); //rec.confidence > 0.6 ? rec.label : "unknown"
                        output.detections.emplace_back(item.rect, "failed");
                    }
                }

                while(recognizer.pending_output()) {
                    typename threaded_recognizer_type::output_type rec_output;
                    recognizer.dequeue(rec_output);

                    // TODO
                }

                auto end = std::chrono::steady_clock::now();
                int interval = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                std::cout << "Pending >> input: " << this->pending_input() << ", output: " << this->pending_output() << std::endl;
                std::cout << "Frame Processing Time (" << (current_state == state_type::detecting ? "detecting" : "tracking") << "): " << interval << std::endl;
            
                frame_number++;
            }

        private:
            enum class state_type {
                none,
                ignore,
                detecting,
                tracking
            } state;
            int frame_count;

            cv::Mat current_frame;

            std::size_t frame_number;

        private:
            detector_type detector;
            tracker_type tracker;
            threaded_recognizer_type recognizer;
        };
    }    

    template <class Detector, class Tracker, class Recognizer, class ItemTag = odr_stream::EmptyClass,
              template <class, class> class TaskThreadTemplate = tools::StandardTaskThread>
    using ODRStream = odr_stream::ODRStream<Detector, Tracker, Recognizer, ItemTag,
                                      TaskThreadTemplate<
                                            odr_stream::InputItem<ItemTag>,
                                            odr_stream::OutputItem<ItemTag>>>;
}

#endif /* ifndef LIVEODR_ODRSTREAM_HPP */