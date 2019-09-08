#include "object_detector.h"
#include <boost/assert.hpp>

ObjectDetector::ObjectDetector(std::string model_config, std::string model_weights, float conf_threshold, float nms_threshold)
    : _conf_threshold(conf_threshold),
      _nms_threshold(nms_threshold),
      _object_detector(std::make_unique<cv::dnn::Net>(cv::dnn::readNetFromDarknet(model_config, model_weights)))
{
    // Assert that object detector has been instantiated.
    _object_detector->setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    _object_detector->setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    BOOST_ASSERT(!_object_detector->empty());
}

void ObjectDetector::Detect(cv::Mat &frame)
{
    // Prepare frame for feeding into the object detection model.
    cv::Mat blob;
    cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(416, 416), cv::Scalar(0, 0, 0), true, false);
    _object_detector->setInput(blob);

    // Run object detector on video frame.
    std::vector<cv::Mat> outs;
    _object_detector->forward(outs, _object_detector->getUnconnectedOutLayersNames());

    // Process model outputs and draw detections on frame.
    Postprocess(frame, outs);
}

// Removes the detections with low confidence and suppress detections that are overalpping.
// Method taken from the following tutorial material:
// https://github.com/spmallick/learnopencv/blob/8fca2653ae1cfe55afa0d9657df26791db5d140e/ObjectDetection-YOLO/object_detection_yolo.cpp#L159
void ObjectDetector::Postprocess(cv::Mat &frame, const std::vector<cv::Mat> &outs)
{
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float *data = (float *)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            cv::Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > _conf_threshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }

    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, _conf_threshold, _nms_threshold, indices);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        cv::Rect box = boxes[idx];
        DrawPred(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, frame);
    }
}

// Draws detected object bounding boxes on supplied video frame.
// Method taken from the following tutorial material:
// https://github.com/spmallick/learnopencv/blob/8fca2653ae1cfe55afa0d9657df26791db5d140e/ObjectDetection-YOLO/object_detection_yolo.cpp#L208
void ObjectDetector::DrawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat &frame)
{
    //Draw a rectangle displaying the bounding box
    cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(255, 178, 50), 3);
}