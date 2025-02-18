#pragma once
#ifndef TP_CONNECTED_COMPONENTS_H
#define TP_CONNECTED_COMPONENTS_H

#include <opencv2/opencv.hpp>
#include <map>
#include <set>

using namespace cv;
using namespace std;

cv::Mat ccLabel(cv::Mat image);
cv::Mat ccAreaFilter(cv::Mat image, int size);
cv::Mat ccTwoPassLabel(cv::Mat image);
cv::Mat fillHoles(cv::Mat labeledImage);
std::map<int, std::set<int>> buildAdjacencyGraph(cv::Mat labeledImage);

#endif // TP_CONNECTED_COMPONENTS_H
