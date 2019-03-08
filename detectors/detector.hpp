#ifndef LIVEODR_DETECTORS_DETECTOR_HPP
#define LIVEODR_DETECTORS_DETECTOR_HPP

#include <opencv2/core.hpp>
#include <vector>

namespace liveodr {
    class IDetector {
    public:       
        virtual const std::vector<cv::Rect> process(cv::Mat input) = 0;
    };
}

#endif /* ifndef LIVEODR_DETECTORS_DETECTOR_HPP */