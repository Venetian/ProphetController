/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "ProphetMidiSettings.h"

#include "SaitekX52controller.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

//below the mainContentComponent extends all these base classes
//Timer has to be private as it is protected 


struct ProphetSlider {
	Slider* prophetSlider;
	ComboBox* parameterBox;
	Label* textDescription;//for what part of joystick is mapped to it
	int lastSentValue;
	float relativeX, relativeY, relativeW, relativeH;
	int nrpn;//the NRPN value assigned to this parameter in Prophet
	int parameterRange;//gets set for each change in NRPN parameter we might send - eg Noise 127, Filter Freq 164
	int outputOn;
};

class MainContentComponent   : private Timer, public Component,
public Button::Listener, public Slider::Listener,
public ComboBox::Listener//extends/implements Listener for listbox?
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

	void setUpSliders();
	
    void paint (Graphics&);
    void resized();
	
	void buttonClicked(Button* button);
	void sliderValueChanged(Slider* slider);
	void comboBoxChanged(ComboBox* box);//virtual fn needs to be overwritten
	void setSliderToParameterIndex(int& i, int& index);
	int checkNotUsedAlready(int& i, int& index);
	
	void timerCallback();//virtual void timer fn so needs writing
	
	void sendNRPN(int parameter, int val);
	int selectNRPNParameter(int i);
	
	int getRangeValueInt(Slider& slider, float& value);
	int getRangeValueInt(Slider* slider, float& value);
	void startSaitek();
	
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
	
	TextButton button1;
	Slider	slider1;
	Label label1;
	
	Slider slider2;
	
	ComboBox midiListBox;//list of Midi outputs
	ComboBox prophetParameterBox;
	
	ComboBox deviceListBox;//list of HID devices that can be connected

	//TO DO
	ComboBox synthesizerBox;//list of possible synths to connect to
	//each will need a class file
	
	MidiOutput* myMidiDevice;
	
	int currentParameter, paramMSB, paramLSB, lastParameter, paramRange;
	
	ProphetMidiSettings prophetSettings;
	
	SaitekX52controller saitek;
	
	std::vector<float*> joystickValues;
	
	std::vector<ProphetSlider> prophetControlSliders;
	StringArray prophetList;
};


#endif  // MAINCOMPONENT_H_INCLUDED
