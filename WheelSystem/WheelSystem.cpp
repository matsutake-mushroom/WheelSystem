// WheelSystem.cpp : アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "Windows.h"
#include "Serial.h"
#include "soundVector.h"
#include "cmatplotlib.h"
#include "WF1974.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <conio.h>//for _kbhit
#include <thread>//for matplotlib thread
#include <time.h>//for clock

using namespace std;
void execExperiment(string filename, double duration_sec, int _mode);

class WheelSystem {
public:
	WheelSystem() {
		baudrate = 9600;
		updated = false;
		endflag = false;
		g.open();
		savefilename = "angle.txt";
	}
	WheelSystem(string filename) {
		baudrate = 9600;
		updated = false;
		endflag = false;
		g.open();
		savefilename = filename;
	}
	string savefilename;
	vector<float> angle_vec;
	Matplotlib g;
	int baudrate;
	bool updated, endflag;
	int count = 0, i = 0;
	int temp_value = 0;
	char buff[5];

	void plot() {
		//g.open();//mat
		g.screen(0, 0, 60, 400);//mat
		double t = 0;//mat
		double before = 0.0;//mat
		double dt = 0.01;//ms//mat
						 /*matplot*/
		while (!endflag) {
			g.line(t - dt, (double)before, t, (double)temp_value*0.18);
			before = temp_value * 0.18;
			t += dt;
			Sleep(2);
		}

		string temp_name = savefilename.substr(0, savefilename.find(".") - 1);
		g.save((string("figure_") + temp_name + ".svg").c_str());
		g.save((string("figure_") + temp_name + ".png").c_str());
		g.close();//mat
	}

	void loop() {
		Serial s(baudrate);
		//Sleep(2000);
		int n_received = 0;
		//int n_previous = 0;
		while (!_kbhit()) {
			n_received = s.available();
			//cout << n_received << endl;
			if (n_received < 1) {
				;//arduino is down
			}
			else {
				//cout << s.read() << flush;
				auto temp = s.read();
				if (temp != '\n') {
					updated = false;
					buff[i] = temp;
					++i;
				}
				else {
					string sensorvalue(buff);
					//cout << sensorvalue << endl;
					try {
						temp_value = stoi(sensorvalue);
						angle_vec.push_back((float)temp_value*0.18);//2000count = 360degree
						//cout << temp_value << endl;
					}
					catch (exception& e) {
						angle_vec.push_back(-1.0);//error
					}
					i = 0;
					updated = true;
				}
			}
		}
		endflag = true;
		return;
	}

	void timerloop(double duration_second) {
		clock_t start = clock();
		Serial s(baudrate);
		//Sleep(2000);
		int n_received = 0;
		//int n_previous = 0;
		while ((double)(clock()-start)/CLOCKS_PER_SEC < duration_second) {
			n_received = s.available();
			//cout << n_received << endl;
			if (n_received < 1) {
				;//arduino is down
			}
			else {
				//cout << s.read() << flush;
				auto temp = s.read();
				if (temp != '\n') {
					updated = false;
					buff[i] = temp;
					++i;
				}
				else {
					string sensorvalue(buff);
					//cout << sensorvalue << endl;
					try {
						temp_value = stoi(sensorvalue);
						angle_vec.push_back((float)temp_value*0.18);//2000count = 360degree
																	//cout << temp_value << endl;
					}
					catch (exception& e) {
						angle_vec.push_back(-1.0);//error
					}
					i = 0;
					updated = true;
				}
			}
		}
		endflag = true;
		return;
	}

	void print() {
		int size = angle_vec.size();
		ofstream ofs(savefilename);

		for (auto value : angle_vec) {
			ofs << value << endl;
		}
		ofs.close();
	}
};

int main(int argc, char** argv) {
	/*
	if (argc < 2) {
	cout << "Enter savefile name to the 1st argument" << endl;
	return -1;
	}
	WheelSystem wheel(argv[1]);
	int mode = stoi(argv[2]);//0:no sound 1:fastBGM 2:slowBGM
	*/

	clock_t start = clock();    // スタート時間
	double habit_time = 150;
	double exp_time = 420;
	double rest_time = 60;
	
	string argument;
	string temp;
	cout << "Input rat name(id): " << flush;
	cin >> temp;
	argument += (temp + "_");
	cout << "Input date: " << flush;
	cin >> temp;
	argument += temp;

	cout << "Which music to be played first? 1: fast, 2:slow =>";
	cin >> temp;
	//argument += (temp + ".txt");
	int variation = stoi(temp);
	/*
	cout << "Input mode (0: no BGM, 1:fast BGM (with 2 WF), 2:slow BGM (with 1 WF) =>";
	cin >> temp;
	*/
	string ext = ".txt";
	//1巡目
	string cond = "_silent";
	try {
		execExperiment(argument + cond + ext, habit_time, 0);
	}
	catch (exception &e) {
		cout << e.what() << endl;//なにもしない
	}
	//2巡目
	try {
		if (variation == 1) {
			cond = "_fast";
			execExperiment(argument + cond + ext, exp_time, 1);
		}
		else {
			cond = "_slow";
			execExperiment(argument + cond + ext, exp_time, 2);
		}
	}
	catch (exception &e) {
		cout << e.what() << endl;//なにもしない
	}
	//休憩時間
	clock_t now = clock();
	while (((double)clock() - now) / CLOCKS_PER_SEC < rest_time) {
		;
	}
	//3巡目
	try {
		if (variation == 1) {
			cond = "_slow";
			execExperiment(argument + cond + ext, exp_time, 2);
		}
		else {
			cond = "_fast";
			execExperiment(argument + cond + ext, exp_time, 1);
		}
	}
	catch (exception &e) {
		cout << e.what() << endl;//なにもしない
	}
	//おしまい
	while (!_kbhit()) {
		;
	}
}
void execExperiment(string filename, double duration_sec, int _mode){
	WheelSystem wheel(filename);
	int mode = _mode;

	cout << "Initializing the wheel experiment system..." << endl << filename << " mode: " << mode << endl;


	SoundVector sv;
	SoundVector sv2;
	WF1974 wf(9116124);
	WF1974 wf2(9148429);


	switch (mode) {
	case 0://no BGM
		break;
	case 1://fast BGM

		PureTone::addSoundPressureMap();//1
		PureTone::addSoundPressureMap();//2
		PureTone::soundCalibrationFromCSV(1, "soundCalibrationForWF9116124.csv");
		PureTone::soundCalibrationFromCSV(2, "soundCalibrationForWF9148429.csv");

		sv.createSequenceFromScore(1, "WilliamTell01.csv");
		sv2.createSequenceFromScore(2, "WilliamTell02.csv");

		wf.send(sv.createSequenceCommand(1));
		wf2.send(sv2.createSequenceCommand(1));
		
		wf.ch2.ON();
		wf2.ch2.ON();
		wf.sequence.compile();
		wf2.sequence.compile();
		break;
	case 2://slow BGM
		PureTone::addSoundPressureMap();//1
		PureTone::soundCalibrationFromCSV(1, "soundCalibrationForWF9116124.csv");
		sv.createSequenceFromScore(1, "NewWorldMov2.csv");

		wf.send(sv.createSequenceCommand());
		//wf2.ch2.OFF();
		wf.sequence.compile();
		break;
	default:
		break;
	}

	thread th([&wheel]() { wheel.plot(); });
	Sleep(3000);
	/*
	wf.send(":SYST:ERROR?");
	string msg = wf.receive();
	cout << "WF1:" << msg << endl;
	wf2.send(":SYST:ERROR?");
	msg = wf.receive();
	cout << "WF2:" << msg << endl;
	*/
	if (mode != 0) {
		wf.ch1.OFF();
		wf2.ch1.OFF();
	}
	cout << "Initialization finished." << endl;

	switch (mode) {
	case 0:
		break;
	case 1:

		wf2.ch2.ON();//1がスタート(のはず??????)
		wf2.sequence.stop();
		wf2.ch1.ON();
		wf2.ch2.ON();

		//2だけstart
		wf.sequence.start();
		wf.ch1.ON();
		break;
	case 2:
		wf.sequence.start();
		wf.ch1.ON();
		break;
	default:
		break;
	}

	cout << "Experiment Start!" << endl << "...Press any key to exit" << endl;
	//wheel.loop();//main LOOP
	wheel.timerloop(duration_sec);

	//if break the loop
	wf.sequence.stop();
	wf2.sequence.stop();
	wf.ch1.OFF();
	wf2.ch1.OFF();
	wf.ch2.OFF();
	wf2.ch2.OFF();
	th.join();

	wheel.print();
	return;
}

