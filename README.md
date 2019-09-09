# CPPND Capstone: Multithreaded Object Detection in Videos.

This repository contains an implementation of a multithreaded application for detecting objects in a user-specified video.  Example output of running the application on the input video (`videos/project_video.mp4`) can is the resulting video (`videos/final_project_video_out.avi`).

The application uses two asynchronous tasks launched from `main.cpp`.  The first asynchronous task (producer) reads frames from a video file using the `VideoReader` class (`src/video_reader.*`) and pushes those frames into a thread-safe queue implemented by the `FIFOWorkQueue` class (`src/fifo_work_queue.h`).

The second asynchronous task (consumer) simultaneously pulls frames from the thread-safe `FIFOWorkQueue` as they become available.  The second task then applies a machine-learned object detecter managed by the `ObjectDetector` class (`src/object_detector.*`).  The processed frames are then written out to user-specified output video file.

A promise/future pattern is used so that the producer task can communicate to the consumer task the total number of frames that need to be processed.

## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
* OpenCV >= 1.4
  * Linux: [click here for installation instructions](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html)
* Boost >= 1.58
  * Linux: [click here for installation instructions](https://www.boost.org/doc/libs/1_71_0/more/getting_started/unix-variants.html)

## Build and Run Instructions

1. Clone this repo.

2. Run the following commands to download object detection models from [Darknet](https://pjreddie.com/darknet/)
```
cd CppND-Capstone
mkdir model && cd model
wget https://pjreddie.com/media/files/yolov3.weights
wget https://github.com/pjreddie/darknet/blob/master/cfg/yolov3.cfg?raw=true -O ./yolov3.cfg
```

2. Make a build directory in the top level directory: `mkdir build && cd build`

3. Compile: `cmake .. && make`

4. Run it on first 300 frames of video (**4 minutes runtime**)
```
./ObjecteDetector --input_fname=../videos/project_video.mp4 
   --output_fname=../videos/project_video_out.avi 
   --model_config=../model/yolov3.cfg 
   --model_weights=../model/yolov3.weights
   --max_num_frames=300
```

5. Run on all frames (**16 minutes runtime**)
```
./ObjecteDetector --input_fname=../videos/project_video.mp4 
   --output_fname=../videos/project_video_out.avi 
   --model_config=../model/yolov3.cfg 
   --model_weights=../model/yolov3.weights
```

5. While running the application will print to the terminal its current progress
```
root@f03d348393db:/home/workspace/CppND-Capstone/build# ./ObjecteDetector --input_fname=../videos/project_video.mp4 --output_fname=../videos/project_video_out.avi --model_config=../model/yolov3.cfg --model_weights=../model/yolov3.weights 
Consumed Frames: 10 of 1257
Consumed Frames: 20 of 1257
Consumed Frames: 30 of 1257
Consumed Frames: 40 of 1257
Consumed Frames: 50 of 1257
Consumed Frames: 60 of 1257
```
6. Once the application terminate the output video can be played.

## Satisfied Rubric Points

### The project reads data from a file and process the data, or the program writes data to a file.

* The project reads frames from a video file using the `VideoReader` class (`line 22 src/video_reader.cpp`).

* The project process the read frames using the `ObjectDetector` class (`line 118 src/main.cpp`).

* The project writes the processed frames to a new output video file (`line 120 src/main.cpp`).

### The project accepts user input and processes the input.

* The project accepts command-line arguments (`line 24 src/main.cpp`) and uses those arguments to determine application behavior.

### The project uses Object Oriented Programming techniques.

* The project uses the classes `VideoReader` (`src/video_reader.*`), `ObjectDetector` (`src/object_detector.*`) and `FIFOWorkQueue` (`src/fifo_work_queue.h`)

### Classes use appropriate access specifiers for class members.

* Example of class-specific access specifiers in `VideoReader` class definition (`line 32 src/vidoe_reader.h`)

### Class constructors utilize member initialization lists.

* Use of initializer list can be observed in the implementation of the `ObjectDetector` constructor (`line 4 src/object_detector.cpp`)

### Templates generalize functions in the project.

* The `FIFOWorkQueue` is a templated class (`src/fifo_work_queue.h`)

### The project makes use of references in function declarations.

* Example of method that uses reference in function declaration is the `GrabNextFrame` method of the `VideoReader` class (`line 22 src/video_reader.cpp`)

### The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate.

* Example use of RAII can be seen acquisition and release of locks using std::lock_guard in `FIFOWorkQueue` (`line 32 src/fifo_work_queue.h`)

### The project uses move semantics to move data, instead of copying it, where possible.

* Example use of move semantics is the pushing and removing of items in `FIFOWorkQueue` (`line 55 src/fifo_work_queue.h`)

### The project uses smart pointers instead of raw pointers.

* Example use of the smart pointers (std::unique_ptr) is the machine-learned object detector private member variable in the `ObjectDetector` class (`line 40 src/object_detector.h`)

### The project uses multithreading.

* The project uses two asynchronous tasks (std::async).  One to read video frame (`line 47 src/main.cpp`) and another to process those frames (`line 82 src/main.cpp`).

### A promise and future is used in the project.

* A promise/future pattern is used so that the producer task can communicate to the consumer task the total number of frames that need to be processed (`line 71 and 101 src/main.cpp`)

### A mutex or lock is used in the project.

* The project uses a mutex lock to implement a thread-safe work queue (`line 32 and 50 src/fifo_work_queue.h`)

## Attributions

* This projet's methods related to handling OpenCV DNN model inputs and outputs were inspired by the following the repository [learnopencv/ObjectDetection-YOLO](https://github.com/spmallick/learnopencv/tree/master/ObjectDetection-YOLO).

* The video used in this repository was taken from the repository [udacity/CarND-Vehicle-Detection](udacity/CarND-Vehicle-Detection).