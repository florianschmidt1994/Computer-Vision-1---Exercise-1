#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <algorithm>
#include <vector>

using namespace cv;
using namespace std;

struct PointWithGradient {
    Point pos;
    int gradient;
};

class ComparePWG {
public:

    // Returns true if t1 has a higher gradient than t2
    bool operator()(PointWithGradient& t1, PointWithGradient& t2) {
        if (t1.gradient > t2.gradient) return true;
        return false;
    }
};

priority_queue<PointWithGradient, vector<PointWithGradient>, ComparePWG> pq, backupQueue;
Mat usedPixels;
Mat mask;
int edgeThresh = 1;
Mat original;

// define a trackbar callback
static void onTrackbar(int, void*)
{
    Mat im = original.clone();
    Mat up = usedPixels.clone();

    mask = Scalar::all(0);
    int i=0;
    while (!pq.empty()) {

        PointWithGradient t2 = pq.top();

        if(mask.at<uchar>(t2.pos.x-1, t2.pos.y-1) == 0) {
            i++;
            Vec3b c1 = im.at<Vec3b>(t2.pos);
            Scalar newVal = Scalar(c1[0], c1[1], c1[2]);
            int lo = edgeThresh/2;
            int up = edgeThresh/2;
            int flags = 4 + (255 << 8); // + FLOODFILL_MASK_ONLY;
            //Rect ccomp;

            floodFill(im, mask, t2.pos, newVal, 0, Scalar(lo, lo, lo), Scalar(up, up, up), flags);

        }



        /*
        // if the pixel at position t2 has not been used before
        if(up.at<uchar>(t2.pos) == 255) {

            // store the current pixel's color
            Vec3b c1 = im.at<Vec3b>(t2.pos);

            // create a region for the current pixel
             vector<Point> region;
             region.push_back(t2.pos);

            // mark pixel as used
            up.at<uchar>(t2.pos) = 0;

            // iterate over all 4-connected pixels
            for(int i=-1; i<=1; i+=2) {
             for(int j=-1; j<=1; j+=2) {

                 // Existiert der Punkt �berhaupt? Pixel am Rand haben ja nur 3 bzw. an den Ecken 2 4-connected Nachbarn
                 if(t2.pos.x + i >= 0 && t2.pos.x + i < im.cols && t2.pos.y + j >= 0 && t2.pos.y + j < im.rows) {
                     Point p = Point(t2.pos.x + i, t2.pos.y + j);
                     if(up.at<uchar>(p) == 255) {
                         // pixel not yet used
                         // color distance zwischen dem Punkt aus der Priority Queue (t2) und dem aktuellen Nachbarn (p) berechnen
                         Vec3b c2 = im.at<Vec3b>(p);
                         double colorDistance = sqrt((c1[0]-c2[0])*(c1[0]-c2[0]) + (c1[1]-c2[1])*(c1[1]-c2[1]) + (c1[2]-c2[2])*(c1[2]-c2[2]));
                         if(colorDistance < edgeThresh) {
                             region.push_back(p);
                             // TODO: diese 2 Zeilen entfernen
                             im.at<Vec3b>(p) = im.at<Vec3b>(t2.pos);
                             up.at<uchar>(p) = 0;
                         }
                     }
                 }
             }
            }

            // TODO: �ber region iterieren, Durchschnittsfarbe berechnen, und jeden Pixel mit dieser Farbe einf�rben!

        }*/


        backupQueue.push(t2);
        pq.pop();
    }


    cout << i << endl;

    pq.swap(backupQueue);
    cv::imshow("Edge map", im);
    cv::imshow("Mask", mask);
}

int main( int argc, char** argv )
{

    Mat src, src_gray;
    Mat grad;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    int c;

    /// Load an image
    src = imread(argv[1], 1 );
    original = src.clone();

    if( !src.data )
    { return -1; }

    GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

    /// Convert it to gray
    cvtColor( src, src_gray, CV_RGB2GRAY );

    /// Generate grad_x and grad_y
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;

    /// Gradient X
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

    usedPixels = grad.clone();

    mask.create(src.rows+2, src.cols+2, CV_8UC1);

    // �ber alle Pixel iterieren, um sie in die PriorityQueue einzusortieren
    for(int y=0;y<grad.rows;y++)
    {
        for(int x=0;x<grad.cols;x++)
        {
            // Gradient f�r jeden Pixel auslesen
            int gradient = grad.at<uchar>(Point(x,y));

            // usedPixels wird als wei�es Bild initialisiert, hier wird sp�ter gespeichert, welche Pixel beim Auslesen der Priority Queue schon verwendet wurden
            usedPixels.at<uchar>(Point(x,y)) = 255;

            PointWithGradient pwg = PointWithGradient();
            pwg.gradient = gradient;
            pwg.pos = Point(x,y);

            // Store PWG in PriorityQueue
            pq.push(pwg);

        }
    }

    // create a toolbar

    namedWindow("Edge map", 1);
    createTrackbar("threshold", "Edge map", &edgeThresh, 20, onTrackbar);
    onTrackbar(0, 0);

    cv::imshow("original", original);
    waitKey(0);

    return 0;
}

/*
Mat image0, image, gray, mask;
int ffillMode = 1;
int loDiff = 20, upDiff = 20;
int connectivity = 4;
int isColor = true;
bool useMask = false;
int newMaskVal = 255;

static void onMouse( int event, int x, int y, int, void* )
{
    if( event != EVENT_LBUTTONDOWN )
        return;

    Point seed = Point(x,y);
    int lo = ffillMode == 0 ? 0 : loDiff;
    int up = ffillMode == 0 ? 0 : upDiff;
    int flags = connectivity + (newMaskVal << 8) +
                (ffillMode == 1 ? FLOODFILL_FIXED_RANGE : 0);
    int b = (unsigned)theRNG() & 255;
    int g = (unsigned)theRNG() & 255;
    int r = (unsigned)theRNG() & 255;
    Rect ccomp;

    Scalar newVal = isColor ? Scalar(b, g, r) : Scalar(r*0.299 + g*0.587 + b*0.114);
    Mat dst = isColor ? image : gray;
    int area;

    if( useMask )
    {
        threshold(mask, mask, 1, 128, THRESH_BINARY);
        area = floodFill(dst, mask, seed, newVal, &ccomp, Scalar(lo, lo, lo),
                  Scalar(up, up, up), flags);
        imshow( "mask", mask );
    }
    else
    {
        area = floodFill(dst, seed, newVal, &ccomp, Scalar(lo, lo, lo),
                  Scalar(up, up, up), flags);
    }

    imshow("image", dst);
    cout << area << " pixels were repainted\n";
}
*/