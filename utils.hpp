#ifndef LIVEODR_UTILS_HPP
#define LIVEODR_UTILS_HPP

#include <dlib/geometry/rectangle.h>
#include <opencv2/core/types.hpp>

namespace liveodr::utils {
  /* Source: https://stackoverflow.com/a/34873134/1935009 */
  static cv::Rect dlib_to_opencv_rect(dlib::rectangle r) {
    return cv::Rect(cv::Point2i(r.left(), r.top()), cv::Point2i(r.right() + 1, r.bottom() + 1));
  }

  static dlib::rectangle opencv_to_dlib_rect(cv::Rect r) {
    return dlib::rectangle((long)r.tl().x, (long)r.tl().y, (long)r.br().x - 1, (long)r.br().y - 1);
  }
}

#endif /* ifndef LIVEODR_UTILS_HPP */