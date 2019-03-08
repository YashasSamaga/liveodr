#ifndef LIVEODR_TRACKERS_TLD_HPP
#define LIVEODR_TRACKERS_TLD_HPP

#include "opencv_tracker.hpp"

namespace liveodr {
    using TLDTracker = liveodr::OpenCVTracker<cv::TrackerTLD>;
}

#endif /* ifndef LIVEODR_TRACKERS_TLD_HPP */