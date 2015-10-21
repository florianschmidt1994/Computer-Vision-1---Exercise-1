#ifndef main_h
#define main_h
 
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <algorithm>
#include <vector>
#include <stack>

using namespace cv;
using namespace std;

int getSquaredColorDistance(Vec3b c1, Vec3b c2);
void init(const Mat &src, Mat &grad);
 
#endif