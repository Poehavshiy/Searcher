#include "Video_controller.h"
#include "queue"
#include "Shape_Finder.h"
#include "thread"
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

double distance_humoments(vector<double>& first,vector<double>& second ){
    if(first.size() != second.size()) throw 228;
    double sum = 0;
    for(int i = 0; i < first.size(); ++i){
        sum += (first[i] - second[i])*(first[i] - second[i]);
    }
    return sqrt(sum);

}

int define_type(vector<Shape>& shapes, vector<double>& mask ){
    double min = distance_humoments(shapes[0].hu_moments, mask);
    int res = 0;
    for(int i = 0; i < shapes.size(); ++i){
        double cur_min = distance_humoments(shapes[i].hu_moments, mask);
        if(cur_min < min){
            res = i;
        }
    }
    return res;
}

void define_types(vector<Shape>& shapes , vector<vector<double>>& masks){
    for(int i = 0; i < MASKS; ++i){
        int defined = define_type(shapes,masks[i] );
//        shapes[defined].type = i;
    }
    std::sort(shapes.begin(), shapes.end(), [](Shape &first, Shape &second) -> bool {
  //      return first.type > second.type;
    });

}

void range_and_smooth(Mat& target, int bot_v = 228, int blur = 25){
    Scalar bot(bot_v, 0 , 0);
    Scalar top(255, 0 , 0);
    inRange(target, bot, top, target);
    GaussianBlur(target, target, Size(blur, blur), 0, 0);
}

//vector<double> get_hu_moments(Moments& moments){
//    vector<double> hu_moments;
//    HuMoments(moments, hu_moments);
//    return hu_moments;
//};
//
//vector<pair<string, double>> get_all_moments(Moments& moments){
//    vector<pair<string, double>> result;
//    result.push_back(pair<string, double>("m10", moments.m10));
//    result.push_back(pair<string, double>("m03", moments.m03));
//    result.push_back(pair<string, double>("m02", moments.m02));
//    result.push_back(pair<string, double>("m00", moments.m00));
//    result.push_back(pair<string, double>("m01", moments.m01));
//    result.push_back(pair<string, double>("m11", moments.m11));
//    result.push_back(pair<string, double>("m12", moments.m12));
//    result.push_back(pair<string, double>("m20", moments.m20));
//    result.push_back(pair<string, double>("m21", moments.m21));
//    result.push_back(pair<string, double>("mu30", moments.mu30));
//    result.push_back(pair<string, double>("mu02", moments.mu02));
//    result.push_back(pair<string, double>("mu03", moments.mu03));
//    result.push_back(pair<string, double>("mu11", moments.mu11));
//    result.push_back(pair<string, double>("mu12", moments.mu12));
//    result.push_back(pair<string, double>("mu20", moments.mu20));
//    result.push_back(pair<string, double>("mu21", moments.mu21));
//    result.push_back(pair<string, double>("nu02", moments.nu02));
//    result.push_back(pair<string, double>("nu03", moments.nu03));
//    result.push_back(pair<string, double>("nu11", moments.nu11));
//    result.push_back(pair<string, double>("nu12", moments.nu12));
//    result.push_back(pair<string, double>("nu20", moments.nu20));
//    result.push_back(pair<string, double>("nu21", moments.nu21));
//    result.push_back(pair<string, double>("nu30", moments.nu30));
//    //
//    double mu_mul = 1;
//    double nu_mul = 1;
//    double help = 0.0000001;
//    for(auto it = result.begin(); it != result.end(); ++it){
//        if( it -> first[0] =='m' && it -> first[1] =='u') {
//            mu_mul *= it->second * help;
//        }
//        if( it -> first[0] =='n'){
//            nu_mul*= it->second * help;
//        }
//    }
//    result.push_back(pair<string, double>("MU_multip", mu_mul));
//    result.push_back(pair<string, double>("NU_multip", nu_mul));
//    result.push_back(pair<string, double>("MU*NU", mu_mul*nu_mul));
//    return result;
//}
//
//vector<double> moments_information(Mat& target){
//    vector<vector<Point>> contours;
//    vector<vector<Point>> real_contours;
//    vector<Vec4i> hierarchy;
//    Moments mu;
//    double div = 0;
//    vector<pair<Point2f, Moments>> centers_moments;
//    findContours(target, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
//    for (int i = 0; i < contours.size(); i++) {
//        mu = moments(contours[i], false);
//        Point2f current(mu.m10 / mu.m00, mu.m01 / mu.m00);
//        if(!centers_moments.empty()) {
//            div = hypot(current.x - centers_moments.back().first.x, current.y - centers_moments.back().first.y);
//            if (div > 5) {
//                centers_moments.push_back(pair<Point2f, Moments>(current,mu));
//                real_contours.push_back(contours[i]);
//            }
//        }
//        else
//            centers_moments.push_back(pair<Point2f, Moments>(current,mu));
//    }
//    //
//    if(centers_moments.size() != 1){
//        throw 228;
//    }
//    return get_hu_moments(centers_moments.begin()->second);
//
//}
//
//vector<vector<Point>> contors(vector<Shape>& shapes, Mat &target_pic) {
//    vector<vector<Point>> contours;
//    vector<vector<Point>> real_contours;
//    vector<Vec4i> hierarchy;
//    Moments mu;
//    findContours(target_pic, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
//    double div = 0;
//
//    for (int i = 0; i < contours.size(); i++) {
//        mu = moments(contours[i], false);
//        Shape shape;
//        HuMoments(mu, shape.hu_moments);
//        shape.center = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
//        if (!shapes.empty()) {
//            div = hypot(shape.center.x - shapes.back().center.x,
//                        shape.center.y - shapes.back().center.y);
//            if (div > 2) {
//                shapes.push_back(shape);
//                real_contours.push_back(contours[i]);
//            }
//        } else
//            shapes.push_back(shape);
//    }
//    vector<vector<Point>> convex_hulls(contours.size());
////    for (int i = 0; i < contours.size(); i++) {
////        convexHull(Mat(contours[i]), convex_hulls[i], false);
////    }
//    return convex_hulls;
//
//}

int main(int argc, char **argv) {
    vector<vector<double>> masks(MASKS);
    cv::Mat circle= imread("/home/nikita/ClionProjects/Searcher/circle.png", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat rectangle = imread("/home/nikita/ClionProjects/Searcher/rectangle.png", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat cross = imread("/home/nikita/ClionProjects/Searcher/cross.jpg" , CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat triang = imread("/home/nikita/ClionProjects/moments_test/triang.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    range_and_smooth(circle);
    range_and_smooth(rectangle);
    range_and_smooth(cross);
    range_and_smooth(triang);
    //
//    masks[0] = moments_information(circle);
//    masks[1] = moments_information(triang);
//    masks[2] = moments_information(cross);

    //00009.MTS kek.avi
    Video_controller video_controller("/home/nikita/ClionProjects/STREAM/00023.MTS");
    vector<Shape> shapes;
    Mat frame;
    int counter = 0;
    Mat map_inv = mat_from_file("/home/nikita/colomna/calibration/text/calibration_mat.txt");
    Mat map = map_inv.inv();
    vector<vector<pair<Point2f, int>>> maped_pints_types;
    vector<double> moments;
    vector<int> frames;
    double time = std::clock();
    //Mat check_image(1080, 1920, CV_8UC3);
    Mat check_image = imread("/home/nikita/ClionProjects/Searcher/kek.png");



    Shape_Finder shape_finder(circle, triang, cross);

    for (;;) {

        double start = std::clock();
        video_controller.start(false);
        frame = video_controller.get_sum();
        imshow("edges", frame);

        vector<vector<Point>> convex_hulls;
        Mat mapped_image(frame.rows, frame.cols, CV_LOAD_IMAGE_GRAYSCALE);
        warpPerspective(frame, mapped_image, map, mapped_image.size());
        Mat circles = mapped_image.clone();
        if (true/*video_controller.CATCH*/) {

            imshow("mapped_image", mapped_image);
            //contors(shapes, mapped_image);
            //define_types(shapes, masks);
            std::map<SHAPE_TYPE, Shape> shapes = shape_finder.find_primitives(mapped_image)
        }

        if( shapes.size() == MASKS) {
            if(DRAW) {
                for (auto it = shapes.begin(); it != shapes.end(); ++it) {
                    for (auto jt = it->hu_moments.begin(); jt != it->hu_moments.end(); ++jt) {
                        cout << *jt << ' ';
                    }
                    cout << endl;
                }
                cout << endl << endl;

                cvtColor(circles, circles, CV_GRAY2BGR);
//                for (int i = 0; i < convex_hulls.size(); i++) {
//                    drawContours(circles, convex_hulls, i, Scalar(255, 0, 0), 1, 8);
//                }

                for (int i = 0; i != shapes.size(); ++i) {
                    cv::circle(circles, shapes[i].center, 30, Scalar(0, 0, 255), 0, 40);
                    putText(circles, to_string(i), shapes[i].center, 1, 5, (0, 0, 255), 2, 4);
                }
            }
            if(true/*video_controller.MAP*/){
                vector<pair<Point2f,int>> mapped_points;
                for (auto it = shapes.begin(); it != shapes.end(); ++it){
                    Point2f maped_center = map_point(&map, it->center);
//                    mapped_points.push_back(pair<Point2f,int>(maped_center, it->type));
                }
                maped_pints_types.push_back(mapped_points);
                moments.push_back(time);
                frames.push_back(counter);

//                for (auto it = mapped_points.begin(); it != mapped_points.end(); ++it) {
//                    if(it->second == 0)
//                        cv::circle(check_image, it->first, 5, Scalar(0, 0, 255), -1, 8);
//                    else if(it->second == 1)
//                        cv::circle(check_image, it->first, 5, Scalar(0, 255, 0), -1, 8);
//                    else if(it->second == 2)
//                        cv::circle(check_image, it->first, 5, Scalar(255, 0, 0), -1, 8);
//
//                }
            }
//            for (auto it = shapes.begin(); it != shapes.end(); ++it) {
//                if(it->type == 0)
//                    cv::circle(check_image, it->center, 5, Scalar(0, 0, 255), -1, 8);
//                else if(it->type == 1)
//                    cv::circle(check_image, it->center, 5, Scalar(0, 255, 0), -1, 8);
//                else if(it->type == 2)
//                    cv::circle(check_image, it->center, 5, Scalar(255, 0, 0), -1, 8);
//
//            }



        }
        shapes.clear();
        double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
        cout << duration << endl;
        imshow("circles", circles);
        counter++;
        cout << counter << endl;

        if (waitKey(1) == 27) break;
    }
    ofstream myfile;
    myfile.open ("example.txt");
    for(int i = 0; i != maped_pints_types.size(); ++i){
        myfile << frames[i] <<  ' ' << moments[i] << endl;
        for(int j = 0 ; j < maped_pints_types[i].size(); ++j){
            myfile << maped_pints_types[i][j].first << maped_pints_types[i][j].second<<endl;
        }
        myfile<<endl;
    }

    imwrite("/home/nikita/ClionProjects/Searcher/check_image.png", check_image);
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;

}