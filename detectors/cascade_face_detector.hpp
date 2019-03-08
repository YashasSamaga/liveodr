#ifndef LIVEODR_DETECTORS_CASCADE_FACE_DETECTOR_HPP
#define LIVEODR_DETECTORS_CASCADE_FACE_DETECTOR_HPP

#include "detector.hpp"

#include <opencv2/objdetect/objdetect.hpp>

#include <string_view>
#include <string>
#include <vector>

namespace liveodr {
    class CascadeFaceDetector : virtual public IDetector {
    public:
        CascadeFaceDetector() {
            load("data/haarcascades/haarcascade_frontalface_alt2.xml");
        }

        CascadeFaceDetector(std::string_view model) {
            load(model);
        }

        void load(std::string_view model) {
            assert(classifier.load(std::string(model))); // TODO EXCEPTION
        }

        const std::vector<cv::Rect> process(cv::Mat input) override {
            cv::Mat input_grey;
            cv::cvtColor(input, input_grey, cv::COLOR_BGR2GRAY);
            cv::equalizeHist(input_grey, input_grey);

            std::vector<cv::Rect> rects;
            classifier.detectMultiScale(input_grey, rects, 1.1, 2, 0, cv::Size(32, 32));
            return rects;
        };

    private:
        cv::CascadeClassifier classifier;
    };
}

#endif /* ifndef LIVEODR_DETECTORS_CASCADE_FACE_DETECTOR_HPP */