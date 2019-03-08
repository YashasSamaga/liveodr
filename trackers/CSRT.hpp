#ifndef LIVEFDR_TRACKERS_CSRT_HPP
#define LIVEFDR_TRACKERS_CSRT_HPP

#include "opencv_tracker.hpp"

namespace liveodr {
    using CSRTTracker = OpenCVTracker<cv::TrackerCSRT>;
}

#endif /* ifndef LIVEFDR_TRACKERS_CSRT_HPP */