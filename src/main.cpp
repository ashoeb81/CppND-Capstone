#include <thread>
#include <future>
#include <chrono>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/assert.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "fifo_work_queue.h"
#include "video_reader.h"

using namespace std;
using namespace boost::program_options;
using namespace cv;
using namespace dnn;

float confThreshold = 0.5;
float nmsThreshold = 0.4;

// Remove the bounding boxes with low confidence using non-maxima suppression
void postprocess(Mat& frame, const vector<Mat>& out);

// Draw the predicted bounding box
void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);

int main(int argc, char* argv[]) {
  // User input catpure.
  options_description desc("Allowed options");
    desc.add_options()
      ("file_name", value<string>(), "pathname for output")
      ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);
    std::cout << "File Name: " << vm["file_name"].as<std::string>() << std::endl;
  
    // Create shared Queue
    FIFOWorkQueue<Mat> mqueue;
    VideoReader vreader(vm["file_name"].as<std::string>());
    std::promise<int> prmsTotalFrames;
    std::future<int> ftrTotalFrames = prmsTotalFrames.get_future();

    auto producer_future = std::async([&vreader, &mqueue, &prmsTotalFrames](const string& file_name){
      
      //VideoCapture cap(file_name);
      int frames_produced = 0;

      // if (!cap.isOpened()) {
      //   std::cout << "File not found!" << std::endl;
      //   return -1;
      // }
      
      Mat frame;
      while(1) {

        //cap >> frame;
        vreader.grabNextFrame(frame);
        
        if (frame.empty()) {
          break;
        }
        
        mqueue.push(std::move(frame));
        frames_produced++;
      }

      prmsTotalFrames.set_value(frames_produced);
      std::cout << "Producer | Total Frames " << frames_produced << std::endl;
      //cap.release();
      return frames_produced;
    
    }, vm["file_name"].as<std::string>());


    // pass to the consumer a vector that will be populated with structs of detections.
    // the struct should keep the name, frame, vector of detections.
    auto consumer_future = std::async([&mqueue, &ftrTotalFrames](){
      int frames_consumed = 0;
      int total_frames = -1;
      while (true) {
        
        // Wait for producer to signal total number of frames.
        if (total_frames == -1) {
          if (ftrTotalFrames.wait_for(std::chrono::milliseconds(50)) == std::future_status::timeout) {
            total_frames = ftrTotalFrames.get();
          }
        }

        // If the consumer has pulled all the frames, then we are done.
        if (frames_consumed == total_frames) {
          break;
        } else {
          auto frame = mqueue.pop();
          frames_consumed++;
        }
      }
      std::cout << "Consumer | Total Frames: " << frames_consumed << std::endl;
      return frames_consumed;
    });     
    
    // Give the configuration and weight files for the model
    // String modelConfiguration = "../model/yolov3.cfg";
    // String modelWeights = "../model/yolov3.weights";

    // Load the network
    // Net net = readNetFromDarknet(modelConfiguration, modelWeights);
    // net.setPreferableBackend(DNN_BACKEND_OPENCV);
    // net.setPreferableTarget(DNN_TARGET_CPU);

    // for (auto& name : net.getUnconnectedOutLayersNames()) {
    //   std::cout << "Layer: " << name << std::endl;
    // }
    
//     namedWindow("Frame");
//     Mat frame, blob;
//     while(1) {
//       cap >> frame;

//       if (frame.empty()) {
//         break;
//       }

//       blobFromImage(frame, blob, 1/255.0, Size(416, 416), Scalar(0,0,0), true, false);
//       net.setInput(blob);
//       vector<Mat> outs;
//       net.forward(outs, net.getUnconnectedOutLayersNames());
//       postprocess(frame, outs);

//       imshow("Frame", frame);
      
//       waitKey(25);

//     }
  
    producer_future.wait();
    consumer_future.wait();
    BOOST_ASSERT(producer_future.get() == consumer_future.get());

    // At this point generate video and write out detection in a CSV.

    std::cout << "Pushed all frames" << std::endl;
    // cap.release();
    // destroyAllWindows();  

    return 0;
}

// Remove the bounding boxes with low confidence using non-maxima suppression
void postprocess(Mat& frame, const vector<Mat>& outs)
{
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    
    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                
                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(Rect(left, top, width, height));
            }
        }
    }
    
    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    vector<int> indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        drawPred(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, frame);
    }
}

// Draw the predicted bounding box
void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame)
{
    //Draw a rectangle displaying the bounding box
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(255, 178, 50), 3);
    
    //Get the label for the class name and its confidence
    // string label = format("%.2f", conf);
    // if (!classes.empty())
    // {
    //     CV_Assert(classId < (int)classes.size());
    //     label = classes[classId] + ":" + label;
    // }
    
    // //Display the label at the top of the bounding box
    // int baseLine;
    // Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    // top = max(top, labelSize.height);
    // rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
    // putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,0),1);
}