#ifndef LIVEODR_TRACKERS_MIL_HPP
#define LIVEODR_TRACKERS_MIL_HPP

#include "opencv_tracker.hpp"

namespace liveodr {
    using MILTracker = liveodr::OpenCVTracker<cv::TrackerMIL>;
}

#endif /* ifndef LIVEODR_TRACKERS_MIL_HPP */