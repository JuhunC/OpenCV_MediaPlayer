#pragma once
#include "opencv2/highgui/highgui.hpp"
// highgui - an interface to video and image capturing.
#include<SFML/Audio.hpp>
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
#define KEY_W 119
#define KEY_S 115

int key;
bool running = true;

long frame_cnt = 0;
bool rewind_sec = false;
bool rewind_forward_sec = false;
bool is_end = false;
bool reset_video = false;
sf::Sound sound;
VideoCapture* cap;
#define VOL_INCR_RATE 10
#define VOL_DECR_RATE 10
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
int volume = 100;
void incrVol() {
	if (volume + VOL_INCR_RATE <= 100) {
		volume += VOL_INCR_RATE;
	}
	else {
		volume = 100;
	}
}
void decrVol() {
	if (volume - VOL_DECR_RATE >= 0) {
		volume -= VOL_DECR_RATE;
	}
	else {
		volume = 0;
	}

}
void key_callback(int key) {
	if(key != -1)
		if (is_end == false) {
			std::cout << key << std::endl;
			switch (key) {
			case KEY_NOTHING:
				break;
			case KEY_ESC:
				exit(1);
			case KEY_SPACE:
				if (running) { 
					sound.pause(); 
					sound.setPlayingOffset(sf::milliseconds(sound.getPlayingOffset().asMilliseconds() - cap->get(CAP_PROP_POS_MSEC)));
				}
				else {
					sound.setPlayingOffset(sf::milliseconds(sound.getPlayingOffset().asMilliseconds() - cap->get(CAP_PROP_POS_MSEC)));
					sound.play();
				}
				running = !running;
				break;
			case KEY_A:
				rewind_sec = true;
				break;
			case KEY_D:
				rewind_forward_sec = true;
				break;
			case KEY_W:
				incrVol();
				sound.setVolume(volume);
				break;
			case KEY_S:
				decrVol();
				sound.setVolume(volume);
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
	int total_time; // in milliseconds
	std::string file_loc = "KakaoTalk_Video_20190513_2045_46_804.mp4";
	cap = new VideoCapture(file_loc);

	cap->set(CAP_PROP_POS_AVI_RATIO, 1);
	total_time = cap->get(CAP_PROP_POS_MSEC);
	cap->set(CAP_PROP_POS_AVI_RATIO, 0);

	sf::SoundBuffer buffer;
	

	// Create WAV file for audio
	std::string wave_loc = file_loc + ".wav";
	std::string ffmpeg_cmd = "ffmpeg.exe -i " + file_loc + " -vn " + wave_loc;
	system(ffmpeg_cmd.c_str());

	// cap is the object of class video capture that tries to capture Bumpy.mp4
	if (!cap->isOpened())  // isOpened() returns true if capturing has been initialized.
	{
		cout << "Cannot open the video file. \n";
		return -1;
	}

	if (!buffer.loadFromFile(wave_loc)) {
		cout << "Cannot open wave file. \n";
		return -1;
	}
	
	// int fps = cap->get(CV_CAP_PROP_POS_MSEC); //get the frames per seconds of the video
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

	
	sound.setBuffer(buffer);
	sound.setPitch(1.1f); // audio speed
	sound.setVolume(volume);
	sound.play();
	int frame_cnt = 0;
	while (1)
	{
		frame_cnt++;
		Mat frame;
		// Mat object is a basic image container. frame is an object of Mat.
		
		if (running == true) {
			// sync video with audio
			if (frame_cnt%10==0) {
				sound.setPlayingOffset(sf::milliseconds(cap->get(CAP_PROP_POS_MSEC)));
			}

			if (rewind_sec == true) {
				int current_msec = cap->get(CAP_PROP_POS_MSEC);
				cap->set(CAP_PROP_POS_MSEC, current_msec - 1000* REWIND_TIME_RATE);
				sound.setPlayingOffset(sf::milliseconds(cap->get(CAP_PROP_POS_MSEC)));
				//cap->set(CAP_PROP_POS_FRAMES, frame_cnt);
				rewind_sec = false;
			}
			else if (rewind_forward_sec == true) {
				int current_msec = cap->get(CAP_PROP_POS_MSEC);
				cap->set(CAP_PROP_POS_MSEC, current_msec + 1000 * REWIND_TIME_RATE);
				sound.setPlayingOffset(sf::milliseconds(cap->get(CAP_PROP_POS_MSEC)));

				rewind_forward_sec = false;
			}
			else if (reset_video == true) {
				cap->set(CAP_PROP_POS_FRAMES, 0);
				sound.resetBuffer();
				sound.play();
				reset_video = false;
			}
			else {
				if (!cap->read(frame)) // if not success, break loop
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