#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include<fstream>

using namespace cv;
using namespace std;

void change_hsv(Mat &image, int hue_shift, int saturation, int value) 
{
	Mat hsv_image;
	cvtColor(image, hsv_image, CV_BGR2HSV);

	/*
	Values found for best data 
	(-117 HUE, +72 SAT , THRES 224)

		HSV: 
		0 - hue
		1 - sat
		2 - val
	*/

	for (int x = 0; x < hsv_image.cols; x++) 
	{
		for (int y = 0; y < hsv_image.rows; y++) 
		{
			auto &HSV_VALUES = hsv_image.at<Vec3b>(Point(x, y));
			HSV_VALUES[0] += hue_shift;
			HSV_VALUES[1] += saturation;
			HSV_VALUES[2] += value;
		}
	}
	cvtColor(hsv_image, image, CV_HSV2BGR);
}

int main( int argc, char** argv) 
{
	String imageName("C:\\Users\\maciej\\Downloads\\niedostrug.bmp");

	Mat image = imread(imageName, IMREAD_COLOR);

	Mat grayscale_image;
	cvtColor(image, grayscale_image, COLOR_BGR2GRAY);
	
	Mat threshold_image;
	threshold(grayscale_image, threshold_image, 254, 255, 0);
	cvtColor(threshold_image, threshold_image, COLOR_GRAY2BGR);
	//threshold(image, threshold_image, 254, 255, 0);

	Mat hsv_image = image.clone();

	struct RGB 
	{
		uchar blue;
		uchar green;
		uchar red;
	};

	vector<int> middle_y;
	vector<int> laser_width;
	vector<int> y1_vec;
	vector<int> y2_vec;

	for (int x = 0; x < threshold_image.cols; x++) 
	{
		int y1, y2;
		for (int y = 0; y < threshold_image.rows; y++) 
		{
			RGB &rgb = threshold_image.ptr<RGB>(y)[x];
			if (rgb.green == 255 || rgb.blue == 255 || rgb.red == 255) 
			{
				y1 = y;
				break;
			}
		}
		for (int y = threshold_image.rows-1; y >= 0; y--) 
		{
			RGB &rgb = threshold_image.ptr<RGB>(y)[x];
			if (rgb.green == 255 || rgb.blue == 255 || rgb.red == 255) 
			{
				y2 = y;
				break;
			}
		}
		y1_vec.push_back(y1);
		y2_vec.push_back(y2);
		middle_y.push_back((y1 + y2) / 2);
		laser_width.push_back(y2 - y1);
	}
	
	change_hsv(hsv_image, 200, 0, 0);
	threshold(hsv_image, hsv_image, 224, 255, 0);
	
	vector<int> white_sums;

	for (int x = 0; x < hsv_image.cols; x++) 
	{
		int white_count = 0;
		for (int y = 0; y < hsv_image.rows; y++) 
		{
			RGB &rgb = hsv_image.ptr<RGB>(y)[x];
			if (rgb.green == 255 || rgb.blue == 255 || rgb.red == 255) 
			{
				white_count++;
			}
		}
		white_sums.push_back(white_count - laser_width[x]);
	}

	for (int x = 0; x < hsv_image.cols; x++)
	{
		for (int y = y1_vec[x]; y < y2_vec[x]; ++y)
		{
			RGB &rgb = hsv_image.ptr<RGB>(y)[x];
			rgb.blue = 0;
			rgb.green = 0;
			rgb.red = 255;
		}
	}

	const int BLOCK = 25;
	const int BLOCK_LIMIT = 500;

	for (int i = BLOCK; i + BLOCK < hsv_image.cols; i += BLOCK)
	{
		int block = 0;
		for (int j = i - BLOCK; j < i + BLOCK; ++j)
		{
			block += white_sums[j];
		}

		if (block > BLOCK_LIMIT)
		{
			circle(image, Point(i, middle_y[i]), 4, Scalar(0, 255, 0, 0), 5);
		}
		else
		{
			circle(image, Point(i, middle_y[i]), 4, Scalar(0, 0, 255, 0), 5);
		}
	}

	//imwrite("finished.jpg", image);

	namedWindow("image", WINDOW_AUTOSIZE);
	imshow("image", image);
	waitKey(0);

	return 0;
}