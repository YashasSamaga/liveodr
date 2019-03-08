#ifndef LIVEFDR_TRACKERS_KCF_HPP
#define LIVEFDR_TRACKERS_KCF_HPP
#include "opencv_tracker.hpp"

namespace liveodr {
    using KCFTracker = liveodr::OpenCVTracker<cv::TrackerKCF>;
}

#endif /* ifndef LIVEFDR_TRACKERS_KCF_HPP */