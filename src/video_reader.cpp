
#include "video_reader.h"

void VideoReader::grabNextFrame(cv::Mat& frame) {
    (*_vcapture) >> frame;
}