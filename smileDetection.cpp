/*
 * File:      smileDetection.cpp
 * Author:    Richard Purcell
 * Date:      2020-18-12
 * Version:   1.0
 * Purpose:   Given an image detect whether subject is smiling and return boolean value.
 * Usage:     $ ./smileDetection
 * Notes:     Created for OpenCV's Computer Vision 2 Smile Detection assignment
 *            Provided filename is hard coded for this project.
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "./dlib/dlib/opencv.h"
#include "./dlib/dlib/image_processing/frontal_face_detector.h"
#include "./dlib/dlib/image_processing.h"
#include <iostream>

#define RESIZE_HEIGHT 240
#define FACE_DOWNSAMPLE_RATIO_DLIB 1

using namespace std;
using namespace cv;
using namespace dlib;

bool smile_detector(const dlib::cv_image<dlib::bgr_pixel> &cimg, \
                    const dlib::rectangle &face, \
                    const dlib::full_object_detection &landmarks)
{
    bool isSmiling = true;

    return isSmiling;
}

int
main()
{
    cout << "[INFO] loading facial landmark predictor..." << endl;
    frontal_face_detector detector = get_frontal_face_detector();
    shape_predictor shape_predictor;
    deserialize("./models/shape_predictor_68_face_landmarks.dat") >> shape_predictor;

    //initialize video capture
    VideoCapture capture("./video/smile.mp4");

    if(!capture.isOpened()) {
        cerr << "[ERROR] Unable to capture frame" << endl;
        EXIT_FAILURE;
    }

    //create video writer
    VideoWriter smileDetectionOut("smileDetectionOutput.avi",
                                 VideoWriter::fourcc('M','J','P','G'),
                                 15,
                                 Size((int)capture.get(CAP_PROP_FRAME_WIDTH),
                                 (int) capture.get(CAP_PROP_FRAME_HEIGHT)));
    
    int frame_number = 0;
    std::vector<int> smile_frames;

    Mat frame, frame_small;
    float IMAGE_RESIZE;

    while(capture.read(frame)){
        if (frame.empty()) {
            cout << "[ERROR] Unable to capture frame" << endl;
            break;
        }
    

        IMAGE_RESIZE = (float)frame.rows/RESIZE_HEIGHT;
        resize(frame, frame, Size(), 1.0/IMAGE_RESIZE, 1.0/IMAGE_RESIZE);
        resize(frame, frame_small, Size(), 1.0/FACE_DOWNSAMPLE_RATIO_DLIB, 1.0/FACE_DOWNSAMPLE_RATIO_DLIB);

        cv_image<bgr_pixel> cimg(frame);
        cv_image<bgr_pixel> cimg_small(frame_small);

        //Detect faces
        std::vector<dlib::rectangle> faces = detector(cimg_small);

        //if number of faces detected is 0
        if(0 == faces.size()) {
            putText(frame, "Unable to detect face, Please check lighting", Point(10, 30), FONT_HERSHEY_COMPLEX, 0.3, Scalar(0,0,255), 1, LINE_AA);
            putText(frame, "Or Decrease FACE_DOWNSAMPLE_RATIO", Point(10,50), FONT_HERSHEY_COMPLEX, 0.3, Scalar(0,0,255), 1, LINE_AA);
        }
        else {
            dlib::rectangle face(
                (long)(faces[0].left() * FACE_DOWNSAMPLE_RATIO_DLIB),
                (long)(faces[0].top() * FACE_DOWNSAMPLE_RATIO_DLIB),
                (long)(faces[0].right() * FACE_DOWNSAMPLE_RATIO_DLIB),
                (long)(faces[0].bottom() * FACE_DOWNSAMPLE_RATIO_DLIB)
            );
            full_object_detection landmarks = shape_predictor(cimg, face);
            if(smile_detector(cimg, face, landmarks)) {
                putText(frame, format("Smiling :0"), Point(10, 30),
                FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,0,255), 2, LINE_AA);
                smile_frames.push_back(frame_number);
            }
        }

        if (frame_number%50 == 0) {
            cout << "\nProcessed " << frame_number << " frames" << endl;
            cout << "Smile detected in " << smile_frames.size() << " number of frames" << endl;
        }

        //write to video writer
        resize(frame, frame, Size(), IMAGE_RESIZE, IMAGE_RESIZE);
        smileDetectionOut.write(frame);
        frame_number++;

    }

    capture.release();
    smileDetectionOut.release();
 
    return 0;

}