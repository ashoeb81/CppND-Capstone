# CPPND: Capstone Hello World Repo

This is a starter repo for the Capstone project in the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213).

The Capstone Project gives you a chance to integrate what you've learned throughout this program. This project will become an important part of your portfolio to share with current and future colleagues and employers.

In this project, you can build your own C++ application starting with this repo, following the principles you have learned throughout this Nanodegree Program. This project will demonstrate that you can independently create applications using a wide range of C++ features.

## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
* OpenCV
  * Linux: [click here for installation instructions](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html)
* Boost
  * Linux: [click here for installation instructions](https://www.boost.org/doc/libs/1_71_0/more/getting_started/unix-variants.html)

## Basic Build Instructions

1. Clone this repo.
2. Run the following commands to download object detection models
```
cd CppND-Capstone/model
wget https://pjreddie.com/media/files/yolov3.weights
wget https://github.com/pjreddie/darknet/blob/master/cfg/yolov3.cfg?raw=true -O ./yolov3.cfg
```
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: 
```
./HelloWorld --input_fname=../videos/project_video.mp4 \ 
             --output_fname=../videos/project_video_out.avi \
             --model_config=../model/yolov3.cfg \
             --model_weights=../model/yolov3.weights
```