#ifndef LIVEODR_DETECTORS_MMOD_FACE_DETECTOR_HPP
#define LIVEODR_DETECTORS_MMOD_FACE_DETECTOR_HPP

#include "detector.hpp"
#include "../utils.hpp"

#include <dlib/dnn.h>
#include <dlib/opencv.h>

#include <string_view>
#include <string>
#include <vector>

namespace liveodr {
    class MMODFaceDetector : virtual public IDetector {
    public:
        MMODFaceDetector() {
            load("data/mmod_human_face_detector.dat");
        }

        MMODFaceDetector(std::string_view model) {
            load(model);
        }
    
        void load(std::string_view model) {
            dlib::deserialize(std::string(model)) >> detector;  
        }

        const std::vector<cv::Rect> process(cv::Mat input) override {
            cv::Mat input_rgb;
            cv::cvtColor(input, input_rgb, cv::COLOR_BGR2RGB);

            dlib::matrix<dlib::rgb_pixel> converted_input;
            dlib::assign_image(converted_input, dlib::cv_image<dlib::rgb_pixel>(input_rgb));

            std::vector<cv::Rect> rects;
            for(auto&& r : detector(converted_input))
                if(r.detection_confidence  > 0.8)
                    rects.push_back(liveodr::utils::dlib_to_opencv_rect(r.rect));
            return rects;
        };

    private:
        template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
        template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

        template <typename SUBNET> using downsampler  = dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16,SUBNET>>>>>>>>>;
        template <typename SUBNET> using rcon5  = dlib::relu<dlib::affine<con5<45,SUBNET>>>;

        using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;

        net_type detector;
    };
}

#endif /* ifndef LIVEODR_DETECTORS_MMOD_FACE_DETECTOR_HPP */