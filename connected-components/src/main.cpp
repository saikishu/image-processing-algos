/*
 * Connected Components Labeling Algorithm
 * More info:
 * https://github.com/saikishu/image-processing-algos/blob/master/connected-components/README.md
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
#include <stack>
#include <iomanip>
#include <stdlib.h>

#define NUM_OF_COLORS 16

int main() {
	std::cout << "Start..." << std::endl;

	//Load input image
	cv::Mat img = cv::imread("Images/components.png", CV_LOAD_IMAGE_GRAYSCALE);
	if (!img.data) {
		std::cout << "Image could not be opened" << std::endl;
		exit(1);
	}

	img.convertTo(img, CV_32FC1);

	//Current label
	int nLabel = 1;
	//holds labels for corresponding px in img; 0 => no label
	cv::Mat label = cv::Mat::zeros(img.rows, img.cols, CV_32FC1);
	//LIFO queue to hold interested pixels
	std::stack<CvPoint> labelQueue;
	//reusable - holds interested px position
	CvPoint pxPos = cvPoint(0, 0);
	//reusable - 8 connected window
	cv::Mat neighbors = cv::Mat::zeros(3, 3, CV_32FC1);

	for (int i = 0; i < img.rows; i++) {

		float* img_row_ptr = img.ptr<float>(i);
		float* label_row_ptr = label.ptr<float>(i);

		for (int j = 0; j < img.cols; j++) {
			//Foreground and no label exists
			if ((img_row_ptr[j] == 255) && (label_row_ptr[j] == 0)) {
				label_row_ptr[j] = nLabel;
				pxPos.x = j;
				pxPos.y = i;
				labelQueue.push(pxPos);
			} else {
				continue;
			}
			//connected neighbors
			while (!labelQueue.empty()) {
				pxPos = labelQueue.top();
				labelQueue.pop();
				//8 connected neighbors: given (x,y) 8 connected neighbor = 3 * 3 rectangle starting at (x-1,y-1)
				neighbors = img(cv::Rect(pxPos.x - 1, pxPos.y - 1, 3, 3));
				for (int k = 0; k < 3; k++) {
					float* neighbor_row_ptr = neighbors.ptr<float>(k);
					for (int l = 0; l < 3; l++) {

						CvPoint tempPos = cvPoint(pxPos.x + l - 1,
								pxPos.y + k - 1);
						float* label_row_ptr2 = label.ptr<float>(tempPos.y);

						if ((neighbor_row_ptr[l] == 255)
								&& (label_row_ptr2[tempPos.x] == 0)) {
							//Relative coordinates wrt to main image - coordinate translation
							label_row_ptr2[tempPos.x] = nLabel;
							labelQueue.push(tempPos);
						}
					}
				}
			} //end while loop
			nLabel++;
		}
	}

	//Colorful output image - 3 channels
	//TODO: May be better way to assign color, like on-the-fly conversion from hex to rgb
	cv::Mat colors = cv::Mat::zeros(1, NUM_OF_COLORS, CV_32FC3); // 1 x 16 to hold all 3 colors in 3 channels
	colors.at<cv::Vec3f>(0, 0) = cv::Vec3f(0, 255, 0);
	colors.at<cv::Vec3f>(0, 1) = cv::Vec3f(255, 0, 0);
	colors.at<cv::Vec3f>(0, 2) = cv::Vec3f(0, 128, 0);
	colors.at<cv::Vec3f>(0, 3) = cv::Vec3f(0, 128, 128);
	colors.at<cv::Vec3f>(0, 4) = cv::Vec3f(255, 0, 255);
	colors.at<cv::Vec3f>(0, 5) = cv::Vec3f(0, 255, 255);
	colors.at<cv::Vec3f>(0, 6) = cv::Vec3f(0, 215, 255);
	colors.at<cv::Vec3f>(0, 7) = cv::Vec3f(128, 128, 128);
	colors.at<cv::Vec3f>(0, 8) = cv::Vec3f(0, 0, 128);
	colors.at<cv::Vec3f>(0, 9) = cv::Vec3f(250, 230, 230);
	colors.at<cv::Vec3f>(0, 10) = cv::Vec3f(128, 0, 128);
	colors.at<cv::Vec3f>(0, 11) = cv::Vec3f(128, 0, 0);
	colors.at<cv::Vec3f>(0, 12) = cv::Vec3f(0, 0, 255);
	colors.at<cv::Vec3f>(0, 13) = cv::Vec3f(128, 128, 0);
	colors.at<cv::Vec3f>(0, 14) = cv::Vec3f(192, 192, 192);
	colors.at<cv::Vec3f>(0, 15) = cv::Vec3f(255, 255, 0);

	cv::Mat channels[] = { label, label, label }; //replicate same channel
	cv::Mat output;
	cv::merge(channels, 3, output);
	int colorIndex = 0;

	for (int i = 0; i < output.rows; i++) {
		for (int j = 0; j < output.cols; j++) {
			if (output.at<cv::Vec3f>(i, j)[0] != 0) {
				colorIndex = ((int) output.at<cv::Vec3f>(i, j)[0] - 1)
						% NUM_OF_COLORS; // color[index] index = label - 1; {(1,16) -> (0,15)}
				output.at<cv::Vec3f>(i, j) = colors.at<cv::Vec3f>(0,
						colorIndex);
			}
		}
	}

	output.convertTo(output, CV_8UC3);
	cv::namedWindow("output", CV_WINDOW_AUTOSIZE);
	cv::imshow("output", output);
	try {
		cv::imwrite("output.jpg", output);
	} catch (std::runtime_error& ex) {
		fprintf(stderr, "Exception saving image: %s\n", ex.what());
		return 1;
	}

	std::cout << "Image saved as output.jpg" << std::endl;

	cvWaitKey(0);
	return 0;
}
