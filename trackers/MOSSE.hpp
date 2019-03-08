#ifndef LIVEODR_TRACKERS_MOSSE_HPP
#define LIVEODR_TRACKERS_MOSSE_HPP

#include "opencv_tracker.hpp"

namespace liveodr {
    using MOSSETracker = liveodr::OpenCVTracker<cv::TrackerMOSSE>;
}

#endif /* ifndef LIVEODR_TRACKERS_MOSSE_HPP */