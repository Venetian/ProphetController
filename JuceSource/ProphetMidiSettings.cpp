/*
 *  ProphetMidiSettings.cpp
 *  MophoJucer
 *
 *  Created by Andrew on 23/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#include "ProphetMidiSettings.h"
#include <iostream.h>

ProphetMidiSettings::ProphetMidiSettings(){
	printf("Prophet initialised\n");
	printf("356: msb %i lsb %i\n", getMSB(356), getLSB(356));
	
	createNRPNlist();
	
	printf("Prophet name is %i\n", myProph.nrpn);
//	ProphetNRPN proph1 = {0, 65, 127};
//	printf("Proph1 has NRPN_A %i, NRPN_B %i, range %i\n", proph1.NRPN_A, proph1.NRPN_B, proph1.range);
}

//populate our vector with the right settings for the Prophet 08
void ProphetMidiSettings::createNRPNlist(){
//	String tmp = "Noisey";
//	ProphetNRPN test1 = ProphetNRPN();
//	test1.name = tmp;
	//, NOISE_LEVEL_A,127);
//	nrpnList.push_back(test1);
	
	nrpnList.push_back(ProphetNRPN((String)"Oscillator 1 Shape", 2, 103));
	nrpnList.push_back(ProphetNRPN((String)"Oscillator 1 Glide", 3, 127));
	nrpnList.push_back(ProphetNRPN((String)"Oscillator 2 Shape", 7, 103));
	nrpnList.push_back(ProphetNRPN((String)"Oscillator 2 Shape", 8, 127));
	
	
	nrpnList.push_back(ProphetNRPN((String)"Noise Level A", 14, 127));
	nrpnList.push_back(ProphetNRPN((String)"Frequency A", 15, 164));
	nrpnList.push_back(ProphetNRPN((String)"Resonance", 16, 127));
	nrpnList.push_back(ProphetNRPN((String)"Filter Key Amount A", 17, 127));
	nrpnList.push_back(ProphetNRPN((String)"Filter Audio Modulation A", 18, 127));
	nrpnList.push_back(ProphetNRPN((String)"Filter Poles (0 two, 1 four)", 19, 1));
	nrpnList.push_back(ProphetNRPN((String)"Filter Env Amount", 20, 254));
	nrpnList.push_back(ProphetNRPN((String)"Filter Env Velocity", 21, 127));
	nrpnList.push_back(ProphetNRPN((String)"Filter Env Delay", 22, 127));
	nrpnList.push_back(ProphetNRPN((String)"Filter Env Attack", 23, 127));
	nrpnList.push_back(ProphetNRPN((String)"Filter Env Decay", 24, 127));
	nrpnList.push_back(ProphetNRPN((String)"Filter Env Sustain", 25, 127));
	nrpnList.push_back(ProphetNRPN((String)"Filter Env Release", 26, 127));
	
	nrpnList.push_back(ProphetNRPN((String)"LFO 1 Shape A", 38, 4));
	nrpnList.push_back(ProphetNRPN((String)"LFO 1 Amount A", 39, 43));

}


int ProphetMidiSettings::getMSB(int val){
	return (val - (val % 128))/128;
}
int ProphetMidiSettings::getLSB(int val){
	return val%128;
}


/*
int ProphetMidiSettings::getNRPN_MSB(int parameterIndex){
	
	switch (parameterIndex) {
		case NOISE_LEVEL_A:
			return 0;
			break;
		default:
			printf("parameter not found yet\n");
			break;
	}
	return -1;
}
int ProphetMidiSettings::getNRPN_LSB(int parameterIndex){
	switch (parameterIndex) {
		case NOISE_LEVEL_A:
			return NOISE_LEVEL_A%128;
			break;
		default:
			printf("parameter not found yet");
			break;
	}
	return -1;
}
*/
