#include "UsedPixelsMap.h"

using namespace cv;

UsedPixelsMap::UsedPixelsMap() { }

UsedPixelsMap::UsedPixelsMap(int rows, int colums) {
    this->usedPixels = Mat(rows, colums, CV_8UC1, Scalar(255));
}

bool UsedPixelsMap::isUsed(Point p) {
    return usedPixels.at<uchar>(p) != 255;
}

void UsedPixelsMap::setUsed(Point p) {
    usedPixels.at<uchar>(p) = 0;
}

void UsedPixelsMap::reset() {
    usedPixels = Scalar(255);
}