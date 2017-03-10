//
// Created by nikita on 08.10.16.
//

#ifndef CALIBRATION_VEDEO_CONTROLLER_H
#define CALIBRATION_VEDEO_CONTROLLER_H
#include "My_exeptions.h"


class Video_controller {
private:
    int camera_id;
    string path_to_stream;
    VideoCapture cap;
    //
    int iLowH = 0;
    int iHighH = 255;
    int iLowS = 0;
    int iHighS = 255;
    int iLowV = 0;
    int iHighV = 255;

    Mat img_original;

    void create_windows();

    void create_track_bar();

    void create_track_bar_bool();
public:
    int CATCH = 0;
    int MAP = 0;

    Video_controller(int camera_id);

    Video_controller(const string& path_to_stream);

    void start(bool show);

    Mat& get_sum();

    bool is_opened();


};


#endif //CALIBRATION_VEDEO_CONTROLLER_H
