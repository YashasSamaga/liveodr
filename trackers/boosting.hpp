#ifndef LIVEFDR_TRACKERS_BOOSTING_HPP
#define LIVEFDR_TRACKERS_BOOSTING_HPP

#include "tracker.hpp"

class BoostingTracker : public liveodr::ITracker {
public:
    BoostingTracker() = default;

    virtual std::size_t size() { return trackers.size(); }
    virtual std::size_t count() {
        return std::count_if(items.begin(), items.end(), [](const auto& item) {
            return item.active; 
        });
    }

    virtual void clear() { trackers.clear(); items.clear(); }

    virtual bool add(cv::Rect rect) {
        auto tracker = cv::TrackerBoosting::create();
        trackers.push_back(tracker);
        items.emplace_back(rect);
        items.back().active = tracker->init(current_frame, rect);
    }

    virtual void update(cv::Mat frame) {
        current_frame = frame;
        for(std::size_t i = 0; i < trackers.size(); i++) {
            if(items[i].active) {
                items[i].active = trackers[i]->update(current_frame, items[i].rect);
            }
        }
    }

    virtual const std::vector<tracking_item_t>& get() const {
        return items;
    }

private:
    cv::Mat current_frame;
    std::vector<tracking_item_t> items;
    std::vector<cv::Ptr<cv::Tracker>> trackers;
};

#endif /* ifndef LIVEFDR_TRACKERS_BOOSTING_HPP */