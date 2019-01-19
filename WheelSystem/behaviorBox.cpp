#include "behaviorBox.h"


BehaviorBox::BehaviorBox(){
    n_pin = 64;
    deviceOpen();
}

BehaviorBox::~BehaviorBox(){
    deviceClose();
}

int BehaviorBox::deviceOpen(){
    DeviceHandle = DioOpen("FBIDIO1", FBIDIO_FLAG_SHARE);
    if(DeviceHandle == INVALID_HANDLE_VALUE){
        return -1;
    }
    return 0;
}

int BehaviorBox::deviceClose(){
    INT ret = DioClose(DeviceHandle); 
    if(ret!=FBIDIO_ERROR_SUCCESS){
        return -1;//error
    }
    return 0;
}

int BehaviorBox::read(int pin){
    if(pin<1 || pin>n_pin){
        return -1;
    }
    INT value;
    INT ret = DioInputPoint(DeviceHandle, &value, pin, 1);
    if(ret != FBIDIO_ERROR_SUCCESS){
        return -1;
    }
    return value;
}

int BehaviorBox::write(int pin, int value){
    if(pin<1 || pin>n_pin){
        return -1;
    }
    INT val = (value>0)? 1 : 0;
    INT ret = DioOutputPoint(DeviceHandle, &val, pin, 1);
    if(ret != FBIDIO_ERROR_SUCCESS){
        return -1;
    }
    return 0;
}

