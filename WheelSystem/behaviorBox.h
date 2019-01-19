#ifndef BEHAVIORBOX_H
#define BEHAVIORBOX_H
#include "common.h"

class BehaviorBox{
    public:
    BehaviorBox();
    ~BehaviorBox();
    HANDLE DeviceHandle;
	private:
    int n_pin;
    int deviceOpen();
    int deviceClose();
    public:
    int read(int pin);
    int write(int pin, int value);
};
#endif//BEHAVIORBOX_H