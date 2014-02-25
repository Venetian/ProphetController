/*
 *  SaitekX52controller.h
 *  ProphetController
 *
 *  Created by Andrew Robertson on 24/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#ifndef SAITEK_X52_CONTROLLER_H
#define SAITEK_X52_CONTROLLER_H

#include "hidapi.h"
#include <vector.h>

#define MAX_STR 255
#define BUTTONS_LENGTH 40
#define AXES_LENGTH 40

//when JUCE loads this we go through and find the possible devices

struct  HIDdevice{
	unsigned short productID;
	unsigned short vendorID;
	wchar_t* productDescription;
};

class SaitekX52controller{
public:
	 SaitekX52controller();
	~SaitekX52controller();
	
	void init();
	int update();
	
	void oldUpdate();
	
	int openDevice(int deviceIndex);
	int checkChanged(double& aux, int index);
	
	//vars
	//hid vars
	int res;
	unsigned char buf[65];
	unsigned char store_buf[65];

	
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	
	float axes[AXES_LENGTH];
	int axesChanged[AXES_LENGTH];
	
	bool buttons[BUTTONS_LENGTH];//for now
	int buttonsChanged[BUTTONS_LENGTH];
	
	std::vector<HIDdevice> devices;
};
#endif