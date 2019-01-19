#include "soundVector.h"

using namespace std;
/*******************
* sound unit
* puretone, silence, click
*******************/

SoundUnit::SoundUnit(int len) {
	length = len;//ms
}
void SoundUnit::writeSequenceStep(stringstream &output, int step, bool isEnd) {
	string comma = string(",");
	output << "STEP#" << step << "=";
	output << showpoint << (double)length << "E-03" << comma;//����
	output << string((isEnd) ? "STOP" : "CONT") << comma;//�Ōォ�p����
	output << "OFF" << comma;//�I�[�g�z�[���h(�Ƃ́H)
	output << 1 << comma;//�X�e�b�v�R�[�h(�Ƃ́H)
	output << "OFF" << comma << 1 << comma;//�X�e�[�g�u�����`�X�e�b�v����H[ON/OFF]�A���̃X�e�b�v�l
	output << "OFF" << comma << 1 << comma;//�C�x���g�u�����`�X�e�b�v����H[ON/OFF]�A���̃X�e�b�v�l
	output << ((isEnd)?"ON":"OFF") << comma << 1 << comma; //�W�����v�X�e�b�v����H[ON/OFF]�A���̃X�e�b�v�l
	output << "INF" << comma << 1 << comma;//�W�����v�񐔎w�肷��H[INF/ON]�A�W�����v��[ON�̂Ƃ�](0���ƃR���p�C������Ȃ��I�I�I�I)
	output << "OFF" << comma << 0.0;//�I���ʑ��w�肷��H[ON/OFF]�A���̈ʑ�
	output << endl;
}

vector<map<pair<double, int>, double>> PureTone::sound_pressure_map_vector;//static�̎���
std::map<std::pair<double, int>, double> PureTone::default_sound_pressure_map;//����, SPM[0]
bool PureTone::sound_pressure_map_is_initialized = false;

void PureTone::initSoundPressureMap(){
	auto it = sound_pressure_map_vector.begin();
	sound_pressure_map_vector.insert(it, default_sound_pressure_map);
	sound_pressure_map_is_initialized = true;
}

PureTone::PureTone(double kHz, int dB, int len) : SoundUnit(len) {
	//use default SPM
	if (!sound_pressure_map_is_initialized) {
		initSoundPressureMap();
	}

	frequency = kHz;
	name = to_string(frequency) + " kHz";
	voltage = sound_pressure_map_vector[0][{kHz, dB}];
}

PureTone::PureTone(int SoundMapID, double kHz, int dB, int len) : SoundUnit(len) {
	//use registered SPM
	frequency = kHz;
	name = to_string(frequency) + " kHz";

	if (SoundMapID > sound_pressure_map_vector.size()) {
		voltage = sound_pressure_map_vector[0][{kHz, dB}];//use default
	}
	else {
		voltage = sound_pressure_map_vector[SoundMapID][{kHz, dB}];
	}
}

void PureTone::writeSequenceChannel(stringstream &output, int step, int channel) {
	string comma = ",";
	output << "CHAN#" << step << "#" << channel << "=";
	output << "SIN" << comma;//�g�`
	output << "0" << comma;//�C�Ӕg�ԍ�
	output << "NORM" << comma;//�ɐ�(NORMal, INVert)
	output << "FS" << comma;//�g�`����(PFS->���AFS->����, MFS->��
	output << "OFF" << comma;//���`�g�g��(OFF/ON)

	output << showpoint << (double)(frequency*1000.0) << comma;//���g��(����)
	output << "CONS" << comma;//����(KEEP/CONStant/SWEep)

	output << showpoint << (double)voltage << comma;//�U��(real number)
	output << "CONS" << comma;//����(KEEP/CONStant/SWEep)

	output << 0.00 << comma;//DC offset
	output << "CONS" << comma;//����

	output << 0.00 << comma;//Phase
	output << "CONS" << comma;//dousa

	output << 50.0 << comma;//Duty[%],real number
	output << "CONS" << endl;//dousa
							 //CH2 for debug
	output << "CHAN#" << step << "#" << 2 << "=SIN,0,NORM,FS,OFF,1000.00000000,CONS,1.0000E-01,CONS,0.0000E+00,CONS,0.000,CONS,50.0000,CONS" << endl;
}

int PureTone::addSoundPressureMap(){
	if (!sound_pressure_map_is_initialized) {
		initSoundPressureMap();
	}
	std::map<std::pair<double, int>, double> new_member;
	sound_pressure_map_vector.push_back(new_member);
	auto itr = find(sound_pressure_map_vector.begin(), sound_pressure_map_vector.end(), new_member);
	return std::distance(sound_pressure_map_vector.begin(), itr);
}

void PureTone::soundCalibration(double kHz, int dB, double voltage){
	if (!sound_pressure_map_is_initialized) {
		initSoundPressureMap();
	}
	sound_pressure_map_vector[0][{kHz, dB}] = voltage;
}

void PureTone::soundCalibration(int soundMapID, double kHz, int dB, double voltage) {
	int id = soundMapID;
	if (soundMapID > sound_pressure_map_vector.size() || soundMapID < 0) {
		id = 0;
	}
	sound_pressure_map_vector[id][{kHz, dB}] = voltage;
}

void PureTone::soundCalibrationFromCSV(string filename){
	/**********************
	* �p�ӂ���csv�̏���
	*�@��s�ځF���ږ�
	*�@��s�ڈȍ~�F(double)kHz, (int)dB, (double)voltage�̏�
	*�@�l��ڈȍ~�͔��l����Ă悢
	***********************/
	CSV csvfile(filename);
	for (int i = 1; i < csvfile.Row; ++i) {
		soundCalibration(stod(csvfile[i][0]), stoi(csvfile[i][1]), stod(csvfile[i][2]));
	}
}

void PureTone::soundCalibrationFromCSV(int soundMapID, string filename) {
	int id = soundMapID;
	if (soundMapID > sound_pressure_map_vector.size() || soundMapID < 0) {
		id = 0;
	}

	CSV csvfile(filename);
	for (int i = 1; i < csvfile.Row; ++i) {
		soundCalibration(id, stod(csvfile[i][0]), stoi(csvfile[i][1]), stod(csvfile[i][2]));
	}
}

Click::Click(int len) : SoundUnit(len) {
	name = "Click";
	voltage = 15.0;//�����l
}

void Click::writeSequenceChannel(stringstream &output, int step, int channel) {//���`�g
	string comma = ",";
	output << "CHAN#" << step << "#" << channel << "=";
	output << "SQU" << comma;//�g�`
	output << "0" << comma;//�C�Ӕg�ԍ�
	output << "NORM" << comma;//�ɐ�(NORMal, INVert)
	output << "FS" << comma;//�g�`����(PFS->���AFS->����, MFS->��
	output << "OFF" << comma;//���`�g�g��(OFF/ON)

	output << showpoint << (double)(1.0) << comma;//���g��(����)
	output << "CONS" << comma;//����(KEEP/CONStant/SWEep)
	
	output << showpoint << (double)voltage << comma;//�U��(real number)
	output << "CONS" << comma;//����(KEEP/CONStant/SWEep)

	output << 0.00 << comma;//DC offset
	output << "CONS" << comma;//����

	output << 0.00 << comma;//Phase
	output << "CONS" << comma;//����

	output << 0.025 << comma;//Duty[%],real number
	output << "CONS" << endl;//����
							 //CH2 for debug
	output << "CHAN#" << step << "#" << 2 << "=SIN,0,NORM,FS,OFF,1000.00000000,CONS,1.0000E-01,CONS,0.0000E+00,CONS,0.000,CONS,50.0000,CONS" << endl;
}


Silence::Silence(int len) : SoundUnit(len) {
	name = "Silence";
}
void Silence::writeSequenceChannel(stringstream &output, int step, int channel) {//���Ȃ�
	string comma = ",";
	output << "CHAN#" << step << "#" << channel << "=";
	output << "SIN" << comma;//�g�`
	output << "0" << comma;//�C�Ӕg�ԍ�
	output << "NORM" << comma;//�ɐ�(NORMal, INVert)
	output << "FS" << comma;//�g�`����(PFS->���AFS->����, MFS->��
	output << "OFF" << comma;//���`�g�g��(OFF/ON)

	output << showpoint << (double)(1000) << comma;//���g��(����)
	output << "CONS" << comma;//����(KEEP/CONStant/SWEep)

	output << "0.0000E+00" << comma;//�U��(real number)
	output << "CONS" << comma;//����(KEEP/CONStant/SWEep)

	output << "0.0000E+00" << comma;//DC offset
	output << "CONS" << comma;//����

	output << "0.0000E+00" << comma;//Phase
	output << "CONS" << comma;//����

	output << 50.0 << comma;//Duty[%],real number
	output << "CONS" << endl;//����
	//CH2 for debug
	output << "CHAN#" << step << "#" << 2 << "=SIN,0,NORM,FS,OFF,1000.00000000,CONS,1.0000E-01,CONS,0.0000E+00,CONS,0.000,CONS,50.0000,CONS" << endl;
}

/*******************
* sound vector
* to generate WF1974 sequence file
*******************/

string SoundVector::createSequenceCommand() {
	string sequenceFile;
	string sequenceCommand;
	int digit = 0;

	sequenceFile += "[FILE]\nVERSION=\"1.00\"\n[SYSTEM]\nMODEL=\"WF1974\"\nNCHAN=2\nVERSION=\"1.00\"\n[DATA]\n";
	sequenceFile += "SEQ=1,SSYN,OFF,STAR,POS\n";
	sequenceFile += createSequenceDataString();

	digit = to_string(sequenceFile.length()).size();
	sequenceCommand = ":SOUR1:SEQ:STAT ON;"; //CH1��V�[�P���X���[�h��
	sequenceCommand += ":TRAC:SEQ 0,\"TEST\",#";//SEQ�f�[�^���o�́B������(0~10)�A���O�A�f�[�^�u�v
	sequenceCommand += to_string(digit) + to_string(sequenceFile.length() + 1) + "\n";
	sequenceCommand += sequenceFile;
	return sequenceCommand;
}

string SoundVector::createSequenceDataString() {
	stringstream ss;
	int channel = 1;
	int step = 1;

	ss << "STEP#0 = 1.0000E+00, CONT, OFF, 1, OFF, 1, OFF, 1, OFF, 1, INF, 1, OFF, 0.000\n";
	ss << "CHAN#0#1 = DC, 0, NORM, FS, OFF, 1000.00000000, CONS, 0.0000E+00, CONS, 0.0000E+00, CONS, 0.000, CONS, 50.0000, CONS\n";
	ss << "CHAN#0#2 = SIN, 0, NORM, FS, OFF, 1000.00000000, CONS, 1.0000E-01, CONS, 0.0000E+00, CONS, 0.000, CONS, 50.0000, CONS\n";

	for (auto i = sequence.begin(); i<sequence.end(); ++i) {
		(*i)->writeSequenceStep(ss, step, ((i+1) == sequence.end()) ? true : false);
		(*i)->writeSequenceChannel(ss, step, channel);
		++step;
	}
	return ss.str();
}
void SoundVector::resetSequence() {
	for (auto i = sequence.begin(); i<sequence.end(); ++i) {
		delete (*i);
	}
	sequence.clear();
}
void SoundVector::createRhythmSequence() {
	/*
	*   ���ρF500ms�Ԋu(2Hz)
	*   1Hz, 2Hz, 4Hz, 8Hz�̃��Y��(f)
	*   1/1, 1/2, 1/4, 1/8 �̃p���[���H�H
	*   8    4    2    1  �̔䁩���ɂ��Ă����́H�H�H
	*	���v15�Ȃ̂�7�{���Ƃ�
	*	56,	28,	14,	7 �̌v105
	*   8s   2s   1/2s 1/8s -> 10+3/8 s (83/8)????
	*/
	int clicktime = 40;//<125
	int silenceArray[105];
	resetSequence();
	for (int i = 0; i<56; ++i) {
		silenceArray[i] = 1000 - clicktime;
	}
	for (int i = 56; i<84; ++i) {
		silenceArray[i] = 500 - clicktime;
	}
	for (int i = 84; i<98; ++i) {
		silenceArray[i] = 250 - clicktime;
	}
	for (int i = 98; i < 105; ++i) {
		silenceArray[i] = 125 - clicktime;
	}
	//shuffle
	for (int times = 0; times<100; ++times) {
		for (int i = 0; i<105; ++i) {
			int j = rnd() % 105;
			int t = silenceArray[i];
			silenceArray[i] = silenceArray[j];
			silenceArray[j] = t;
		}
	}

	for (int i = 0; i<105; ++i) {
		sequence.push_back(new Click(clicktime));
		sequence.push_back(new Silence(silenceArray[i]));
	}

}

void SoundVector::createRegularRhythmSequence() {
	int clicktime = 40;
	int Hz = 2;
	resetSequence();
	sequence.push_back(new Click(clicktime));
	sequence.push_back(new Silence((1000/Hz)-clicktime));

}

void SoundVector::createRandomRhythmSequence() {
	/*
	*   ���ρF500ms�Ԋu(2Hz)
	*   1Hz, 2Hz, 4Hz, 8Hz�̃��Y��(f)
	*   25�������v100�ɂ��Ƃ�
	*/
	int clicktime = 40;//<125
	int silenceArray[100];
	resetSequence();
	for (int i = 0; i<25; ++i) {
		silenceArray[i] = 1000 - clicktime;
	}
	for (int i = 25; i<50; ++i) {
		silenceArray[i] = 500 - clicktime;
	}
	for (int i = 50; i<75; ++i) {
		silenceArray[i] = 250 - clicktime;
	}
	for (int i = 75; i < 100; ++i) {
		silenceArray[i] = 125 - clicktime;
	}
	//shuffle
	for (int times = 0; times<100; ++times) {
		for (int i = 0; i<100; ++i) {
			int j = rnd() % 100;
			int t = silenceArray[i];
			silenceArray[i] = silenceArray[j];
			silenceArray[j] = t;
		}
	}

	for (int i = 0; i<100; ++i) {
		sequence.push_back(new Click(clicktime));
		sequence.push_back(new Silence(silenceArray[i]));
	}

}

void SoundVector::createTestSequence() {
	resetSequence();
	sequence.push_back(new PureTone(1, 60, 1000));
	sequence.push_back(new PureTone(2, 60, 1000));
	sequence.push_back(new PureTone(1, 60, 1000));
	sequence.push_back(new PureTone(2, 60, 1000));
	sequence.push_back(new PureTone(1, 60, 1000));
	sequence.push_back(new PureTone(2, 60, 1000));
	sequence.push_back(new PureTone(1, 60, 1000));
	sequence.push_back(new PureTone(2, 60, 1000));
	sequence.push_back(new PureTone(1, 60, 1000));
	sequence.push_back(new PureTone(2, 60, 1000));
}

void SoundVector::createPitchSequence(int id = 0) {
	/*
	*   500ms�Ԋu(2Hz)
	*   24(C), 27(D), 30(E),36(G),40(A), 48(C) kHz�̌܉����K
	*   �ŏ����{�� = 1080
	*   45, 40, 36, 30, 27, 22.5 ���v200.5��(400STEP)
	*   23, 20, 18, 15, 13, 11�@���v 100 ��
	*
	*/
	int pitchLength = 400;
	int interval = 100;
	double pitchArray[100];
	resetSequence();

	for (int i = 0; i<23; ++i) {
		pitchArray[i] = 24;
	}
	for (int i = 23; i<43; ++i) {
		pitchArray[i] = 27;
	}
	for (int i = 43; i<61; ++i) {
		pitchArray[i] = 30;
	}
	for (int i = 61; i<76; ++i) {
		pitchArray[i] = 36;
	}
	for (int i = 76; i<89; ++i) {
		pitchArray[i] = 40;
	}
	for (int i = 89; i<100; ++i) {
		pitchArray[i] = 48;
	}

	//shuffle
	for (int times = 0; times<100; ++times) {
		for (int i = 0; i<100; ++i) {
			int j = rnd() % 100;
			double t = pitchArray[i];
			pitchArray[i] = pitchArray[j];
			pitchArray[j] = t;
		}
	}

	for (int i = 0; i<100; ++i) {
		sequence.push_back(new PureTone(id, pitchArray[i], 60, pitchLength));
		sequence.push_back(new Silence(interval));
	}
}

void SoundVector::createRandomPitchSequence(int id = 0) {
	/*
	*   500ms�Ԋu(2Hz)
	*   24(C), 27(D), 30(E),36(G),40(A), 48(C) kHz�̌܉����K
	*   17����102�̉���ɂ���
	*
	*/
	int pitchLength = 400;
	int interval = 100;
	double pitchArray[102];
	resetSequence();

	for (int i = 0; i<17; ++i) {
		pitchArray[i] = 24;
	}
	for (int i = 17; i<34; ++i) {
		pitchArray[i] = 27;
	}
	for (int i = 34; i<51; ++i) {
		pitchArray[i] = 30;
	}
	for (int i = 51; i<68; ++i) {
		pitchArray[i] = 36;
	}
	for (int i = 68; i<85; ++i) {
		pitchArray[i] = 40;
	}
	for (int i = 85; i<102; ++i) {
		pitchArray[i] = 48;
	}

	//shuffle
	for (int times = 0; times<100; ++times) {
		for (int i = 0; i<102; ++i) {
			int j = rnd() % 102;
			double t = pitchArray[i];
			pitchArray[i] = pitchArray[j];
			pitchArray[j] = t;
		}
	}

	for (int i = 0; i<102; ++i) {
		sequence.push_back(new PureTone(id, pitchArray[i], 60, pitchLength));
		sequence.push_back(new Silence(interval));
	}
}
void SoundVector::createRegularPitchSequence(int id = 0) {
	/*
	*   500ms�Ԋu(2Hz)
	*   24(C), 27(D), 30(E),36(G),40(A), 48(C) kHz�̌܉����K
	*   �Ƃ肠�����㏸���K�ɂ���
	*
	*/
	int pitchLength = 400;
	int interval = 100;

	resetSequence();
	sequence.push_back(new PureTone(id, 24, 60, pitchLength));
	sequence.push_back(new Silence(interval));
	sequence.push_back(new PureTone(id, 27, 60, pitchLength));
	sequence.push_back(new Silence(interval));
	sequence.push_back(new PureTone(id, 30, 60, pitchLength));
	sequence.push_back(new Silence(interval));
	sequence.push_back(new PureTone(id, 36, 60, pitchLength));
	sequence.push_back(new Silence(interval));
	sequence.push_back(new PureTone(id, 40, 60, pitchLength));
	sequence.push_back(new Silence(interval));
	sequence.push_back(new PureTone(id, 48, 60, pitchLength));
	sequence.push_back(new Silence(interval));
}
