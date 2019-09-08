#include <boost/assert.hpp>

#include "video_reader.h"

VideoReader::VideoReader(std::string file_name) : _vcapture(std::make_unique<cv::VideoCapture>(file_name))
{
    // Assert that video file has been successfully opened.
    BOOST_ASSERT(_vcapture->isOpened());

    // Determine video file frame rate.
    _frame_rate = _vcapture->get(cv::CAP_PROP_FPS);

    // Determine video file codec format.
    _four_cc = _vcapture->get(cv::CAP_PROP_FOURCC);

    // Determine video file frame size.
    _frame_size = cv::Size(
        _vcapture->get(cv::CAP_PROP_FRAME_WIDTH),
        _vcapture->get(cv::CAP_PROP_FRAME_HEIGHT));
}

bool VideoReader::GrabNextFrame(cv::Mat &frame)
{
    // Use wrapped cv::VideoCaptuer object to read frame and
    // determine success of read operation.
    return _vcapture->read(frame);
}