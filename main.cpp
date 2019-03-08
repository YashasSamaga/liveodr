#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "detectors/frontal_face_detector.hpp"
#include "detectors/mmod_face_detector.hpp"
#include "detectors/cascade_face_detector.hpp"
#include "detectors/resnet_ssd_face_detector.hpp"

#include "recognizers/dlib_face_recognizer.hpp"

#include "trackers/boosting.hpp"
#include "trackers/CSRT.hpp"
#include "trackers/GOTURN.hpp"
#include "trackers/KCF.hpp"
#include "trackers/median_flow.hpp"
#include "trackers/MIL.hpp"
#include "trackers/MOSSE.hpp"
#include "trackers/TLD.hpp"

#include "odr_stream.hpp"

template <class DetectorType, class TrackerType, class RecognizerType>
class Wrapper {
public:
    Wrapper() : m_pending{0} { };

    void insert(cv::Mat frame) {
        typename stream_type::input_type input;
        input.frame = frame;
        stream.enqueue(input);

        m_pending++;
    }

    bool try_extract(cv::Mat& output) {
        if(!stream.pending_output()) 
            return false;
        
        typename stream_type::output_type result;
        stream.dequeue(result);

        output = result.frame;
            
        for(auto&& d : result.detections) {
            cv::putText(output, d.second, cv::Point2f(d.first.x, d.first.y), cv::FONT_HERSHEY_COMPLEX_SMALL, 3.0f, cv::Scalar(0, 255, 0));
            cv::rectangle(output, d.first, cv::Scalar(0, 255, 0), 3);
        }

        m_pending--;
        return true;
    }

    std::size_t pending() const { return m_pending; }

private:
    using stream_type = liveodr::ODRStream<DetectorType, TrackerType, RecognizerType>;
    stream_type stream;

    std::size_t m_pending;
};

int main(int argc, char *argv[])
{
    if(argc <= 1) {
        std::cout << "Usage: " << argv[0] << " <video>\n";
        return 0;
    }

    std::string input_file(argv[1]);
    cv::VideoCapture input(input_file);
    if(!input.isOpened()) {
        std::cout << "Failed to open " << input_file << '\n';
        return 0;
    }        

    const double fps = input.get(cv::CAP_PROP_FPS);
    const cv::Size size(input.get(cv::CAP_PROP_FRAME_WIDTH), input.get(cv::CAP_PROP_FRAME_HEIGHT));
    std::cout << "Frame Size: " << size.width << ' ' << size.height << '\n';
    std::cout << "FPS: " << fps << '\n';

    /*
    ** we dump several initial frames in advance so that the ODRStream has good number of output
    ** frames waiting in the output buffer to compensate for sudden FPS drops
     */
    Wrapper<liveodr::ResnetSSDFaceDetector, liveodr::MedianFlowTracker, liveodr::DlibFaceRecognizer> wrapper;
    for(int i = 0; i < 15; i++) {
        /* dump several frames for processing */
        cv::Mat frame;
        input >> frame;
        if(frame.empty())
            break;
        wrapper.insert(frame);        
    }
    
    /* give time for the odrstream to generate few output frames */
    std::this_thread::sleep_for(std::chrono::seconds{1});

    cv::namedWindow("output", 1);
    for(cv::Mat output; wrapper.pending();)
    {
        auto start = std::chrono::steady_clock::now();
        {
            cv::Mat frame;
            input >> frame;
            wrapper.insert(frame);

            wrapper.try_extract(output);

            if(!output.empty()) {
                /* the ODRStream has few frames ready for display */
                cv::resize(output, output, cv::Size(frame.size().width/2.0, frame.size().height/2.0));
                cv::imshow("output", output);
            } else {
                assert(!output.empty());
            }
        }        
        auto end = std::chrono::steady_clock::now();
        int interval = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        int excess = int(1000.0/fps) - interval - 1;
        cv::waitKey(excess > 0 ? excess : 1);
    }

	return 0;
}