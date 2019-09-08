#ifndef OBJECT_DETECTOR_H
#define OBJECT_DETECTOR_H

#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

// Class that manages Deep Neural Network (DNN) used to detect
// objects in a video frame.
class ObjectDetector
{
public:
  // Constructs ObjectDetector given:
  // model_config: Path to DNN model configuration file.
  // modol_weights: Path to DNN model weight files.
  // conf_threshold: Minimum DNN confidence required for an object to be marked as a detection.
  // nms_threshold: Threshold above which two overlapping detections are considered the same.
  ObjectDetector(std::string model_config, std::string model_weights, float conf_threshold = 0.5, float nms_threshold = 0.4);

  // Detects objects in a video frame.
  // Modifies supplied video frame by drawing bounding boxes around detected objects.
  void Detect(cv::Mat &frame);

private:
  // Removes the detections with low confidence and suppress detections that are overalpping.
  void Postprocess(cv::Mat &frame, const std::vector<cv::Mat> &out);

  // Draws detected object bounding boxes on supplied video frame.
  void DrawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat &frame);

  // Threshold requried for an object to be marked as a detection.
  float _conf_threshold = 0.0;
  
  // Threshold above which two overlapping detections are considered the same. 
  float _nms_threshold = 0.0;
  
  // Unique pointer cv::dnn::Net object that managed life-cycle of object detector.
  std::unique_ptr<cv::dnn::Net> _object_detector;
};

#endif