

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int main()
{

	cv::Mat imgColor1, imgColor2, imgDepth2, imgDepth1;        // input images
	imgColor1 = cv::imread("Data\\color_initial.png", CV_LOAD_IMAGE_COLOR);          // open images
	imgColor2 = cv::imread("Data\\color_final.png", CV_LOAD_IMAGE_COLOR);
	imgDepth1 = cv::imread("Data\\depth_initial.png", CV_LOAD_IMAGE_UNCHANGED);
	imgDepth2 = cv::imread("Data\\depth_final.png", CV_LOAD_IMAGE_UNCHANGED);

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

	ifstream myReadFile;
	Mat transformD_C = Mat::zeros(4, 4, DataType<float>::type);
	//int i = 0, j = 0;
	myReadFile.open("Data\\TransformationD-C");
	double output;
	double val;
	if (myReadFile.is_open()) {
		int cnt = 0;//index starts from 0
		cout << "TransformDC";
		while (myReadFile >> output)
		{
			int temprow = cnt / 4;
			int tempcol = cnt % 4;
			transformD_C.at<float>(temprow, tempcol) = output;
			cout << transformD_C.at<float>(temprow, tempcol);
			cnt++;
		}
	}
	myReadFile.close();

	// Read intrinsic RGB Matrix 
	ifstream myReadFile1;
	Mat intrinsicRGB = Mat::zeros(3, 3, DataType<float>::type);
	i = 0, j = 0;
	myReadFile1.open("Data\\IntrinsicRGB1");
	output = 0;
	if (myReadFile1.is_open()) {
		int cnt = 0;//index starts from 0
		cout << "Intrinsic RGB";
		while (myReadFile1 >> output)
		{
			int temprow = cnt / 3;
			int tempcol = cnt % 3;
			intrinsicRGB.at<float>(temprow, tempcol) = output;
			cout << intrinsicRGB.at<float>(temprow, tempcol);
			cnt++;
		}
	}
	myReadFile1.close();

	// Read inverse intrinsic Matrix 
	ifstream myReadFile2;
	//Mat invIntrinsic = Mat::zeros(3, 3, DataType<float>::type);
	i = 0, j = 0;
	myReadFile2.open("Data\\InvIntrinsicIR1");
	output = 1;
	if (myReadFile2.is_open()) {
		int cnt = 0;//index starts from 0
		cout << "Inv Intrinsic";
		while (myReadFile2 >> output)
		{
			int temprow = cnt / 3;
			int tempcol = cnt % 3;
			invIntrinsic.at<float>(temprow, tempcol) = output;
			cout << invIntrinsic.at<float>(temprow, tempcol);
			cnt++;
		}
	}
	myReadFile2.close();
	cout << "Done";
	
	// Initial Image
	for (int j = 0; j < imgDepth1.rows; j++)
		for (int i = 0; i < imgDepth1.cols; i++)
		{
			//int p = floor(depthimage.rows / 2);
			//int q = floor(depthimage.cols / 2);
			Mat pt = Mat::zeros(1, 3, DataType<float>::type);

			Mat tempmat = Mat::zeros(1, 3, DataType<float>::type);
			Mat tempmat1 = Mat::zeros(1, 4, DataType<float>::type);
			Mat tempmat2 = Mat::zeros(1, 4, DataType<float>::type);
			Mat tempmat3 = Mat::zeros(1, 3, DataType<float>::type);
			Mat tempmat4 = Mat::zeros(1, 3, DataType<float>::type);

			float x, y, z;
			x = i;
			y = j;
			z = static_cast<float>(imgDepth1.at<ushort>(j, i));

			pt.at<float>(0, 0) = x;
			pt.at<float>(0, 1) = y;
			pt.at<float>(0, 2) = 1;

			if (z > 1)
			{
				try
				{
					tempmat = pt * invIntrinsic;
					
					tempmat1.at<float>(0, 0) = tempmat.at<float>(0, 0)*z;
					tempmat1.at<float>(0, 1) = tempmat.at<float>(0, 1)*z;
					tempmat1.at<float>(0, 2) = tempmat.at<float>(0, 2)*z;
					tempmat1.at<float>(0, 3) = 1;

					tempmat2 = tempmat1 * transformD_C;

					tempmat3.at<float>(0, 0) = tempmat2.at<float>(0, 0) / tempmat2.at<float>(0, 2);
					tempmat3.at<float>(0, 1) = tempmat2.at<float>(0, 1) / tempmat2.at<float>(0, 2);
					tempmat3.at<float>(0, 2) = 1;
					
					tempmat4 = tempmat3 * intrinsicRGB;

					int cx, cy;
					cy = (int)tempmat4.at<float>(0, 0);
					cx = (int)tempmat4.at<float>(0, 1);
					
					if (cx < 0 || cy < 0)
					{
						continue;
					}
					if (cx >= imgColor1.rows || cy >= imgColor1.cols)
					{
						continue;
					}

					int b = (imgColor1.at<cv::Vec3b>(cx, cy)[0]);
					int g = (imgColor1.at<cv::Vec3b>(cx, cy)[1]);
					int r = (imgColor1.at<cv::Vec3b>(cx, cy)[2]);

					imgNew1.at<Vec3b>(j, i) = (Vec3b(b, g, r));
					
				}
				catch (cv::Exception const & e) {
					
					std::cerr << "OpenCV exception: Tempmat values\n" << tempmat4.at<float>(0, 0) << " " << tempmat4.at<float>(0, 1) << " " << tempmat4.at<float>(0, 2) << std::endl;

				}


			}

		}// end for
	
	// Initial Image
	for (int j = 0; j < imgDepth2.rows; j++)
		for (int i = 0; i < imgDepth2.cols; i++)
		{
			Mat pt = Mat::zeros(1, 3, DataType<float>::type);

			Mat tempmat = Mat::zeros(1, 3, DataType<float>::type);
			Mat tempmat1 = Mat::zeros(1, 4, DataType<float>::type);
			Mat tempmat2 = Mat::zeros(1, 4, DataType<float>::type);
			Mat tempmat3 = Mat::zeros(1, 3, DataType<float>::type);
			Mat tempmat4 = Mat::zeros(1, 3, DataType<float>::type);

			float x, y, z;
			x = i;
			y = j;
			z = static_cast<float>(imgDepth2.at<ushort>(j, i));

			pt.at<float>(0, 0) = x;
			pt.at<float>(0, 1) = y;
			pt.at<float>(0, 2) = 1;

			if (z > 1)
			{
				try
				{
					tempmat = pt * invIntrinsic;

					tempmat1.at<float>(0, 0) = tempmat.at<float>(0, 0)*z;
					tempmat1.at<float>(0, 1) = tempmat.at<float>(0, 1)*z;
					tempmat1.at<float>(0, 2) = tempmat.at<float>(0, 2)*z;
					tempmat1.at<float>(0, 3) = 1;

					tempmat2 = tempmat1 * transformD_C;

					tempmat3.at<float>(0, 0) = tempmat2.at<float>(0, 0) / tempmat2.at<float>(0, 2);
					tempmat3.at<float>(0, 1) = tempmat2.at<float>(0, 1) / tempmat2.at<float>(0, 2);
					tempmat3.at<float>(0, 2) = 1;

					tempmat4 = tempmat3 * intrinsicRGB;

					int cx, cy;
					cy = (int)tempmat4.at<float>(0, 0);
					cx = (int)tempmat4.at<float>(0, 1);

					if (cx < 0 || cy < 0)
					{
						continue;
					}
					if (cx >= imgColor2.rows || cy >= imgColor2.cols)
					{
						continue;
					}

					int b = (imgColor2.at<cv::Vec3b>(cx, cy)[0]);
					int g = (imgColor2.at<cv::Vec3b>(cx, cy)[1]);
					int r = (imgColor2.at<cv::Vec3b>(cx, cy)[2]);

					imgNew2.at<Vec3b>(j, i) = (Vec3b(b, g, r));

				}
				catch (cv::Exception const & e) {

					std::cerr << "OpenCV exception: Tempmat values\n" << tempmat4.at<float>(0, 0) << " " << tempmat4.at<float>(0, 1) << " " << tempmat4.at<float>(0, 2) << std::endl;

				}


			}

		}// end for

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