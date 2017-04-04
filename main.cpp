#include "Video_controller.h"
#include "queue"
#include "Shape_Finder.h"
#include "thread"
#include <fstream>
#include "algorithm"

using namespace cv;
using namespace std;

const int MASKS = 3;

const bool DRAW = true;
bool MAP = false;


Mat mat_from_file(const char *adress) {
    ifstream is(adress);
    vector<double> a((istream_iterator<double>(is)), istream_iterator<double>());
    cout << endl;
    int k = 0, j = 0;
    Mat M(3, 3, CV_64F);
    for (int i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            M.at<double>(i, j) = a.at(k + j);
        }
        k += j;
    }
    is.close();
    return M;
}

Point2f map_point(const Mat *oper, Point2f P) {
    Point2f res;
    res.x = (oper->at<double>(0, 0) * P.x + oper->at<double>(0, 1) * P.y + oper->at<double>(0, 2)) /
            (oper->at<double>(2, 0) * P.x + oper->at<double>(2, 1) * P.y + oper->at<double>(2, 2));

    res.y = (oper->at<double>(1, 0) * P.x + oper->at<double>(1, 1) * P.y + oper->at<double>(1, 2)) /
            (oper->at<double>(2, 0) * P.x + oper->at<double>(2, 1) * P.y + oper->at<double>(2, 2));
    return res;
}

void range_and_smooth(Mat &target, int bot_v = 228, int blur = 25) {
    Scalar bot(bot_v, 0, 0);
    Scalar top(255, 0, 0);
    inRange(target, bot, top, target);
    GaussianBlur(target, target, Size(blur, blur), 0, 0);
}



int main(int argc, char **argv) {
    vector<vector<double>> masks(MASKS);
    cv::Mat circle = imread("/home/nikita/ClionProjects/Searcher/circle.png", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat rectangle = imread("/home/nikita/ClionProjects/Searcher/rectangle.png", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat cross = imread("/home/nikita/ClionProjects/Searcher/cross.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat triang = imread("/home/nikita/ClionProjects/moments_test/triang.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat line = imread("/home/nikita/ClionProjects/Searcher/line.png", CV_LOAD_IMAGE_GRAYSCALE);
    range_and_smooth(circle);
    range_and_smooth(rectangle);
    range_and_smooth(cross);
    range_and_smooth(triang);
    range_and_smooth(line);

    string video_path = "/home/nikita/ClionProjects/STREAM/true_videos/00044.MTS";
    Video_controller video_controller(video_path);
    Mat frame;
    int counter = 0;
    Mat map_inv = mat_from_file("/home/nikita/colomna/calibration/text/calibration_mat.txt");
    Mat map = map_inv.inv();
    vector<double> moments;
    vector<int> frames;
    double time = std::clock();
    //Mat check_image(1080, 1920, CV_8UC3);
    Mat check_image = imread("/home/nikita/ClionProjects/check_triectory/kek.png");
    Shape_Finder shape_finder(circle, line, cross);
    std::map<SHAPE_TYPE, Shape> shapes;
    int case_ = 14;
    ofstream cross_check_file("/home/nikita/ClionProjects/Searcher/test/cross_real" +to_string(case_) + ".txt");
    ofstream triangle_check_file("/home/nikita/ClionProjects/Searcher/test/tiangle_real"+ to_string(case_) + ".txt");
    ofstream circle_check_file("/home/nikita/ClionProjects/Searcher/test/dot_real" + to_string(case_) + ".txt");
    for (;;) {

        double start = std::clock();
        video_controller.start(false);
        frame = video_controller.get_sum();
       // imshow("edges", frame);

        Mat circles = frame.clone();
        if (true/*video_controller.CATCH*/) {
        //    imshow("mapped_image", frame);
            shapes = shape_finder.find_primitives(frame);
        }

        if (true) {
            if (video_controller.CATCH) {

                cvtColor(circles, circles, CV_GRAY2BGR);
                for (auto it = shapes.begin(); it != shapes.end(); ++it) {
                    cv::circle(circles, it->second.center, 30, Scalar(0, 0, 255), 0, 40);
                    string type;
                    Scalar color;
                    Point2f maped_center = map_point(&map, it->second.center);
                    if (it->first == SHAPE_TYPE::CIRCLE) {
                        color = Scalar(255, 0, 0);
                        type = "CIRCLE";
                        circle_check_file<<maped_center.x<<" ";
                        circle_check_file<<maped_center.y<<endl;
                    } else if (it->first == SHAPE_TYPE::TRIANGLE) {
                        color = Scalar(0, 255, 0);
                        type = "TRIANGLE";
                        triangle_check_file<<maped_center.x<<" ";
                        triangle_check_file<<maped_center.y<<endl;
                    } else if (it->first == SHAPE_TYPE::CROSS) {
                        color = Scalar(0, 0, 255);
                        type = "CROSS";
                        cross_check_file<<maped_center.x<<" ";
                        cross_check_file<<maped_center.y<<endl;
                    } else if (it->first == SHAPE_TYPE::INTERSECTION) {
                        color = Scalar(0, 0, 0);
                        type = "INTERSECTION";
                    }
                    putText(circles, type, it->second.center, 1, 5, (0, 0, 255), 2, 4);
                    cv::circle(check_image, maped_center, 1, color, -1, 8);

                }


            }
            if (video_controller.MAP) {
                vector<pair<Point2f, SHAPE_TYPE>> mapped_points;
                for (auto it = shapes.begin(); it != shapes.end(); ++it) {
                    Point2f maped_center = map_point(&map_inv, it->second.center);
                    mapped_points.push_back(pair<Point2f, SHAPE_TYPE>(maped_center, it->first));
                }
                moments.push_back(time);
                frames.push_back(counter);

            }

        }
        double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
        imshow("circles", circles);
//        cout << duration << endl;
        counter++;
        cout << counter << endl;
        if (waitKey(1) == 27) break;
    }
    imwrite("/home/nikita/ClionProjects/Searcher/check_image.png", check_image);
    cross_check_file.close();
    triangle_check_file.close();
    circle_check_file.close();
    return 0;

}