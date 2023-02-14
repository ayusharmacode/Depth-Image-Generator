
#include<iostream>
#include<conio.h>
#include<fstream>
#include <math.h>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int main() {
	cv::Mat img1, img2;        // input images
	img1 = cv::imread("colorizedDepthImage1.jpg");          // open images
	img2 = cv::imread("colorizedDepthImage2.jpg");
	cv::Mat img = cv::imread("test.png");

	/*if (img1.empty() || img2.empty()) {                                  // if unable to open image
		std::cout << "error: image(s) not read from file\n\n";     // show error message on command line
		_getch();
		return(0);
	}*/

	//cv::Mat imgNew1 = Mat::zeros(img1.size(), img1.type());
	//cv::Mat imgNew2 = Mat::zeros(img2.size(), img2.type());

	//cv::Mat orangeBall;
	//cv::Mat purpleBall;
	//cv::inRange(img1, cv::Scalar(90, 37, 123), cv::Scalar(147, 86, 149), purpleBall);
	//cv::inRange(img1, cv::Scalar(255, 140, 85), cv::Scalar(255, 215, 80), orangeBall);

	int org_initialX, org_initialY, org_finalX, org_finalY;
	float org_sumX = 0, org_sumY = 0;
	int org_count = 0;
	int vio_initialX, vio_initialY, vio_finalX, vio_finalY;
	float vio_sumX = 0, vio_sumY = 0;
	int vio_count = 0;
	for (int y = 0; y < img1.rows; y++) {
		for (int x = 0; x < img1.cols; x++) {
			int b = img1.at<Vec3b>(y, x)[0];
			int g = img1.at<Vec3b>(y, x)[1];
			int r = img1.at<Vec3b>(y, x)[2];

			// purple ball
			if ((r<= 143 && r>=90)  && (g >= 37 && g <= 86) && (b <= 149 && b >= 123)) {
				vio_sumX += x;
				vio_sumY += y;
				vio_count++;
				//img1.at<Vec3b>(y, x)[2] = 255;
			}

			// orange ball
			else if ((r <= 145 && r >= 88) && (g >= 47 && g <= 97) && (b <= 9 && b >= 60)) {
				org_sumX += x;
				org_sumY += y;
				org_count++;
				//img1.at<Vec3b>(y, x)[0] = 255;
			}
		}
	}

	vio_initialX = (int)(vio_sumX / vio_count);
	vio_initialY = (int)(vio_sumY / vio_count);
	org_initialX = (int)(org_sumX / org_count);
	org_initialY = (int)(org_sumY / org_count);
	//img1.at<Vec3b>(initialY, initialX) = Vec3b(255, 0, 0);
	vio_sumX = vio_sumY = vio_count = 0;
	org_sumX = org_sumY = org_count = 0;

	for (int y = 0; y < img2.rows; y++) {
		for (int x = 0; x < img2.cols; x++) {
			int b = img2.at<Vec3b>(y, x)[0];
			int g = img2.at<Vec3b>(y, x)[1];
			int r = img2.at<Vec3b>(y, x)[2];

			// purple ball
			if ((r <= 143 && r >= 90) && (g >= 37 && g <= 86) && (b <= 149 && b >= 123)) {
				vio_sumX += x;
				vio_sumY += y;
				vio_count++;
				//img2.at<Vec3b>(y, x)[2] = 255;
				//img2.at<Vec3b>(y, x)[0] = img2.at<Vec3b>(y, x)[1] = 0;
			}
			// orange ball
			else if ((r <= 145 && r >= 88) && (g >= 47 && g <= 97) && (b <= 9 && b >= 60)) {
				org_sumX += x;
				org_sumY += y;
				org_count++;
				//img2.at<Vec3b>(y, x)[0] = 255;
				//img2.at<Vec3b>(y, x)[1] = img2.at<Vec3b>(y, x)[2] = 0;
			}
		}
	}

	vio_finalX = (int)(vio_sumX / vio_count);
	vio_finalY = (int)(vio_sumY / vio_count);
	org_finalX = (int)(org_sumX / org_count);
	org_finalY = (int)(org_sumY / org_count);
	//img2.at<Vec3b>(initialY, initialX) = Vec3b(255, 0, 0);

	float time = 2433 / 1000.0 ; // time duration in seconds
	float temp = (vio_finalX*vio_finalX - vio_initialX*vio_initialX) + 
		(vio_finalY*vio_finalY - vio_initialY*vio_initialY);
	float vio_dist = sqrt(temp);
	temp = (org_finalX*org_finalX - org_initialX*org_initialX) +
		(org_finalY*org_finalY - org_initialY*org_initialY);
	float org_dist = sqrt(temp);
	cout << "Violet: \n";
	cout << "Distance: " << vio_dist << "\nTime: " << time << endl;
	cout << (vio_dist / time);
	cout << "\nOrange: \n";
	cout << "Distance: " << vio_dist-- << "\nTime: " << time << endl;
	cout << (vio_dist / time);
	// creating windows
	cv::namedWindow("Colorized Depth Image 1", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Colorized Depth Image 2", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("Bounding Box 1", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("Bounding Box 2", CV_WINDOW_AUTOSIZE);

	// show windows
	cv::imshow("Colorized Depth Image 1", img1);
	cv::imshow("Colorized Depth Image 2", img2);
	//cv::imshow("Bounding Box 1", purpleBall);
	//cv::imshow("Bounding Box 2", purpleBall);

	// Saving the new image
	//imwrite("boundingBox1.jpg", imgNew1);
	//imwrite("boundingBox2.jpg", imgNew2);

	cv::waitKey(0);              // hold windows open until user presses a key
	return(0);
}