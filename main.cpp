#include "main.h"
#include "UsedPixelsMap.h"

struct PointWithGradient {
    Point pos;
    int gradient;
};

class ComparePWG {
public:

    // Returns true if t1 has a higher gradient than t2
    bool operator()(PointWithGradient& t1, PointWithGradient& t2) {
        return (t1.gradient > t2.gradient);
    }
};

// Maximum color distance derived from RGB(0,0,0) and RGB(255,255,255)
const int MAX_SQUARED_COLOR_DISTANCE = getSquaredColorDistance(Vec3b(0,0,0), Vec3b(255,255,255));
priority_queue<PointWithGradient, vector<PointWithGradient>, ComparePWG> pq, backupQueue;
UsedPixelsMap usedPixels;
int edgeThresh = 1;
Mat original;

int main( int argc, char** argv ) {

	Mat src, grad;

	//Load an image and handle errors
	if(argc != 2) {
        cout << "Usage: Main.out path/to/file" << endl;
        return -1;
    }

    src = imread(argv[1], 1 );

	if( !src.data ) {
        return -1;
    }

    original = src.clone();
    init(src, grad);

    // create a toolbar
	namedWindow("Result", WINDOW_AUTOSIZE);

    //Workaround to fix width of trackbar to width of window
    resizeWindow("Result", original.size().width,original.size().height);

	//Add onTrackbar callback and call it for the first time
    createTrackbar("threshold", "Result", &edgeThresh, 100, onTrackbar);
	onTrackbar(0, 0);

    //Show original image
	imshow("Original", original);

	waitKey(0);

	return 0;
}

int getSquaredColorDistance(Vec3b c1, Vec3b c2) {
    return (c1[0]-c2[0])*(c1[0]-c2[0]) + (c1[1]-c2[1])*(c1[1]-c2[1]) + (c1[2]-c2[2])*(c1[2]-c2[2]);
}

void init(const Mat &src, Mat &grad) {
    Mat srcGray;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    GaussianBlur(src, src, Size(3,3), 0, 0, BORDER_DEFAULT);

    /// Convert it to gray
    cvtColor(src, srcGray, CV_RGB2GRAY);

    /// Generate gradientX and gradientY
    Mat gradientX, gradientY;
    Mat absolutGradientX, absolutGradientY;

    /// Gradient X
    Sobel(srcGray, gradientX, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(gradientX, absolutGradientX);

    /// Gradient Y
    Sobel(srcGray, gradientY, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(gradientY, absolutGradientY);

    /// Total Gradient (approximate)
    addWeighted(absolutGradientX, 0.5, absolutGradientY, 0.5, 0, grad);

    usedPixels = UsedPixelsMap(src.rows, src.cols);

    // iterate over all pixels and add them to the priority queue
    for(int y=0;y<grad.rows;y++) {
		for(int x=0;x<grad.cols;x++) {
			// get the gradient for each pixel
			int gradient = grad.at<uchar>(Point(x,y));

			PointWithGradient pwg = PointWithGradient();
			pwg.gradient = gradient;
			pwg.pos = Point(x,y);

			// store pwg in priority queue
			pq.push(pwg);
		}
	}
}

void onTrackbar(int, void*) {

    Mat im = original.clone();
    usedPixels.reset();

    edgeThresh = edgeThresh * MAX_SQUARED_COLOR_DISTANCE / 100;

    //int squaredEdgeThresh = edgeThresh * edgeThresh;

    while (!pq.empty()) {

        PointWithGradient t2 = pq.top();
        backupQueue.push(t2);
        pq.pop();

        // if the pixel at position t2 has not been used before
        if(usedPixels.isUsed(t2.pos)) {
            continue;
        }

        // store the current pixel's color
        Vec3b c1 = im.at<Vec3b>(t2.pos);

        // create a region for the current pixel
        vector<Point> region;
        // stores the colors of all points belonging to the region
        vector<Vec3b> regionColors;
        stack<Point> stack;
        region.push_back(t2.pos);
        regionColors.push_back(c1);
        stack.push(t2.pos);

        // mark pixel as used
        usedPixels.setUsed(t2.pos);

        while(!stack.empty()) {
            Point p = stack.top();
            stack.pop();

            // iterate over all 4-connected pixels
            for(int i=-1; i<=1; i+=2) {
                for(int j=-1; j<=1; j+=2) {

                    bool pointExists = p.x + i >= 0 && p.x + i < im.cols && p.y + j >= 0 && p.y + j < im.rows;

                    if(pointExists) {
                        Point p2 = Point(p.x + i, p.y + j);
                        if(!usedPixels.isUsed(p2)) {
                            // pixel not yet used
                            // color distance zwischen dem Punkt aus der Priority Queue (t2) und dem aktuellen Nachbarn (p) berechnen
                            Vec3b c2 = im.at<Vec3b>(p2);
                            if(getSquaredColorDistance(c1,c2) <= edgeThresh) {
                                region.push_back(p2);
                                regionColors.push_back(c2);
                                usedPixels.setUsed(p2);
                                stack.push(p2);
                            }
                        }
                    }
                }
            }
        }

        // calculate mean color of region
        Scalar mc = (mean(regionColors));
        Vec3b meanColor = Vec3b(mc[0], mc[1], mc[2]);

        // color all points in region with corresponding meanColor
        for(std::vector<Point>::iterator it = region.begin(); it != region.end(); ++it) {
            im.at<Vec3b>(*it) = meanColor;
        }

    }

    pq.swap(backupQueue);
    imshow("Result", im);
}