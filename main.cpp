#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

using namespace cv;
using namespace std;

Mat image;

void change_hsv(Mat &image, int hue_shift, int saturation, int value) {
	Mat hsv_image;
	cvtColor(image, hsv_image, CV_BGR2HSV);

	//-117 HUE, +72 SAT , threshhold 224
	// hsv 
	// 0 -> hue
	// 1 -> sat
	// 2 -> val ?

	for (int x = 0; x < hsv_image.cols; x++) {
		for (int y = 0; y < hsv_image.rows; y++) {
			auto &HSV_VALUES = hsv_image.at<Vec3b>(Point(x, y));
			//HSV_VALUES[0] = (HSV_VALUES[0] + hue_shift) % 180;
			HSV_VALUES[0] += hue_shift;
			HSV_VALUES[1] += saturation;
			HSV_VALUES[2] += value;
		}
	}
	//threshold(hsv_image, hsv_image, 224, 255, 0);
	cvtColor(hsv_image, image, CV_HSV2BGR);
}

int main( int argc, char** argv) 
{
	String imageName("C:\\Users\\maciej\\Downloads\\niedostrug.bmp");

	//Mat image;
	image = imread(imageName, IMREAD_COLOR);

	Mat grayscale_image;
	cvtColor(image, grayscale_image, COLOR_BGR2GRAY);
	
	Mat threshold_image;
	threshold(grayscale_image, threshold_image, 254, 255, 0);

	//imwrite("Threshold_image.jpg", threshold_image);

	struct RGB {
		uchar blue;
		uchar green;
		uchar red;
	};

	//vector<int> middle_y(threshold_image.cols-1, threshold_image.rows/2);
	vector<int> middle_y;

	for (int x = 0; x < threshold_image.cols; x++) {
		int y1, y2;
		for (int y = 0; y < threshold_image.rows; y++) {
			RGB &rgb = threshold_image.ptr<RGB>(y)[x];
			if (rgb.blue == 255) {
				y1 = y;
				break;
			}
		}
		for (int y = threshold_image.rows-1; y >= 0; y--) {
			RGB &rgb = threshold_image.ptr<RGB>(y)[x];
			if (rgb.blue == 255) {
				y2 = y;
				break;
			}
		}
		//middle_y[x] = ((y1 + y2) / 2);
		middle_y.push_back((y1 + y2) / 2);
	}
	
	RGB ALL_BLUE;
	ALL_BLUE.blue = 0;
	ALL_BLUE.red = 255;
	ALL_BLUE.green = 0;

	namedWindow("hsv", WINDOW_AUTOSIZE);

	const int AROUND = 100;
	const int BLOCK = 10;

	// image, hue_shift, saturation, value
	change_hsv(image, 200, 0, 0);
	cvtColor(image, image, COLOR_BGR2GRAY);
	threshold(image, image, 224, 255, 0);
	
	//imwrite("hsv_image.jpg", image);

	vector<int> white_sums;

	for (int x = 0; x < threshold_image.cols; ++x) {
		int white_count = 0;
		for (int y = middle_y[x] - AROUND; y <= middle_y[x] + AROUND; ++y) {
			RGB &rgb = image.ptr<RGB>(y)[x];
			if (rgb.blue == 255) {
				white_count++;
			}
		}
		white_sums.push_back(white_count);
	}
	
	vector<int> white_blocks;

	for (int i = 0; i < white_sums.size() ; i+=BLOCK) {
		int white_block_count = 0;
		for (int j = 0; j < BLOCK && i + j < white_sums.size(); j++) {
			white_block_count += white_sums[i + j];
		}
		white_blocks.push_back(white_block_count);
		cout << white_block_count << " ";
	}

	for (int i = 5; i < image.cols; i += 10) {
		circle(image, Point(i, middle_y[i]), 5, Scalar(0, 0, 255));
	}

	imshow("hsv", image);
	waitKey(0);

	return 0;
}

//namedWindow(imageName, WINDOW_AUTOSIZE);
//cout << threshold_image.cols << " " << threshold_image.rows;
//cout << threshold_image.at<Vec3b>(Point(0, 0));

//Vec3b & color = threshold_image.at<Vec3b>(Point(0, i));
//RGB &rgb = threshold_image.ptr<RGB>(i)[0];
//cout << int(rgb.blue) << " ";
//imshow(imageName, image);

	/*for (int y = 0; y < threshold_image.rows; y++) {
		RGB &rgb = threshold_image.ptr<RGB>(y)[0];
		cout << int(rgb.blue) << " ";
	}*/
