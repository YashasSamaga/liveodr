#ifndef LIVEODR_DETECTORS_RESNET_SSD_FACE_DETECTOR_HPP
#define LIVEODR_DETECTORS_RESNET_SSD_FACE_DETECTOR_HPP

#include "detector.hpp"

#include <opencv2/dnn.hpp>

#include <string_view>
#include <string>

namespace liveodr {
    class ResnetSSDFaceDetector : 
        virtual public liveodr::IDetector {
    public:
        ResnetSSDFaceDetector() {
            load("data/face_detector/deploy.prototxt", "data/face_detector/res10_300x300_ssd_iter_140000_fp16.caffemodel");
        }

        ResnetSSDFaceDetector(std::string_view config, std::string_view model) {
            load(config, model);
        }

        void load(std::string_view config, std::string_view model) {
            detector = cv::dnn::readNetFromCaffe(std::string(config), std::string(model));
            assert(!detector.empty());
        }

        const std::vector<cv::Rect> process(cv::Mat input) override {
            const cv::Scalar mean(104.0, 117.0, 123.0);
            double scale = 1.0f;

            cv::Mat blob = cv::dnn::blobFromImage(input, scale, cv::Size(300, 300), mean);
            detector.setInput(blob, "data");
            cv::Mat detections = detector.forward("detection_out");

            cv::Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());

            std::vector<cv::Rect> faces;
            for(int i = 0; i < detectionMat.rows; i++)
            {
                float confidence = detectionMat.at<float>(i, 2);
                if(confidence < 0.9)
                    continue;
                    
                int left_bottom_x = detectionMat.at<float>(i, 3) * input.cols,
                    left_bottom_y = detectionMat.at<float>(i, 4) * input.rows,
                    right_top_x = detectionMat.at<float>(i, 5) * input.cols,
                    right_top_y = detectionMat.at<float>(i, 6) * input.rows;
                cv::Rect rect(left_bottom_x, left_bottom_y,
                                (right_top_x - left_bottom_x),
                                (right_top_y - left_bottom_y));
                faces.push_back(rect);
            }
            return faces;
        }

    private:
        cv::dnn::Net detector;
    };
}

#endif /* ifndef LIVEODR_DETECTORS_RESNET_SSD_FACE_DETECTOR_HPP */