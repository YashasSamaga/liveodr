#ifndef LIVEODR_TRACKERS_OPENCV_TRACKER_HPP
#define LIVEODR_TRACKERS_OPENCV_TRACKER_HPP

#include "tracker.hpp"

#include <cstddef>
#include <vector>
#include <opencv2/core.hpp>

namespace liveodr {
    template <class OCVTracker>
    class OpenCVTracker : public ITracker {
    public:
        std::size_t size() const override { return trackers.size(); }
        std::size_t count() const override {
            return std::count_if(items.begin(), items.end(), [](const auto& item) {
                return item.active; 
            });
        }

        void clear() override { trackers.clear(); items.clear(); }

        bool add(cv::Rect rect) override {
            auto tracker = OCVTracker::create();
            trackers.push_back(tracker);
            items.emplace_back(rect);
            items.back().active = tracker->init(current_frame, rect);
        }

        void update(cv::Mat frame) override {
            current_frame = frame;
            for(std::size_t i = 0; i < trackers.size(); i++) {
                if(items[i].active) {
                    items[i].active = trackers[i]->update(current_frame, items[i].rect);
                }
            }
        }

        const std::vector<tracking_item_t>& get() const override { //TODO dangling reference
            return items;
        }

    private:
        cv::Mat current_frame;
        std::vector<tracking_item_t> items;
        std::vector<cv::Ptr<cv::Tracker>> trackers;
    };
}

#endif /* ifndef LIVEODR_TRACKERS_OPENCV_TRACKER_HPP */