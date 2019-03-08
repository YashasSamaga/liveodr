#ifndef LIVEODR_TRACKERS_MEDIAN_FLOW_HPP
#define LIVEODR_TRACKERS_MEDIAN_FLOW_HPP

#include "opencv_tracker.hpp"

namespace liveodr {
    using MedianFlowTracker = liveodr::OpenCVTracker<cv::TrackerMedianFlow>;
}

#endif /* ifndef LIVEODR_TRACKERS_MEDIAN_FLOW_HPP */