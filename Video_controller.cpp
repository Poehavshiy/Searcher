//
// Created by nikita on 08.10.16.
//

#include "Video_controller.h"

void Video_controller::create_windows(){
    namedWindow("Control", CV_WINDOW_AUTOSIZE);
    moveWindow("Control", 350, 350);
}

void Video_controller::create_track_bar(){
    //Create trackbars in "Control" window
    cvCreateTrackbar("LowH", "Control", &iLowH, 255); //Hue (0 - 179)
    cvCreateTrackbar("HighH", "Control", &iHighH, 255);

    cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
    cvCreateTrackbar("HighS", "Control", &iHighS, 255);

    cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
    cvCreateTrackbar("HighV", "Control", &iHighV, 255);
}

void Video_controller::create_track_bar_bool(){
    namedWindow("CATCH@MAP", CV_WINDOW_AUTOSIZE);
    moveWindow("Control", 500, 350);
    cvCreateTrackbar("CATCH", "CATCH@MAP", &CATCH, 1); //Hue (0 - 179)
    cvCreateTrackbar("MAP", "CATCH@MAP", &MAP, 1);
}

Video_controller::Video_controller(int camera_id):cap(camera_id){
    create_windows();
    create_track_bar();
    create_track_bar_bool();
    this->camera_id = camera_id;
    cap.set(CV_CAP_PROP_BUFFERSIZE, 10000);
}

Video_controller::Video_controller(const string& path_to_stream):cap(path_to_stream){
    create_windows();
    create_track_bar();
    create_track_bar_bool();
    cap.set(CV_CAP_PROP_BUFFERSIZE, 10000);
}

void Video_controller::start(bool show){
    bool bSuccess = cap.read(img_original);
    //
    cvtColor(img_original, img_original, COLOR_BGR2GRAY);
    inRange(img_original, Scalar(228, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), img_original);
    GaussianBlur(img_original, img_original, Size(5, 5), 0, 0);
}

Mat& Video_controller::get_sum(){
    return img_original;
}

bool Video_controller::is_opened(){
    return cap.isOpened();
}
