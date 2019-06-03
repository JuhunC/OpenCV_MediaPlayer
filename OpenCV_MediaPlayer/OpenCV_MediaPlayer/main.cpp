#pragma once
#include "opencv2/highgui/highgui.hpp"
// highgui - an interface to video and image capturing.

#include <iostream>
// The header files for performing input and output.

using namespace cv;
// Namespace where all the C++ OpenCV functionality resides.

using namespace std;
// For input output operations.

#define KEY_ESC 27
#define KEY_SPACE 32
#define KEY_NOTHING -1
#define KEY_A 97
#define KEY_D 100
#define KEY_ENTER 13

int key;
bool running = true;

long frame_cnt = 0;
bool rewind_sec = false;
bool rewind_forward_sec = false;
bool is_end = false;
bool reset_video = false;
#define REWIND_TIME_RATE 1
void printSettings() {
	std::cout << "*************************************" << std::endl;
	std::cout << "A : Rewind 1 second" << std::endl;
	std::cout << "D : Rewind forward 1 second" << std::endl;
	std::cout << "ESC : Exit the program" << std::endl;
	std::cout << "SPACE : Stop/Resume Video" << std::endl;
	std::cout << "********After End of the Video*******" << std::endl;
	std::cout << "Enter : Restart Video" << std::endl;
	std::cout << "OTHER KEYS : Exit the Program" << std::endl;
	std::cout << "*************************************" << std::endl;
}
void key_callback(int key) {
	if (is_end == false) {
		switch (key) {
		case KEY_NOTHING:
			break;
		case KEY_ESC:
			exit(1);
		case KEY_SPACE:
			running = !running;
			break;
		case KEY_A:
			rewind_sec = true;
			break;
		case KEY_D:
			rewind_forward_sec = true;
			break;
		default:
			std::cout << key << std::endl;
			//rewind_sec = true;
		}
	}
	else { // video ended
		switch (key) {
		case KEY_ENTER:
			is_end = false;
			reset_video = true;
			break;
		default:
			exit(1);
		}

	}
}
int main()
{
	VideoCapture cap("KakaoTalk_Video_20190513_2045_46_804.mp4");
	// cap is the object of class video capture that tries to capture Bumpy.mp4
	if (!cap.isOpened())  // isOpened() returns true if capturing has been initialized.
	{
		cout << "Cannot open the video file. \n";
		return -1;
	}

	// int fps = cap.get(CV_CAP_PROP_POS_MSEC); //get the frames per seconds of the video
	// The function get is used to derive a property from the element.
	// Example:
	// CV_CAP_PROP_POS_MSEC :  Current Video capture timestamp.
	// CV_CAP_PROP_POS_FRAMES : Index of the next frame.

	namedWindow("A_good_name", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
	// first argument: name of the window.
	// second argument: flag- types: 
	// WINDOW_NORMAL : The user can resize the window.
	// WINDOW_AUTOSIZE : The window size is automatically adjusted to fitvthe displayed image() ), and you cannot change the window size manually.
	// WINDOW_OPENGL : The window will be created with OpenGL support.

	/* iterate through first 10 frames */
	//for (int i = 0; i < 10000; i++)
	//{
	//	/* set pointer to frame index i */
	//	cap.set(cv::CAP_PROP_POS_MSEC, i);
	//	Mat  frame;
	//	/* capture the frame and do sth with it */
	//	if (!cap.read(frame)) {
	//		cout << "\n Cannot read the video file. \n";
	//		break;
	//	}
	//	imshow("A_good_name", frame);
	//	key = waitKey(0);
	//	key_callback(key);		
	//}
	
	while (1)
	{
		Mat frame;
		// Mat object is a basic image container. frame is an object of Mat.

		if (running == true) {
			if (rewind_sec == true) {
				int current_msec = cap.get(CAP_PROP_POS_MSEC);
				cap.set(CAP_PROP_POS_MSEC, current_msec - 1000* REWIND_TIME_RATE);
				//cap.set(CAP_PROP_POS_FRAMES, frame_cnt);
				rewind_sec = false;
			}
			else if (rewind_forward_sec == true) {
				int current_msec = cap.get(CAP_PROP_POS_MSEC);
				cap.set(CAP_PROP_POS_MSEC, current_msec + 1000 * REWIND_TIME_RATE);
				rewind_forward_sec = false;
			}
			else if (reset_video == true) {
				cap.set(CAP_PROP_POS_FRAMES, 0);
				reset_video = false;
			}
			else {
				if (!cap.read(frame)) // if not success, break loop
					  // read() decodes and captures the next frame.
				{
					is_end = true;
					cout << "\n Cannot read the video file. \n";
					key = waitKey();
					key_callback(key);
				}
				else {
					imshow("A_good_name", frame);
				}
			}
		}
		// first argument: name of the window.
		// second argument: image to be shown(Mat object).

		key = waitKey(30);
		key_callback(key);
		
	}

	return 0;
}