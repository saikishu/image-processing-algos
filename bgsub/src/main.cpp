/*
 * Background Subtraction Algorithm
 * More info:
 * https://github.com/saikishu/image-processing-algos/blob/master/bgsub/README.md
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
#include <iomanip>
#include <stdlib.h>

#define IMG_NUM 620
#define TEMPORAL 0.01
#define THRESHOLD 44

int main() {

	std::cout << "Start..." << std::endl;

	//Initial values
	float temporal = TEMPORAL;
	float threshold = THRESHOLD;

	//Image naming convention: Prefix+3digit number
	std::string path_prefix = "Images/in000";
	cv::Mat img0 = cv::imread("Images/in000001.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!img0.data) {
		std::cout << "Terminating: Images couldn't be loaded" << std::endl;
		exit(1);
	}
	img0.convertTo(img0, CV_32FC1); //I0

	cv::Mat mean_prev = img0.clone(); //Initial mean = I0 intensity, At given t, mean_prev holds mean for t-1
	cv::Mat variance_prev; // At given t, variance_prev holds variance for t-1
	cv::Mat mean;
	cv::blur(img0, mean, cv::Size(3, 3));
	cv::Mat mean_squared; // E[(x)^2]
	cv::blur(img0.mul(img0), mean_squared, cv::Size(3, 3));
	cv::Mat square_of_mean = mean.mul(mean);
	variance_prev = mean_squared - square_of_mean;
	cv::Mat img;
	cv::Mat d; // holds Eucledian distance

	for (int i = 1; i <= IMG_NUM; i++) {
		std::ostringstream imageName;
		imageName << path_prefix << std::setw(3) << std::setfill('0') << i
				<< ".jpg";
		std::cout << imageName.str() << std::endl;
		img = cv::imread(imageName.str(), CV_LOAD_IMAGE_GRAYSCALE);
		img.convertTo(img, CV_32FC1);

		cv::Mat mean_t = temporal * img + (1 - temporal) * mean_prev;
		absdiff(img, mean_t, d); // d= |(It - Mut)|
		cv::Mat variance_t = (temporal * (d.mul(d)))
				+ ((1 - temporal) * variance_prev);

		//update mean_prev = mean_t and variance_prev = variance_t for next iteration
		mean_t.copyTo(mean_prev);
		variance_t.copyTo(variance_prev);
	}

	//Here img holds image number IMG_NUM and 'd' holds its eucledian distance
	//variance_prev holds variance for image IMG_NUM

	cv::Mat dist;
	divide(d, variance_prev, dist); // each element is either > threshold or <= threshold

	//Binary painting
	for (int i = 0; i < img.rows; i++) {
		float* img_row_ptr = img.ptr<float>(i);
		float* d_row_ptr = d.ptr<float>(i);

		for (int j = 0; j < img.cols; j++) {
			if (d_row_ptr[j] > threshold) {
				//Foreground
				img_row_ptr[j] = 255;
			} else {
				//Background
				img_row_ptr[j] = 0;
			}
		}
	}

	img.convertTo(img, CV_8UC1);
	cv::namedWindow("image0", CV_WINDOW_AUTOSIZE);
	cv::imshow("image0", img);

	try {
		cv::imwrite("in000620.jpg", img);
	} catch (std::runtime_error& ex) {
		fprintf(stderr, "Exception saving image: %s\n", ex.what());
		return 1;
	}

	std::cout << "Image saved as in000620.jpg" << std::endl;

	cvWaitKey(0);
	return 0;
}
