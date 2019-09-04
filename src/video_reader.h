#ifndef VIDEO_READER_H
#define VIDEO_READER_H

#include <memory>
#include <string>

#include <opencv2/opencv.hpp>

class VideoReader
{
  public:
    VideoReader(std::string file_name)
        : _vcapture(std::make_unique<cv::VideoCapture>(file_name)) {}

    void grabNextFrame(cv::Mat &frame);

  private:
    std::unique_ptr<cv::VideoCapture> _vcapture;
};

#endif