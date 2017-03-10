//
// Created by nikita on 10.03.17.
//

#ifndef SEARCHER_SHAPE_FINDER_H
#define SEARCHER_SHAPE_FINDER_H

#include "My_exeptions.h"
#include <map>

enum class SHAPE_TYPE{
    CIRCLE, TRIANGLE, CROSS, INTERSECTION};

struct Shape{
    Shape(Moments& mu){
        center = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
        HuMoments(mu, hu_moments);
    }
    Shape(){

    }
    Point2f center;
    vector<double> hu_moments;
};

class Shape_Finder {
    std::map<SHAPE_TYPE, Shape> sample_shapes;
    vector<pair<SHAPE_TYPE, Point2f>> defined_centers;
    vector<Vec4i> hierarchy;
    std::map<SHAPE_TYPE, Shape> define_shapes(vector<Shape>& all_shapes);
    pair<SHAPE_TYPE, double> closest_shape(Shape& shape);
    double shape_difference(Shape& first, Shape& second);
    double distance_humoments(vector<double>& first,vector<double>& second);

    pair<int, int> min_diff(vector<vector<pair<bool, double>>> &diffs);
    void delete_cross(vector<vector<pair<bool, double>>>& diffs,pair<int, int>& index);
    SHAPE_TYPE cast_shape_from_int(int n);
    void fill_diffs(vector<vector<pair<bool, double>>>& diffs, vector<Shape> &all_shapes);
public:
    Shape_Finder() = delete;
    Shape_Finder(const Shape_Finder& right) = delete;
    Shape_Finder(Mat& circle, Mat& triangle, Mat& cross);
    std::map<SHAPE_TYPE, Shape> find_primitives(Mat& target_image);
};


#endif //SEARCHER_SHAPE_FINDER_H
