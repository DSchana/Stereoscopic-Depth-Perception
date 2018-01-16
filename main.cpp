/*
 * Title:	main.cpp
 * Author:	Dilpreet S. Chana
 * Description:	Dual camera range finder
**/

#include <cstdio>
#include <cmath>
#include <vector>

#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

#define FOCAL_LENGTH 60	      // Focal length of both camereas
#define CAMERA_DIFFERENCE 95  // Distance between cameras
#define FACE_CASCADE "data/haarcascade_frontalface_alt.xml"

using namespace cv;
using namespace std;

// Global constants
CascadeClassifier face_cascade;

double findTarget(Mat &frame1, Mat &frame2);
Point findFaces(Mat &frame);
double calculateDistance(float d);

int main() {
	printf("START\n");
	VideoCapture cap1(0);
	VideoCapture cap2(1);
	Mat frame1, frame2;

	if (!cap1.isOpened()) {
		printf("Cap1 fucked\n");
		return -1;
	}

	if (!cap2.isOpened()) {
		printf("Cap2 fucked\n");
		return -1;
	}

	if (!face_cascade.load(FACE_CASCADE)) {
		printf("Cascade fucked\n");
		return -1;
	}

	while (cap1.read(frame1) && cap2.read(frame2)) {
		printf("%f cm\n", calculateDistance(findTarget(frame1, frame2)));

		imshow("Cam1", frame1);
		imshow("Cam2", frame2);

		// Esc to end
		if (char(waitKey(10)) == 27) {
			break;
		}
	}

	return 0;
}

double findTarget(Mat &frame1, Mat &frame2) {
	Point p1 = findFaces(frame1);
	Point p2 = findFaces(frame2);

	return sqrt(pow(abs(p1.x - p2.x), 2) + pow(abs(p1.y - p2.y), 2));
}

Point findFaces(Mat &frame) {
	// Perhaps find faces for round 1
	vector<Rect> faces;
	Mat frame_gray, frame_gray_blur;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	GaussianBlur(frame_gray, frame_gray_blur, Size(3, 3), 0, 0);

	Rect main_face = Rect(0, 0, 0, 0);

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(80, 80));

	for (size_t i = 0; i < faces.size(); i++) {
		if (faces[i].width * faces[i].height > main_face.width * main_face.height) {
			main_face.x = faces[i].x;
			main_face.y = faces[i].y;
			main_face.width = faces[i].width;
			main_face.height = faces[i].height;
		}
	}

	return Point(main_face.x + main_face.width / 2, main_face.y + main_face.height / 2);
}

double calculateDistance(float d) {
	return FOCAL_LENGTH * CAMERA_DIFFERENCE / d;
}
