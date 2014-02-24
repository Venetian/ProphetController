/*
 *  ProphetMidiSettings.h
 *  MophoJucer
 *
 *  Created by Andrew Robertson on 23/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#ifndef PROPHET_MIDI_H
#define PROPHET_MIDI_H

#define USING_NRPN 1 //this should be set on the Global settings of Prophet (to NRPN 1, or Control Change CC 0)

//#include <stdio.h>
#include "../JuceLibraryCode/JuceHeader.h"
#include <vector.h>

#define NOISE_LEVEL_A 14
#define NOISE_LEVEL_B 214
#define FILTER_FREQUENCY_A 15
#define RESONANCE_A 16
#define FILTER_FREQUENCY_B 215
#define RESONANCE_B 216


struct ProphetNRPN{
public:
	String name;
	int nrpn;
	int range;

	ProphetNRPN() : name("none"), nrpn(0), range(127){
	}
	
	ProphetNRPN(String n, int i, int k){
		name = n;
		nrpn = i;
		range = k;
	}
	//, int nrpn = 0, int range = 1);
	//ProphetNRPN(String name = "none", int nrpn = 0, int range = 0);
};


class ProphetMidiSettings{
public:
	ProphetMidiSettings();
	
//	int getNRPN_MSB(int parameterIndex);
//	int getNRPN_LSB(int parameterIndex);
	int getMSB(int val);
	int getLSB(int val);
	void createNRPNlist();
	
	ProphetNRPN myProph;
	std::vector<ProphetNRPN> nrpnList;
};
#endif
