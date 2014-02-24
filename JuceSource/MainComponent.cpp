/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//idea would be map controller (any HID?) to a hardware synth
//need to figure out what controller has as inputs
//use hidapi.h for this
//poll the controller
//set mapping to a range on the possible synth NRPN mapping

//we need several range sliders and possible NRPNs

//==============================================================================
MainContentComponent::MainContentComponent() : button1("click it"),
slider1 (Slider::LinearHorizontal, Slider::TextBoxLeft),
slider2 (Slider::ThreeValueHorizontal, Slider::TextBoxLeft)
{
	//Slider::LinearHorizontal

							   
	slider1.setRange(0.0, 128.0, 1.);
	slider1.addListener(this);
	
	button1.addListener(this);

	slider2.setRange(0., 128.0, 1.);
	slider2.setMinAndMaxValues(0., 100.0, sendNotification);
	slider2.addListener(this);
	addAndMakeVisible(&slider2);
	
	addAndMakeVisible(&button1);
	addAndMakeVisible(&slider1);
	addAndMakeVisible(&label1);

	
	//copying technique from Juce demo:
//	addAndMakeVisible (midiOutputListLabel);
//	midiOutputListLabel.setText ("MIDI Output:", dontSendNotification);
//	midiOutputListLabel.attachToComponent (&midiListBox, true);
	
	addAndMakeVisible (midiListBox);
	midiListBox.setTextWhenNoChoicesAvailable ("No MIDI Output Enabled");
	
	const StringArray midiOutputs (MidiOutput::getDevices());
	midiListBox.addItemList (midiOutputs, 1);
	midiListBox.addListener (this);
	
	myMidiDevice = NULL;
	
	addAndMakeVisible(prophetParameterBox);
	prophetParameterBox.setTextWhenNoChoicesAvailable ("No Prophet Parameter chosen");
	
	StringArray prophetList;
	for (int i = 0; i < prophet.nrpnList.size(); i++)
		prophetList.add(prophet.nrpnList[i].name);
		
//	prophetList.add("Noise A");
	prophetParameterBox.addItemList(prophetList, 1);
	prophetParameterBox.addListener(this);
	
	currentParameter = 14;
	lastParameter = 0;
	paramMSB = 0;
	paramLSB = 0;
	paramRange = 1;
	
	//look for controllers and add to new device menu bar
	saitek.init();
	addAndMakeVisible(deviceListBox);
	deviceListBox.setTextWhenNoChoicesAvailable ("No Saitek device found");
	
	StringArray deviceList;
	for (int i = 0; i < saitek.devices.size(); i++){
		juce::String js = "test";//(const char*) saitek.devices[i].productDescription;
		deviceList.add(js);//juce::String(saitek.devices[i].productDescription));//"hello");//saitek.devices[i].productDescription);
	}
	deviceListBox.addItemList(deviceList, 1);
	deviceListBox.addListener(this);
	
//	startSaitek();
	
	setSize (700, 600);
	
	joystickValues.reserve(3);
	for (int i = 0; i < 3; i++)
		joystickValues.push_back(&saitek.axes[i]);
	
	
	startTimer(10);//do this when we choose device though
}

MainContentComponent::~MainContentComponent()
{
//	slider1.removeListener(this);
//	button1.removeListener(this);
	
	midiListBox.removeListener(this);
	//or else we leak from the const array of possible Midi outputs
	
	prophetParameterBox.removeListener(this);
	
	deviceListBox.removeListener(this);
	
	if (myMidiDevice != NULL)
		myMidiDevice->sendMessageNow(MidiMessage(128, 60, 100));
	
	delete myMidiDevice;
	
}

void MainContentComponent::buttonClicked(Button* button){
	if (button == &button1){
		slider1.setValue(2, sendNotification);
	}
	if (myMidiDevice != NULL)
		myMidiDevice->sendMessageNow(MidiMessage(128, 60, 100));
}


void MainContentComponent::sliderValueChanged(Slider* slider){
	if (&slider1 == slider){
		label1.setText(String (slider1.getValue()), sendNotification);
	}
	
	if (&slider2 == slider){
		//do something
	}
	
	int val = (int)slider1.getValue();
//	printf("slider %i\n", val);
	
	if (USING_NRPN){
//		printf("msb noise %i lsb noise %i\n", prophet.getMSB(currentParameter), prophet.getLSB(currentParameter));
		
		sendNRPN(currentParameter, val);

	//	D,Bb,E,lowG
	//	D,E,F higher
	} else {
		MidiMessage m(176, 29, val);
		myMidiDevice->sendMessageNow(m);
	}
	
}

void MainContentComponent::sendNRPN(int parameter, int val){
	if (myMidiDevice != NULL){
		if (parameter != lastParameter){
			if (val <= paramRange){
				printf("Parameter NRPN %i\n", parameter);
				lastParameter = parameter;
				paramMSB = prophet.getMSB(parameter);
				paramLSB = prophet.getLSB(parameter);
				myMidiDevice->sendMessageNow(MidiMessage(176, 99, paramMSB));
				myMidiDevice->sendMessageNow(MidiMessage(176, 98, paramLSB));
			} else {
				printf("%i is Out of range (%i)\n", parameter, paramRange);
			}
		}
		MidiMessage m3(176, 6, prophet.getMSB(val));
		MidiMessage m4(176, 38, prophet.getLSB(val));
		myMidiDevice->sendMessageNow(m3);
		myMidiDevice->sendMessageNow(m4);
	}
}

void MainContentComponent::comboBoxChanged(ComboBox* box) override
{
	if (box == &midiListBox){
		myMidiDevice = MidiOutput::openDevice(midiListBox.getSelectedItemIndex());
		myMidiDevice->sendMessageNow(MidiMessage(144, 60, 100));
	}
	
	if (box == &prophetParameterBox){
		printf("PROPHET SEL %i\n", prophetParameterBox.getSelectedItemIndex());
		if (selectNRPNParameter(prophetParameterBox.getSelectedItemIndex()))
			printf("Could not load NRPN parameter\n");
	}
	
	if (box == &deviceListBox)
	{
		int index =  deviceListBox.getSelectedItemIndex();
		printf("open device %i\n", deviceListBox.getSelectedItemIndex());
		saitek.openDevice(index);
	}
	
	//	if (myMidiDevice != NULL){
//		printf("delete pointer to midi device\n");
//		delete myMidiDevice;
//	}
	
	
}

int MainContentComponent::selectNRPNParameter(int i){
	if (i < prophet.nrpnList.size() && i >= 0){
		currentParameter = prophet.nrpnList[i].nrpn;
		if (prophet.nrpnList[i].range != paramRange){
			paramRange = prophet.nrpnList[i].range;
			slider1.setRange(0.0, paramRange, 1.);
			slider2.setRange(0.0, paramRange, 1.);
			slider2.setMinAndMaxValues(0.0, paramRange, sendNotification);
		}
		return 0;
	} else 
		return 1;
}


void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xffeeddff));

    g.setFont (Font (16.0f));
    g.setColour (Colours::black);
    g.drawText ("Mopho Jucer!", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
	
	button1.setBoundsRelative(0.05, 0.35, 0.2, 0.1);
	slider1.setBoundsRelative(0.4, 0.8, 0.5, 0.1);
	slider2.setBoundsRelative(0.4, 0.9, 0.5, 0.1);

	label1.setBoundsRelative(0.3, 0.8, 0.15, 0.05);
	
	midiListBox.setBoundsRelative(0.1,0.1,0.7,0.1);
	prophetParameterBox.setBoundsRelative(0.1,0.3,0.7,0.1);
	deviceListBox.setBoundsRelative(0.1,0.5,0.7, 0.1);}

void MainContentComponent::timerCallback() override
{
//	printf("timer called\n");
	
	if (saitek.update() == 1){
		slider1.setValue(saitek.axes[0]*128, sendNotification);
		printf("saitek %f , %f\n", *(joystickValues[0]), *(joystickValues[1]));
	}
}


void MainContentComponent::startSaitek(){
	//this could be any controller that is called
	//we just need a way to translate the data into
	//control data for the MIDI synthesizer
	//saitek.init();
	
}