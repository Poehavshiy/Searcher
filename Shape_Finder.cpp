//
// Created by nikita on 10.03.17.
//

#include "Shape_Finder.h"


Shape_Finder::Shape_Finder(Mat &circle_image, Mat &triangle_image, Mat &cross_image) {
    vector<vector<Point>> circle_contors;
    findContours(circle_image, circle_contors, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> triangle_contors;
    findContours(triangle_image, triangle_contors, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    if (circle_contors.size() != 1 || triangle_contors.size() != 1) {
        throw 228;
    }
    vector<vector<Point>> cross_contors;
    findContours(cross_image, cross_contors, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    if (cross_contors.size() != 1) {
        throw 228;
    }
    //
    Moments circle_moments = moments(circle_contors[0], false);
    Moments triangle_moments = moments(triangle_contors[0], false);
    Moments cross_moments = moments(cross_contors[0], false);
    //
    Shape circle(circle_moments);
    Shape triangle(triangle_moments);
    Shape cross(cross_moments);
    //
    sample_shapes.insert(pair<SHAPE_TYPE, Shape>(SHAPE_TYPE::CIRCLE, circle));
    sample_shapes.insert(pair<SHAPE_TYPE, Shape>(SHAPE_TYPE::TRIANGLE, triangle));
    sample_shapes.insert(pair<SHAPE_TYPE, Shape>(SHAPE_TYPE::CROSS, cross));

}


std::map<SHAPE_TYPE, Shape> Shape_Finder::find_primitives(Mat &target_image) {
    vector<vector<Point>> all_contors;
    vector<Shape> all_shapes;
    findContours(target_image, all_contors, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    Moments mu;
    for (auto it = all_contors.begin(); it != all_contors.end(); ++it) {
        mu = moments(*it, false);
        all_shapes.push_back(Shape(mu));
    }
    return define_shapes(all_shapes);
}

pair<SHAPE_TYPE, double> Shape_Finder::closest_shape(Shape &shape) {

}

std::map<SHAPE_TYPE, Shape> Shape_Finder::define_shapes(vector<Shape> &all_shapes) {
    std::map<SHAPE_TYPE, Shape> result;
    int size = all_shapes.size();
    switch (size) {
        case (0):
            break;
        case (1) : {
            //pair<SHAPE_TYPE, double> closest = closest_shape(all_shapes[0]);
            //sample_shapes[closest.first] = all_shapes[0];
            result.insert(std::pair<SHAPE_TYPE, Shape>(SHAPE_TYPE::INTERSECTION, all_shapes[0]));
            break;
        }
        case (2) : {
            //first closest mask
            pair<SHAPE_TYPE, double> true_shape = closest_shape(all_shapes[0]);
            pair<SHAPE_TYPE, double> intersection_shape = closest_shape(all_shapes[1]);
            int true_id = 0;
            int intersection_id = 1;
            //if this -> then second is properly found shape
            if (true_shape.second > intersection_shape.second) {
                swap(true_id, intersection_id);
                swap(true_shape, intersection_shape);
            }
            sample_shapes[true_shape.first] = all_shapes[true_id];
            //return properly founded
            result.insert(std::pair<SHAPE_TYPE, Shape>(true_shape.first, all_shapes[true_id]));
            //and INTERSECTION
            result.insert(std::pair<SHAPE_TYPE, Shape>(SHAPE_TYPE::INTERSECTION, all_shapes[intersection_id]));
            break;
        }
        case (3): {
            vector<vector<pair<bool, double>>> difs;
            fill_diffs(difs, all_shapes);
            pair<int, int> first_shape_id = min_diff(difs);
            delete_cross(difs, first_shape_id);
            pair<int, int> second_shape_id = min_diff(difs);
            delete_cross(difs, second_shape_id);
            pair<int, int> third_shape_id = min_diff(difs);
            //
            SHAPE_TYPE first_type = cast_shape_from_int(first_shape_id.first);
            SHAPE_TYPE second_type = cast_shape_from_int(second_shape_id.first);
            SHAPE_TYPE third_type = cast_shape_from_int(third_shape_id.first);
            //
            sample_shapes[first_type] = all_shapes[first_shape_id.second];
            sample_shapes[second_type] = all_shapes[second_shape_id.second];
            sample_shapes[third_type] = all_shapes[third_shape_id.second];
            result = sample_shapes;
            break;
        }
        default:
            break;
    }
    return result;
}


void Shape_Finder::fill_diffs(vector<vector<pair<bool, double>>>& diffs, vector<Shape> &all_shapes){
    for(int i = 0; i < 3; ++i){
        diffs[0].push_back(pair<bool, double>(true,
                                              shape_difference(sample_shapes[SHAPE_TYPE::CIRCLE], all_shapes[i])));
        diffs[1].push_back(pair<bool, double>(true,
                                              shape_difference(sample_shapes[SHAPE_TYPE::TRIANGLE], all_shapes[i])));
        diffs[2].push_back(pair<bool, double>(true,
                                              shape_difference(sample_shapes[SHAPE_TYPE::CROSS], all_shapes[i])));
    }
}

double Shape_Finder::shape_difference(Shape& first, Shape& second){
    return distance_humoments(first.hu_moments, second.hu_moments);
}

double Shape_Finder::distance_humoments(vector<double>& first,vector<double>& second ){
    if(first.size() != second.size()) throw 228;
    double sum = 0;
    for(int i = 0; i < first.size(); ++i){
        sum += (first[i] - second[i])*(first[i] - second[i]);
    }
    return sqrt(sum);
}

pair<int, int> Shape_Finder:: min_diff(vector<vector<pair<bool, double>>> &diffs){
    double min = 10e20;
    pair<int, int> indexes(-1, -1);
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            if(diffs[i][j].first && diffs[i][j].second < min){
                indexes = pair<int, int>(i, j);
                min = diffs[i][j].second;
            }
        }
    }
    return indexes;
};

void Shape_Finder::delete_cross(vector<vector<pair<bool, double>>>& diffs,pair<int, int>& index){
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            if(i == index.first || j == index.second){
                diffs[i][j].first = false;
            }
        }
    }
}

SHAPE_TYPE Shape_Finder::cast_shape_from_int(int n){
    if(n == 0) return SHAPE_TYPE::CIRCLE;
    else if(n == 1) return SHAPE_TYPE ::TRIANGLE;
    else if(n == 2) return SHAPE_TYPE ::CROSS;
    else
        return SHAPE_TYPE ::INTERSECTION;
}