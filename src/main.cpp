#include <thread>
#include <future>
#include <chrono>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/assert.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "fifo_work_queue.h"
#include "video_reader.h"
#include "object_detector.h"

using namespace std;
using namespace boost::program_options;
using namespace cv;

int main(int argc, char *argv[])
{
  // Capture and parse command line arguments using boost::program_options.
  options_description desc("Allowed options");
  desc.add_options()
    ("input_fname", value<string>(), "Path to input video file to be processed.")
    ("output_fname", value<string>(), "Path to save output video file.")
    ("model_config", value<string>(), "Path to object detection model configuration file.")
    ("model_weights", value<string>(), "Path to object detection model weights.")
    ("max_num_frames", value<int>()->default_value(-1), "Maximum number of frames to process.");
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  // Instantiate FIFOWorkQueue that will connect producer and consumer threads.
  FIFOWorkQueue<Mat> queue;

  // Instantiate VideoReader that will read video file and push frames to the FIFOWorkQueue.
  VideoReader vreader(vm["input_fname"].as<std::string>());

  // Create promise/future pair that will be used by the producer to signal to the consumer
  // the total number of frames that have been pushed to the work FIFOWorkQueue.
  std::promise<int> prms;
  std::future<int> ftr = prms.get_future();

  // Producer asynchronous task uses VideoReader (vreader) to read video frames from a video
  // file and then pushes those frames to the FIFOWorkQueue (queue).
  auto producer_future = std::async([&vreader, &queue, &prms](int max_num_frames) {
    // Counter keeps track of total frames read.
    int frames_produced = 0;

    // Holds frames read from video file.
    Mat frame;

    while (true)
    {
      // Read frame.
      bool success = vreader.GrabNextFrame(frame);

      // If no frame returned, then exit task.
      // Otherwise push frame onto FIFOWorkQueue and increment frame count.
      if (!success || (max_num_frames != -1 && frames_produced == max_num_frames))
      {
        break;
      } else {
        queue.Push(std::move(frame));
        frames_produced++;
      }
    }

    // Satisfy promise with total number of frames produced.
    prms.set_value(frames_produced);

    // return total number of frames produced.
    return frames_produced;
  }, vm["max_num_frames"].as<int>());

  // Instantiate ObjectDetector that will be used to detect objects in video frames.
  ObjectDetector detector(vm["model_config"].as<std::string>(), vm["model_weights"].as<std::string>());

  // Consumer asynchronous task uses ObjectDetector (detector) to draw bounding boxes around detected objects
  // in each video frame.  The task also writes the processed output files to a new output video file.
  auto consumer_future = std::async([&detector, &queue, &ftr](std::string output_path, double fps, int four_cc, cv::Size frame_size) {
    // Counter keeps track of frames processed.
    int frames_consumed = 0;

    // Total number of frames to be processed will be set when promise is fullfilled by producer task.
    int total_frames = -1;

    // Object managing writing frames to output video file.
    VideoWriter out_video(output_path, four_cc, fps, frame_size);

    while (true)
    {

      if (total_frames == -1)
      {
        // Wait for 50ms to see if future is ready and use value to determine total number of frames that need to be processed.
        // If future is not ready, continue processing frames.
        if (ftr.wait_for(std::chrono::milliseconds(50)) == std::future_status::ready)
        {
          total_frames = ftr.get();
        }
      }

      // If the consumer has pulled all the frames, then we are done.
      // Otherwise process the frame and add to the output video file.
      if (frames_consumed == total_frames)
      {
        break;
      } else {
        // Pull frame from the queue.
        cv::Mat frame = queue.Pop();

        // If frame is not empty then process it.
        if (!frame.empty())
        {
          // Apply detector to frame.
          detector.Detect(frame);
          // Write frame to output video file.
          out_video.write(frame);
          // Increment processed frame count.
          frames_consumed++;
          // Periodic logging.
          if (frames_consumed % 10 == 0 && total_frames != -1) {
            std::cout << "Consumed Frames: " << frames_consumed << " of " << total_frames << std::endl;
          }
        }
      }
    }

    // Free resources associated with VideoWriter and close output video file.
    out_video.release();

    // Return total number of frames consumed.
    return frames_consumed;
  }, vm["output_fname"].as<std::string>(), vreader.GetFrameRate(), vreader.GetFourCC(), vreader.GetFrameSize());

  producer_future.wait();
  consumer_future.wait();
  
  // Assert that cosumer and procuder have processed an equal number of frames.
  BOOST_ASSERT(producer_future.get() == consumer_future.get());

  return 0;
}
