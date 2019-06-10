#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <Windows.h>
#include <time.h>
#include <stdlib.h>
#include <Leap.h>
#include <SFML/Audio.hpp>
using namespace Leap;
using namespace std;
using namespace cv;

#define KEY_ESC 27
#define KEY_SPACE 32
#define KEY_NOTHING -1
#define KEY_A 97
#define KEY_D 100
#define KEY_ENTER 13
#define KEY_W 119
#define KEY_S 115

int key;
bool stoped_by_leap = false;
bool running = true;
long frame_cnt = 0;
bool rewind_sec = false;
bool rewind_forward_sec = false;
bool is_end = false;
bool reset_video = false;
int volume = 100;
sf::Sound sound;
cv::VideoCapture* cap;
#define VOL_INCR_RATE 20
#define VOL_DECR_RATE 20
#define REWIND_TIME_RATE 10
#define EYE_MIS_DELAY 10
void runLeap();
void printSettings();
void incrVol();
void decrVol();
void key_callback(int key);

Vector skip_preVec, skip_postVec;
Vector sound_preVec, sound_postVec;
int skip = 0; 
int skip_moment = 0;
int sound_moment = 0;
int play_moment = 0;
int ready = 0;
#define DELAY_TIME 1000
class MyListener : public Leap::Listener {
public:
	virtual void onConnect(const Leap::Controller &);
	virtual void onFrame(const Leap::Controller &);
};

void MyListener::onConnect(const Leap::Controller &)
{
	std::cout << "Connected." << std::endl;
}

void MyListener::onFrame(const Leap::Controller & controller) {

	const Leap::Frame frame = controller.frame();

	// get hands
	Leap::HandList hands = frame.hands();
	// get gesture
	Leap::GestureList gestures = frame.gestures();
	Leap::PointableList pointables = frame.pointables();

	// access to a hand.
	// the first sensed hand will be at index 0.
	hands[0];
	//hands[1];
	
	// get fingers from a hand.
	Leap::FingerList fingers = hands[0].fingers();
	// from thumb to little finger 0 ~ 4
	fingers[0] = Leap::Finger(pointables[0]);
	fingers[1] = Leap::Finger(pointables[1]);
	fingers[2] = Leap::Finger(pointables[2]);
	fingers[3] = Leap::Finger(pointables[3]);
	fingers[4] = Leap::Finger(pointables[4]); // little

	if (!hands[0].isValid()) {
		//std::cout << "당신의 손을 인식하지 못했습니다. 다시 시도해주세요!" << std::endl;
		skip_moment = 0;
		sound_moment = 0;
		play_moment = 0;
		skip_preVec = Vector(0, 0, 0), skip_postVec = Vector(0, 0, 0);
		sound_preVec = Vector(0, 0, 0), sound_postVec = Vector(0, 0, 0);
		Sleep(DELAY_TIME/10);
	}
	else
	{
		if (ready == 0)
		{
			//std::cout << "원하는 기능에 맞게 움직여주세요!!" << std::endl;
			ready = 1;
			Sleep(DELAY_TIME/100);
		}
		else if (ready == 1) {
			
			if (fingers[1].isExtended() && !fingers[4].isExtended() && !fingers[3].isExtended() && !fingers[2].isExtended())
			{

				if (skip_moment == 0) {
					//cout << "skip을 위해 움직여주세요!" << endl;
					skip_preVec = fingers[1].tipPosition();
					skip_moment = 1;
					//cout << "preVec:" << skip_preVec << endl;
					Sleep(DELAY_TIME);
				}
				else {
					skip_postVec = fingers[1].tipPosition();
					skip_moment = 0;
					//cout << "postVec:" << skip_postVec << endl;
					Vector subVec = skip_postVec - skip_preVec;
					//cout << "subVec:" << subVec << endl;
					if (subVec.x < 0 && subVec.magnitude() > 5.0f) {
						//cout << "동영상을 10초 전으로 이동했습니다." << endl;
						key_callback(KEY_A);
					}
					else if (subVec.x > 0 && subVec.magnitude() > 5.0f) {
						//cout << "동영상을 10초 후으로 이동했습니다." << endl;
						key_callback(KEY_D);
					}
					Sleep(DELAY_TIME);
					ready = 0;
				}
			}
			else if (fingers[1].isExtended() && fingers[2].isExtended() && !fingers[4].isExtended() && !fingers[3].isExtended()) {
				if (sound_moment == 0) {
					//cout << "소리를 위해 움직여주세요!" << endl;
					sound_preVec = fingers[1].tipPosition();
					sound_moment = 1;
					//cout << "preVec:" << sound_preVec << endl;
					Sleep(DELAY_TIME/10);
				}
				else {
					sound_postVec = fingers[1].tipPosition();
					sound_moment = 0;
					//cout << "postVec:" << sound_postVec << endl;
					Vector subVec = sound_postVec - sound_preVec;
					//cout << "subVec:" << subVec << endl;
					if (subVec.y < 0 && subVec.magnitude() > 10.0f) {
						//cout << "소리를 줄였습니다." << endl;
						key_callback(KEY_S);
					}
					else if (subVec.y > 0 && subVec.magnitude() > 10.0f) {
						//cout << "소리를 키웠습니다." << endl;
						key_callback(KEY_W);
					}
					Sleep(DELAY_TIME);
					ready = 0;
				}
			}
			else if (!fingers[4].isExtended() && !fingers[3].isExtended() && !fingers[2].isExtended() && !fingers[1].isExtended() && (play_moment == 1)) {
				//cout << "동영상을 멈춤/진행을 하였습니다." << endl;
				if (running == true) {
					stoped_by_leap = true;
				}
				else {
					stoped_by_leap = false;
				}
				key_callback(KEY_SPACE);
				play_moment = 0;
				Sleep(DELAY_TIME + 1000);
				ready = 0;
			}
			else if (fingers[4].isExtended() && fingers[3].isExtended() && fingers[2].isExtended() && fingers[1].isExtended() && (play_moment == 0)) {
				//cout << "동영상을 멈춤/진행을 위해 움직여주세요." << endl;
				play_moment = 1;
				//Sleep(DELAY_TIME);
			}
			
		}
	}
}

cv::Rect getLeftmostEye(std::vector<cv::Rect> &eyes)
{
	int leftmost = 99999999;
	int leftmostIndex = -1;
	for (int i = 0; i < eyes.size(); i++)
	{
		if (eyes[i].tl().x < leftmost)
		{
			leftmost = eyes[i].tl().x;
			leftmostIndex = i;
		}
	}
	return eyes[leftmostIndex];
}

std::vector<cv::Point> centers;
cv::Point lastPoint;

cv::Point stabilize(std::vector<cv::Point> &points, int windowSize)
{
	float sumX = 0;
	float sumY = 0;
	int count = 0;
	for (int i = std::max<int>(0, (int)(points.size() - windowSize)); i < points.size(); i++)
	{
		sumX += points[i].x;
		sumY += points[i].y;
		++count;
	}
	if (count > 0)
	{
		sumX /= count;
		sumY /= count;
	}
	return cv::Point(sumX, sumY);
}

int fcount = 0;
int ecount = 0;
int eye_mis_cnt = 0;
int eye_cnt = 0;
void detectEyes(cv::Mat &frame, cv::CascadeClassifier &faceCascade, cv::CascadeClassifier &eyeCascade)
{
	cv::Mat grayscale;
	cv::cvtColor(frame, grayscale, CV_BGR2GRAY); // convert image to grayscale
	cv::equalizeHist(grayscale, grayscale); // enhance image contrast 
	std::vector<cv::Rect> faces;
	faceCascade.detectMultiScale(grayscale, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(150, 150));
	bool is_face = true, is_eye = true;
	if (faces.size() == 0) {
		fcount++;
		if (fcount > 3) {
			//std::cout << "Face not detected! Pause" << std::endl;
			is_face = false;
			eye_mis_cnt++;
			if (eye_mis_cnt >= 10 && running == true && stoped_by_leap == false) {
				key_callback(KEY_SPACE);
				eye_mis_cnt = 0;
			}
		}
		return; // none face was detected
	}
	cv::Mat face = grayscale(faces[0]); // crop the face
	std::vector<cv::Rect> eyes;
	eyeCascade.detectMultiScale(face, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30)); // same thing as above    
	rectangle(frame, faces[0].tl(), faces[0].br(), cv::Scalar(255, 0, 0), 2);
	if (eyes.size() != 2) {
		ecount++;
		if (ecount > 3) {
			is_eye = false;
			eye_mis_cnt++;
			if (eye_mis_cnt >= EYE_MIS_DELAY && running == true && stoped_by_leap == false) {
				key_callback(KEY_SPACE);
				eye_mis_cnt = 0;
			}
			//std::cout << "Eyes not detected! Pause" << std::endl;
		}
		return; // both eyes were not detected
	}
	for (cv::Rect &eye : eyes)
	{
		rectangle(frame, faces[0].tl() + eye.tl(), faces[0].tl() + eye.br(), cv::Scalar(0, 255, 0), 2);
	}
	cv::Rect eyeRect = getLeftmostEye(eyes);
	cv::Mat eye = face(eyeRect); // crop the leftmost eye
	cv::equalizeHist(eye, eye);
	fcount = 0;
	ecount = 0;
	if (is_face == true && is_eye == true && stoped_by_leap==false) {
		eye_cnt++;
		if (eye_cnt >= EYE_MIS_DELAY && running == false) {
			key_callback(KEY_SPACE);
			std::cout << "Web" << std::endl;
			eye_cnt = 0;
		}
	}/*
	else {
		eye_mis_cnt++;
		if (eye_mis_cnt >= 10 && running == true) {
			key_callback(KEY_SPACE);
			eye_mis_cnt = 0;
		}
	}*/
}