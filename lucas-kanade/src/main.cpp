 /*
 * Lucas Kanade Optical flow algorithm
 * More info:
 * https://github.com/saikishu/image-processing-algos/blob/master/lucas-kanade/README.md
 *
 * Copyright (c) 2015 Sai Kishore
 * Free to use under the MIT license
 * https://github.com/saikishu/image-processing-algos/blob/master/LICENSE
 *
 */

#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <time.h>
#include <vector>
#include <math.h>

#define DEVIATION 1.5
#define KERNELSIZE 5
static const double pi = 3.14159265358979323846;

int main() {

	clock_t t1, t2;
	t1 = clock();

	cv::Mat image0 = cv::imread("Images/image0.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat image1 = cv::imread("Images/image1.png", CV_LOAD_IMAGE_GRAYSCALE);

	if (!image0.data || !image1.data) {
		std::cout << "Terminating: Couldn't open input image" << std::endl;
		exit(1);
	}

	cv::Mat image0_blur = image0.clone();
	cv::Mat image1_blur = image1.clone();

	//Blur and apply filter2D
	cv::GaussianBlur(image0, image0_blur, cv::Size(KERNELSIZE, KERNELSIZE),
			DEVIATION);
	cv::GaussianBlur(image1, image1_blur, cv::Size(KERNELSIZE, KERNELSIZE),
			DEVIATION);
	cv::Mat fx1, fx2, fy1, fy2, fx, fy, ft;
	float data[5] = { 1, -8, 0, 8, -1 };
	cv::Mat kernelX = cv::Mat(1, 5, CV_32FC1, &data) / (float) 12;
	cv::Mat kernelY = cv::Mat(5, 1, CV_32FC1, &data) / (float) 12;
	cv::filter2D(image0_blur, fx1, -1, kernelX, cv::Point(-1, -1), 0,
			cv::BORDER_DEFAULT);
	cv::filter2D(image0_blur, fy1, -1, kernelY, cv::Point(-1, -1), 0,
			cv::BORDER_DEFAULT);
	cv::filter2D(image1_blur, fx2, -1, kernelX, cv::Point(-1, -1), 0,
			cv::BORDER_DEFAULT);
	cv::filter2D(image1_blur, fy2, -1, kernelY, cv::Point(-1, -1), 0,
			cv::BORDER_DEFAULT);

	fx = 0.5 * (fx1 + fx2);
	fy = 0.5 * (fy1 + fy2);
	ft = image1_blur - image0_blur;

	//Ignore first and last 4 columns and rows

	//LK Window size = 9 * 9;
	//Form A, Form b and Compute v : Av = b
	cv::Mat A = cv::Mat::zeros(81, 2, CV_8UC1);
	cv::Mat V = cv::Mat::zeros(2, 1, CV_8UC1);
	cv::Mat B = cv::Mat::zeros(81, 1, CV_8UC1);

	//flow mat
	cv::Mat Flow = cv::Mat::zeros(image0.rows, image0.cols, CV_32FC2); //same size as image0

	for (int i = 4; i < (fx.rows - 4); i++) {

		for (int j = 4; j < (fx.cols - 4); j++) {

			cv::Mat lkWindowX, lkWindowY, lkWindowT;
			lkWindowX = fx(cv::Rect(j - 4, i - 4, 9, 9));
			lkWindowY = fy(cv::Rect(j - 4, i - 4, 9, 9)); //banking on fact that both are same size
			lkWindowT = ft(cv::Rect(j - 4, i - 4, 9, 9));

			unsigned char colA0[81];
			unsigned char colA1[81];
			unsigned char colT0[81];
			std::vector<std::vector<unsigned char> > cols;

			for (int k = 0; k < 9; k++) {
				for (int l = 0; l < 9; l++) {
					colA0[l + 9 * k] = lkWindowX.at<uchar>(k, l);
					colA1[l + 9 * k] = lkWindowY.at<uchar>(k, l);
					colT0[l + 9 * k] = lkWindowT.at<uchar>(k, l);
				}
			}

			cv::Mat mColA0 = cv::Mat(81, 1, CV_8UC1, &colA0);
			cv::Mat mColA1 = cv::Mat(81, 1, CV_8UC1, &colA1);
			cv::Mat mColT0 = cv::Mat(81, 1, CV_8UC1, &colT0);

			mColA0.col(0).copyTo(A.col(0));
			mColA1.col(0).copyTo(A.col(1));
			mColT0.col(0).copyTo(B.col(0));

			//for calculations
			cv::Mat nA = cv::Mat::zeros(81, 2, CV_32FC1);
			cv::Mat nV = cv::Mat::zeros(2, 1, CV_32FC1);
			cv::Mat nB = cv::Mat::zeros(81, 1, CV_32FC1);

			A.convertTo(nA, CV_32FC1);
			B.convertTo(nB, CV_32FC1);
			cv::solve(nA, nB, nV, cv::DECOMP_NORMAL);
			nV.convertTo(V, CV_8UC1);

			//At this stage, v for a given window is calculated.
			//Store all v's
			Flow.at<cv::Vec2f>(i, j)[0] = nV.at<float>(0, 0); //motion in x direction
			Flow.at<cv::Vec2f>(i, j)[1] = nV.at<float>(1, 0); //motion in y direction
		}
	}

	//Draw motion vectors on image0
	int line_thickness = 1;
	CvScalar line_color = CV_RGB(255, 0, 0);
	cv::Mat finalImg;
	cvtColor(image0, finalImg, CV_GRAY2RGB); // for color lines
	int count = 0;
	double motionX = 0;
	double motionY = 0;

	for (int i = 4; i < (fx.rows - 4); i++) {

		for (int j = 4; j < (image0.cols - 4); j++) {

			CvPoint p, q;
			//Every line has origin on corresponding point
			p.x = j;
			p.y = i;

			q.x = j + (int) Flow.at<cv::Vec2f>(i, j)[0]; //from motion matrix - channel 1 is x displacement
			q.y = i + (int) Flow.at<cv::Vec2f>(i, j)[1]; // channel 2 is y displacement

			if ((p.x == q.x) && (p.y == q.y)) {
				continue; //no change => skip
			}
			motionX += (int) Flow.at<cv::Vec2f>(i, j)[0];
			motionY += (int) Flow.at<cv::Vec2f>(i, j)[1];
			count++;

			double angle = atan2((double) p.y - q.y, (double) p.x - q.x);
			double hypotenuse = sqrt(
					(p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));

			//Magnifying length for presentation and reduce density for better visual. Draw every 25th vector only.
			if (count % 25 == 0) {

				q.x = (int) (p.x - 2 * hypotenuse * cos(angle));
				q.y = (int) (p.y - 2 * hypotenuse * sin(angle));
				cv::line(finalImg, p, q, line_color, line_thickness, CV_AA, 0);

				//arrow tip
				p.x = (int) (q.x + 9 * cos(angle + pi / 4));
				p.y = (int) (q.y + 9 * sin(angle + pi / 4));
				cv::line(finalImg, p, q, line_color, line_thickness, CV_AA, 0);
				p.x = (int) (q.x + 9 * cos(angle - pi / 4));
				p.y = (int) (q.y + 9 * sin(angle - pi / 4));
				cv::line(finalImg, p, q, line_color, line_thickness, CV_AA, 0);
			}

		}
	}

	double avgMotionX = motionX / count;
	double avgMotionY = motionY / count;

	std::cout << "Average Motion in X = " << avgMotionX << std::endl;
	std::cout << "Average Motion in Y = " << avgMotionY << std::endl;

	std::vector<int> compression;
	compression.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression.push_back(9);

	try {
		cv::imwrite("output.png", finalImg, compression);
	} catch (std::runtime_error& ex) {
		fprintf(stderr, "Exception converting image to PNG format: %s\n",
				ex.what());
		return 1;
	}

	std::cout << "Image saved as output.png" << std::endl;

	t2 = clock();
	float diff = ((float) t2 - (float) t1);
	float seconds = diff / CLOCKS_PER_SEC;
	std::cout << "Execution time (in seconds): " << seconds << std::endl;

	//cv::waitKey(10);
	//getchar();
	return 0;

}
