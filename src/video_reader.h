#ifndef VIDEO_READER_H
#define VIDEO_READER_H

#include <memory>
#include <string>

#include <opencv2/opencv.hpp>

// Class that manages the opening of video files and
// the reading of individual frames.
class VideoReader
{
public:
  // Constructs VideoReader given path to video file.
  VideoReader(std::string file_name);

  // Reads a frame from the video file and fills
  // supplied cv::Mat with contents of the frame.
  // Returns a boolean indicating of reading a new
  // frame was successful.
  bool GrabNextFrame(cv::Mat &frame);

  // Returns frame rate of underlying video file.
  double GetFrameRate() { return _frame_rate; }

  // Returns video codec format of underlying video file.
  int GetFourCC() { return _four_cc; }

  // Returns the size (width, height) of  video frame.
  cv::Size GetFrameSize() { return _frame_size; }

private:
  // Unique pointer to cv::VideoCapture object that manages life-cycle of video file.
  std::unique_ptr<cv::VideoCapture> _vcapture;

  // Video file frame rate.
  double _frame_rate = 0;

  // Video file codec format.
  int _four_cc = 0;

  // Video file frame size.
  cv::Size _frame_size;
};

#endif