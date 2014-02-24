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

class MainContentComponent   : private Timer, public Component,
public Button::Listener, public Slider::Listener,
public ComboBox::Listener//extends/implements Listener for listbox?
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
	
	void buttonClicked(Button* button);
	void sliderValueChanged(Slider* slider);
	void comboBoxChanged(ComboBox* box);//virtual fn needs to be overwritten

	void timerCallback();//virtual void timer fn so needs writing
	
	void sendNRPN(int parameter, int val);
	int selectNRPNParameter(int i);
	
	void startSaitek();
	
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
	
	TextButton button1;
	Slider	slider1;
	Label label1;
	
	Slider slider2;
	
	ComboBox midiListBox;//decalre list
	ComboBox prophetParameterBox;
	
	ComboBox deviceListBox;
	
	MidiOutput* myMidiDevice;
	
	int currentParameter, paramMSB, paramLSB, lastParameter, paramRange;
	
	ProphetMidiSettings prophet;
	
	SaitekX52controller saitek;
	
	std::vector<float*> joystickValues;
};


#endif  // MAINCOMPONENT_H_INCLUDED
