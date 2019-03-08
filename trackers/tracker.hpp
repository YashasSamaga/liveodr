#ifndef LIVEODR_TRACKERS_TRACKER_HPP
#define LIVEODR_TRACKERS_TRACKER_HPP

#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>

namespace liveodr {
    class ITracker {
    public:
        struct tracking_item_t {
            tracking_item_t(cv::Rect2d rect) : rect(rect) { active = true; }
            cv::Rect2d rect;
            bool active;
        };
        
        virtual std::size_t size() const = 0;
        virtual std::size_t count() const = 0;
        virtual void clear() = 0;

        virtual bool add(cv::Rect rect) = 0;
        virtual void update(cv::Mat) = 0;    
        virtual const std::vector<tracking_item_t>& get() const = 0; 
    };
}

#endif /* ifndef LIVEODR_TRACKERS_TRACKER_HPP */