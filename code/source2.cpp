

#include<iostream>
#include<conio.h>
#include<fstream>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int main() {
	cv::Mat imgColor1, imgColor2, imgDepth2, imgDepth1;        // input images
	imgColor1 = cv::imread("Data\\color_initial.png");          // open images
	imgColor2 = cv::imread("Data\\color_final.png");         
	imgDepth1 = cv::imread("Data\\depth_initial.png");       
	imgDepth2 = cv::imread("Data\\depth_final.png");         

	if (imgColor1.empty()|| imgColor2.empty() || imgDepth1.empty() || imgDepth1.empty()) {                                  // if unable to open image
		std::cout << "error: image(s) not read from file\n\n";     // show error message on command line
		_getch();                                               
		return(0);                                              
	}

	cv::Mat imgNew1 = Mat::zeros(imgDepth1.size(), imgColor1.type());
	cv::Mat imgNew2 = Mat::zeros(imgDepth2.size(), imgColor2.type());

	// to read the matrix files
	string intrinsicPath = "Data\\IntrinsicRGB1";
	string invIntrinsicPath = "Data\\InvIntrinsicIR1";
	string transformationPath = "Data\\TransformationD-C";

	// taking in all the intrinsic, inverse intrinsic and transformation matrices
	Mat intrinsic = Mat::zeros(3, 3, DataType<float>::type);
	Mat invIntrinsic = Mat::zeros(3, 3, DataType<float>::type);
	Mat transformation = Mat::zeros(4, 4, DataType<float>::type);

	std::ifstream inputFile;
	string line;
	int i = 0, j = 0;

	//intrinsic = fileToMatrix(intrinsicPath, 3);

	/*for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			cout << intrinsic.at<float>(i, j) << "    ";
		cout << endl;
	} */

	inputFile.open(intrinsicPath);
	if (inputFile.is_open()) {
		while (getline(inputFile, line, ' ')) {
			if (j == 3) {
				j = 0;
				i++;
			}
			intrinsic.at<float>(i, j) = std::stof(line);
			j++;

		}
	}

	inputFile.open(invIntrinsicPath);
	if (inputFile.is_open()) {
		while (getline(inputFile, line, ' ')) {
			if (j == 3) {
				j = 0;
				i++;
			}
			invIntrinsic.at<float>(i, j) = std::stof(line);
			j++;

		}
	}

	inputFile.open(transformationPath);
	if (inputFile.is_open()) {
		while (getline(inputFile, line, ' ')) {
			if (j == 4) {
				j = 0;
				i++;
			}
			transformation.at<float>(i, j) = std::stof(line);
			j++;

		}
	}
	
	// generating a colorized depth image using color image and depth image
	for (int y = 0; y < imgDepth1.rows; y++) {
		for (int x = 0; x < imgDepth1.cols; x++) {

			float z = static_cast<float>(imgDepth1.at<ushort>(y, x));

			if (z > 1) {
				//for (int c = 0; c < imgDepth1.channels(); c++) {

				cv::Mat point = cv::Mat::zeros(1, 3, DataType<float>::type);
				cv::Mat temp1 = cv::Mat::zeros(1, 3, DataType<float>::type);
				cv::Mat temp2 = cv::Mat::zeros(1, 4, DataType<float>::type);
				cv::Mat temp3 = cv::Mat::zeros(1, 4, DataType<float>::type);
				cv::Mat temp4 = cv::Mat::zeros(1, 3, DataType<float>::type);
				cv::Mat temp5 = cv::Mat::zeros(1, 3, DataType<float>::type);

				try {
					

					// 2D to 3D conversion of depth image
					point.at<float>(0, 0) = x;
					point.at<float>(0, 1) = y;
					point.at<float>(0, 2) = 1;

					temp1 = point * invIntrinsic;

					temp2.at<float>(0, 0) = temp1.at<float>(0, 0) * z;
					temp2.at<float>(0, 1) = temp1.at<float>(0, 1) * z;
					temp2.at<float>(0, 2) = temp1.at<float>(0, 2) * z;
					temp2.at<float>(0, 3) = 1;

					// transformation from depth image to color image
					temp3 = temp2 * transformation;

					// 3D to 2D conversion of color image
					temp4.at<float>(0, 0) = temp3.at<float>(0, 0) / temp3.at<float>(0, 2);
					temp4.at<float>(0, 1) = temp3.at<float>(0, 1) / temp3.at<float>(0, 2);
					temp4.at<float>(0, 2) = 1;

					
					temp5 = temp4 * intrinsic;


					int cx, cy;
					cy = (int)temp5.at<float>(0, 0);
					cx = (int)temp5.at<float>(0, 1);
					
					if (cx < 0 || cy < 0)
					{
						continue;
					}
					if (cx >= imgColor1.rows || cy >= imgColor1.cols)
					{
						continue;
					}

					int b = imgColor1.at<cv::Vec3b>(cx, cy)[0];
					int g = imgColor1.at<cv::Vec3b>(cx, cy)[1];
					int r = imgColor1.at<cv::Vec3b>(cx, cy)[2];

					imgNew1.at<Vec3b>(y, x) = (Vec3b(b, g, r));

				}
				catch (cv::Exception const &e) {
					std::cerr << "OpenCV exception: Template Values\n" << 
						temp5.at<float>(0, 0) << " " << temp5.at<float>(0, 1) 
						<< " " << temp5.at<float>(0, 2) << std::endl;
				}
			}
		}
	}

	
	/*
	// Final Image
	for (int y = 0; y < imgDepth2.rows; y++) {
		for (int x = 0; x < imgDepth2.cols; x++) {

			float z = static_cast<float>(imgDepth2.at<ushort>(x, y));

			if (z > 1) {
				//for (int c = 0; c < imgDepth1.channels(); c++) {

				cv::Mat point = cv::Mat::zeros(1, 3, DataType<float>::type);
				cv::Mat temp1 = cv::Mat::zeros(1, 3, DataType<float>::type);

				// 2D to 3D conversion of depth image
				point.at<float>(0, 0) = x;
				point.at<float>(0, 1) = y;
				point.at<float>(0, 2) = 1;

				temp1 = point * invIntrinsic;


				cv::Mat temp2 = cv::Mat::zeros(1, 4, DataType<float>::type);
				temp2.at<float>(0, 0) = temp1.at<float>(0, 0) * z;
				temp2.at<float>(0, 1) = temp1.at<float>(0, 1) * z;
				temp2.at<float>(0, 2) = temp1.at<float>(0, 2) * z;
				temp2.at<float>(0, 3) = 1;

				// transformation from depth image to color image
				cv::Mat temp3 = cv::Mat::zeros(1, 4, DataType<float>::type);
				temp3 = temp2 * transformation;

				// 3D to 2D conversion of color image
				cv::Mat temp4 = cv::Mat::zeros(1, 3, DataType<float>::type);
				temp4.at<float>(0, 0) = temp3.at<float>(0, 0) / temp3.at<float>(0, 2);
				temp4.at<float>(0, 1) = temp3.at<float>(0, 1) / temp3.at<float>(0, 2);
				temp4.at<float>(0, 3) = 1;

				cv::Mat temp5 = cv::Mat::zeros(1, 3, DataType<float>::type);
				temp5 = temp4 * intrinsic;


				int cx, cy;
				cy = (int)temp5.at<float>(0, 0);
				cx = (int)temp5.at<float>(0, 1);
				//cout << "x :" << cx << "y :" << cy << endl;


				if (cx < 0 || cy < 0)
				{
					//cout << "1";
					continue;
				}
				if (cx >= imgColor2.rows || cy >= imgColor2.cols)
				{
					continue;
				}

				int b = imgColor2.at<cv::Vec3b>(cx, cy)[0];
				int g = imgColor2.at<cv::Vec3b>(cx, cy)[1];
				int r = imgColor2.at<cv::Vec3b>(cx, cy)[2];

				imgNew2.at<Vec3b>(j, i) = (Vec3b(b, g, r));

			}
		}
	}
	*/
	// creating windows
	cv::namedWindow("Color Image 1", CV_WINDOW_AUTOSIZE);     
	cv::namedWindow("Color Image 2", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Depth Image 1", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Depth Image 2", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Colorized Depth Image 1", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Colorized Depth Image 2", CV_WINDOW_AUTOSIZE);
	
	// show windows
	cv::imshow("Color Image 1", imgColor1);    
	cv::imshow("Color Image 2", imgColor2);     
	cv::imshow("Depth Image 1", imgDepth1);     
	cv::imshow("Depth Image 2", imgDepth2);   
	cv::imshow("Colorized Depth Image 1", imgNew1);
	cv::imshow("Colorized Depth Image 2", imgNew2);

	// Saving the new image
	imwrite("colorizedDepthImage1.jpg", imgNew1);
	imwrite("colorizedDepthImage2.jpg", imgNew2);

	cv::waitKey(0);              // hold windows open until user presses a key
	return(0);
} 


