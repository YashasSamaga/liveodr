#ifndef LIVEODR_DETECTORS_FRONTAL_FACE_DETECTOR_HPP
#define LIVEODR_DETECTORS_FRONTAL_FACE_DETECTOR_HPP

#include "detector.hpp"
#include "../utils.hpp"

#include <opencv2/imgproc.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <vector>

namespace liveodr {
    class FrontalFaceDetector : virtual public IDetector {
    public:
        FrontalFaceDetector() {
            detector = dlib::get_frontal_face_detector();
        }

        const std::vector<cv::Rect> process(cv::Mat input) override {
            cv::Mat input_grey;
            cv::cvtColor(input, input_grey, cv::COLOR_BGR2GRAY);
            //cv::equalizeHist(input_grey, input_grey);

            dlib::cv_image<unsigned char> converted_input(input_grey);
            
            std::vector<cv::Rect> faces;
            for(auto&& r : detector(converted_input))
                faces.emplace_back(liveodr::utils::dlib_to_opencv_rect(r));
            return faces;
        };

    private:
        dlib::frontal_face_detector detector;
    };
}

#endif /* ifndef LIVEODR_DETECTORS_FRONTAL_FACE_DETECTOR_HPP */