//
// Created by Florian Schmidt on 21/10/15.
//

#ifndef EXERCISE_1_USEDPIXELSMAP_H
#define EXERCISE_1_USEDPIXELSMAP_H

#include <opencv2/core/mat.hpp>

class UsedPixelsMap {
    cv::Mat usedPixels;

public:
    UsedPixelsMap();
    UsedPixelsMap(int rows, int colums);
    bool isUsed(cv::Point p);
    void setUsed(cv::Point);
    void reset();
};

#endif //EXERCISE_1_USEDPIXELSMAP_H
