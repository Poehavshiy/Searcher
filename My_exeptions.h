//
// Created by nikita on 07.10.16.
//

#ifndef CALIBRATION_MY_EXEPTIONS_H
#define CALIBRATION_MY_EXEPTIONS_H

//opencv
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

//standart
#include <vector>
#include <iterator>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;


class My_exeption : public exception {
    string path_to_log;

public:
    My_exeption() {
        path_to_log = "";
    }

    My_exeption(string path_tp_log) {
        this->path_to_log = path_tp_log;
    }

    virtual const char *what() const throw() = 0;

    virtual void write_to_log(ofstream &myfile) = 0;

    virtual void write_to_log_additional(string additional, ofstream &myfile) {
        myfile << additional << endl;
    }
};

class No_countors : public My_exeption {

public:
    No_countors(string path_to_log) : My_exeption(path_to_log) {

    }

    const char *what() const throw() {
        return "There no contours have been found";
    }

    //
    void write_to_log(ofstream &myfile) {
        myfile << "There no contours have been found.\n";
    }

};

//
class Few_countors : public My_exeption {
public:
    Few_countors(string path_to_log) : My_exeption(path_to_log) {

    }

    const char *what() const throw() {
        return "There too few contours have been found";
    }

    //
    void write_to_log(ofstream &myfile) {
        myfile << "There too few contours have been found.\n";
    }
};

//
class Many_countors : public My_exeption {
public:
    Many_countors(string path_to_log) : My_exeption(path_to_log) {

    }

    const char *what() const throw() {
        return "There too many have been found";
    }

    //
    void write_to_log(ofstream &myfile) {
        myfile << "There too many  contours have been found.\n";
    }
};

//
class Not_enough_data : public My_exeption {
public:
    Not_enough_data(string path_to_log) : My_exeption(path_to_log) {

    }

    const char *what() const throw() {
        return "Insufficient data for calibration";
    }

    //
    void write_to_log(ofstream &myfile) {
        myfile << "Insufficient data for calibration. \n";
    }
};
//
class Empty_result : public My_exeption {
public:
    Empty_result(string path_to_log) : My_exeption(path_to_log) {

    }

    const char *what() const throw() {
        return "Result mat is empty";
    }

    //
    void write_to_log(ofstream &myfile) {
        myfile << "Result mat is empty. \n";
    }
};

#endif //CALIBRATION_MY_EXEPTIONS_H
