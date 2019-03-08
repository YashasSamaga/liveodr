#ifndef LIVEODR_RECOGNIZERS_RECOGNIZER_HPP
#define LIVEODR_RECOGNIZERS_RECOGNIZER_HPP

#include <opencv2/core.hpp>

#include <vector>

namespace liveodr {
    class IRecognizer {
    public:
        struct result_type {
            std::string label;
            float confidence;
        };

        virtual const result_type recognize(const cv::Mat sample, cv::Rect) = 0;
        virtual const std::vector<result_type> recognize(const cv::Mat sample, std::vector<cv::Rect>) = 0;
    };
}

#endif /* ifndef LIVEODR_RECOGNIZERS_RECOGNIZER_HPP */