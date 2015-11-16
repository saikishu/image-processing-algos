#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <time.h>

int main()
{

  cv::Mat image1;
  image1 = cv::imread("4kresolution.jpg");

  if (!image1.data)
  {
	std::cout << "Could not open input image" << std::endl;
	getchar();
  }

  cv::Mat image2 = image1.clone();
  cv::Mat image3 = image1.clone();



  /*cv::namedWindow("Input", CV_WINDOW_AUTOSIZE);
  cv::imshow("Input", image1);
  cv::imwrite("input_image.jpg", image1);
  cv::waitKey(10);*/


  //--------------------METHOD 1------------------------------


  clock_t t1, t2;
  t1 = clock();
  for (int i = 0; i < image1.rows; i++)
  {
	for (int j = 0; j < image1.cols; j++)
	{
	  image1.at<cv::Vec3b>(i, j)[0] = abs(255 - image1.at<cv::Vec3b>(i, j)[0]);
	  image1.at<cv::Vec3b>(i, j)[1] = abs(128 - image1.at<cv::Vec3b>(i, j)[1]);
	  image1.at<cv::Vec3b>(i, j)[2] = abs(64 - image1.at<cv::Vec3b>(i, j)[2]);

	}
  }
  t2 = clock();
  float diff = ((float)t2 - (float)t1);
  float seconds = diff / CLOCKS_PER_SEC;

  std::cout << "Seconds Method 1: " << seconds << std::endl;


  //--------------------METHOD 2------------------------------


  clock_t t3, t4;
  t3 = clock();
  int step = image1.step;
  int channels = image1.channels();
  uchar *data = image1.data;
  for (int i = 0; i < image1.rows; i++)
  {
	for (int j = 0; j < image1.cols; j++)
	{
	  data[i*step + j*channels] = abs(255 - data[i*step + j*channels]);
	  data[i*step + j*channels + 1] = abs(128 - data[i*step + j*channels + 1]);
	  data[i*step + j*channels + 2] = abs(64 - data[i*step + j*channels + 2]);
	}
  }
  t4 = clock();
  diff = ((float)t4 - (float)t3);
  seconds = diff / CLOCKS_PER_SEC;

  std::cout << "Seconds Method 2: " << seconds << std::endl;


  //--------------------METHOD 3------------------------------


  clock_t t5, t6;
  t5 = clock();
  int channels2 = image1.channels();
  if (image1.isContinuous())
  {
	uchar *data = image1.data;
	int end = image1.rows*image1.cols*channels2;
	for (int i = 0; i < end; i += 3)
	{
	  data[i] = abs(255 - data[i]);
	  data[i + 1] = abs(128 - data[i + 1]);
	  data[i + 2] = abs(64 - data[i + 2]);
	}

  }
  else
	std::cout << "Image not continuous" << std::endl;

  t6 = clock();
  diff = ((float)t6 - (float)t5);
  seconds = diff / CLOCKS_PER_SEC;

  std::cout << "Seconds Method 3: " << seconds << std::endl;

  /*cv::namedWindow("Output", CV_WINDOW_AUTOSIZE);
  cv::imshow("Output", image1);
  cv::imwrite("output_image.jpg", image1);
  cv::waitKey(10);*/

  getchar();

  return 0;
}
