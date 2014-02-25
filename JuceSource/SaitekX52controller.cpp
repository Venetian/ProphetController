/*
 *  SaitekX52controller.cpp
 *  ProphetController
 *
 *  Created by Andrew Robertson on 24/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#include "SaitekX52controller.h"

SaitekX52controller::SaitekX52controller(){
	
	for (int i = 0; i < AXES_LENGTH; i++)
		axesChanged[i] = 0;
	
	for (int i = 0; i < BUTTONS_LENGTH; i++)
		buttonsChanged[i] = 0;
}

SaitekX52controller::~SaitekX52controller()
{
	
	hid_close(handle);
	hid_exit();
}

void SaitekX52controller::init()
{
	
	// Initialize the hidapi library
	res = hid_init();
	
	devices.clear();
	
	// Enumerate and print the HID devices on the system
	struct hid_device_info *devs, *cur_dev;
	
	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;	
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls",
			   cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		std::wcout << L"product is '" << cur_dev->product_string << L"'"<< std::endl;
		
		
		HIDdevice newDev;
		newDev.vendorID = cur_dev->vendor_id;
		newDev.productID = cur_dev->product_id;
		newDev.productDescription = cur_dev->product_string;
		wchar_t* newMem = new wchar_t[100];
		newDev.productDescription = newMem;
		wcscpy(newDev.productDescription, cur_dev->product_string);
		std::wcout << L"copied product is '" << newDev.productDescription << L"'"<< std::endl;
		std::cout << "memory is " << &newDev.productDescription << std::endl;
		
		printf("\n");
		devices.push_back(newDev);
		
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
	
	
	// Open the device using the VID, PID,
	// and optionally the Serial number.
	//in this case, this has been changed to the Saitek joystick owned
	
	//need to generalise this for any HID device
	//and store the various possibilities
	
	if (handle = hid_open(0x6a3, 0x75c, NULL))
		printf("opened haitek\n");
	else
		printf("didnt open\n");
	
	//printf("anr doesnt exist but would like it to: Report length %ls\n", handle->usage);
	
	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);
	
	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);
	
	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);
	
	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);
	
	for (int i = 0; i < 65; i++)
		store_buf[i] = 0;
	
}

int SaitekX52controller::openDevice(int deviceIndex){
	
	printf("open device index %i out of %i\n", deviceIndex, (int)devices.size());
	
	const wchar_t* testw = L"yessir";//for testing
	if (deviceIndex >= 0 && deviceIndex < devices.size())
	{
		std::wcout << L"cout gives " << testw << std::endl;//devices[deviceIndex].productDescription << std::endl; 
		std::wcout << L"cout stored gives " << devices[deviceIndex].productDescription << std::endl;// << std::endl; 
		
		std::wcout << L"copied product is '" << devices[deviceIndex].productDescription << L"'"<< std::endl;
		//hid_close(handle);
//		wprintf(L"device name '%s'\n", testw);//devices[deviceIndex].productDescription);
//		wprintf(L"would open %c\n", testw);//devices[deviceIndex].productDescription);
		//		if (handle = hid_open(devices[deviceIndex].vendorID, devices[deviceIndex].productID, NULL))
//			printf("opened haitek\n");
//		else
//			printf("didnt open\n");	
	}
	return 1;
}


void SaitekX52controller::oldUpdate()
{
	res = hid_read_timeout(handle, buf, 65, 12);
	
	// Print out the returned buffer.
	
	//	[1] between 0 and 255 (2 to the 8)
	//	[2] between 0 and 32 (2 to the 5)
	//	2 to the 10 1024
	
	int i = 0;
	while (++i < 64){
		if (store_buf[i] != buf[i]){
			//		printf("buf[%d]: %d\n", i, buf[i]);
			store_buf[i] = buf[i];
			
			switch (i) {
				case 1:
			//		slider1.setValue((float)buf[2]/2.0, dontSendNotification);
					break;
				default:
					break;
			}
			
		}
	}

}



int SaitekX52controller::update(){
	//	int read(sensor_msgs::Joy* const msg)
	//originally a class of the ros framework was used here
	//could be interesting to look at some point?
	//	{
	//		unsigned char buf[256];
	
	//sensor_msgs::Joy* const msg
	int returnVal = 2;
	
	int res = hid_read_timeout(handle,buf,sizeof(buf),10);
	if (res == 0) {
		return 0; // TimeOut
	}
	if (res < 0) {
		return -1; // Error;
	}
	
	// x,y,z 10bit (0,1024) Scaled to (-1,1)
	double aux;
	
	//	aux = (double)(((buf[1]&0x3)<<8)+buf[0]);
	//	aux = (double)(((buf[1]&0x3)<<8)+buf[0]);
	//last two values of second char shifted up 8 places
	//plus the buf[0];
	
	//	aux=(double)((buf[1]&0x3)<<8);
	
	//	aux=(double)((buf[1]&0x3<<6));//+buf[0];
	aux=(double)((buf[1]&0x7)<<7)+(buf[0]>>1);	
	aux /= 1024.0;
	checkChanged(aux, 0);
	/*
	 if (aux != axes[0])
	{
		axes[0] = aux;
		axesChanged[0] = 1;
		returnVal = 1;
	} else
		axesChanged[0] = 0;
*/
	
	//xValue = aux/1024.0;
	//	if (aux <= 512) axes[0] = (aux - 512) / 512;
	//	else axes[0] = (aux - 511) / 512;
	
	//original line: aux = (double)(((buf[2]&0xF)<<8) + buf[1]>>2);
	aux = (double)((((buf[2]&0x3F)<<4))) + (buf[1]>>4);//not 6?
	aux /= 1024.0;
	checkChanged(aux, 1);
	
	
	//	yValue = aux/1024.0;
	//		if (aux <= 512) axes[1] = (aux - 512) / 512;
	//		else axes[1] = (aux - 511) / 512;
	
	aux = (double)(((buf[3])<<8) + buf[2]>>6);
	aux /= 1024.0;
	checkChanged(aux, 2);
	
	//	if (aux <= 512) axes[2] = (aux - 512) / 512;
	//	else axes[2] = (aux - 511) / 512;
	
	// (0,255) Scaled to (0,1)
	aux = (255 - (double)buf[4]) / 255.0;
	checkChanged(aux, 3);
	
//	axes[4] = (255 - (double)buf[5]) / 255.0; // xRot
	aux = (255 - (double)buf[5]) / 255.0;
	checkChanged(aux, 4);
	
	//axes[5] = (255 - (double)buf[6]) / 255.0; // yRot
	aux = (255 - (double)buf[6]) / 255.0;
	checkChanged(aux, 5);
	
	
//	axes[6] = (255 - (double)buf[7]) / 255.0; // Slider
	aux = (255 - (double)buf[7]) / 255.0;
	checkChanged(aux, 6);
	
	// | 0=Trigger L1 | 1=Fire | 2=A | 3=B | 4=C | 5=Pinkie | D=6 | E=7
	// | 8=T1 | 9=T2 | 10=T3 | 11=T4 | 12=T5 | 13=T6 | 14=Trigger L2 | 15=LeftMouse
	int i,j;
	for (i=8;i<10;i++) {
		for (j=0;j<8;j++) {
			buttons[(i-8)*8 + j] = (bool)(buf[i] & (1 << j));
		}
	}
	//msg->buttons replaced with my own array
	
	// 16:Scroll
	if (buf[10] & (1 << 0)) buttons[16] = -1;
	else if (buf[10] & (1 << 1)) buttons[16] = 1;
	else buttons[16] = 0;
	
	return 1;//returnVal; - used in mainComponent to see if there is value change
}

int SaitekX52controller::checkChanged(double& aux, int index){
	if (aux != axes[index])
	{
		axes[index] = aux;
		axesChanged[index] = 1;
		return 1;
	} else{
		axesChanged[index] = 0;
		return 0;
	}
}
