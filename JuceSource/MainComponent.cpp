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
slider1 (Slider::ThreeValueHorizontal, Slider::TextBoxLeft),
slider2 (Slider::ThreeValueHorizontal, Slider::TextBoxLeft)
{
	//Slider::LinearHorizontal

							   
	
	
	button1.addListener(this);
	addAndMakeVisible(&button1);
	
	
	slider1.setRange(0.0, 128.0, 1.);
	slider1.setMinAndMaxValues(0., 100.0, sendNotification);
	slider1.addListener(this);
	addAndMakeVisible(&slider1);
	
	slider2.setRange(0., 128.0, 1.);
	slider2.setMinAndMaxValues(0., 100.0, sendNotification);
	slider2.addListener(this);
	addAndMakeVisible(&slider2);
	
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
	
	prophetList.clear();
	for (int i = 0; i < prophetSettings.nrpnList.size(); i++)
		prophetList.add(prophetSettings.nrpnList[i].name);
		
//	prophetList.add("Noise A");
	prophetParameterBox.addItemList(prophetList, 1);
	prophetParameterBox.addListener(this);
	
	currentParameter = 14;
	lastParameter = 0;
	paramMSB = 0;
	paramLSB = 0;
	paramRange = 127;
	
	//look for controllers and add to new device menu bar
	saitek.init();
	
	deviceListBox.setTextWhenNoChoicesAvailable ("No Saitek device found");
	StringArray deviceList;
	for (int i = 0; i < saitek.devices.size(); i++){
		juce::String js(saitek.devices[i].productDescription);
		deviceList.add(js);//juce::String(saitek.devices[i].productDescription));//"hello");//saitek.devices[i].productDescription);
	}
	deviceListBox.addItemList(deviceList, 1);
	deviceListBox.addListener(this);
	addAndMakeVisible(deviceListBox);
	
//	startSaitek();
	//make sure getting the prophet parameter list first
	setUpSliders();
	
	setSize (900, 900);
	
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
	
	for (int i = 0; i < prophetControlSliders.size();i++){
		prophetControlSliders[i].prophetSlider->removeListener(this);
		delete prophetControlSliders[i].prophetSlider;
		
		prophetControlSliders[i].parameterBox->removeListener(this);
		delete prophetControlSliders[i].parameterBox;
	}
	
	deviceListBox.removeListener(this);
	
	if (myMidiDevice != NULL)
		myMidiDevice->sendMessageNow(MidiMessage(128, 60, 100));
	
	delete myMidiDevice;
	
}

#pragma mark CreatingVectorOfSliders
void MainContentComponent::setUpSliders(){
	for (int i =0; i < 5; i++){
		ProphetSlider newSlider;
		
		newSlider.prophetSlider = new Slider(Slider::ThreeValueHorizontal, Slider::TextBoxLeft);
		newSlider.prophetSlider->setRange(0., 128.0, 1.);
		newSlider.prophetSlider->setMinAndMaxValues(0., 100.0, sendNotification);
		newSlider.prophetSlider->addListener(this);
		newSlider.relativeX = 0.5;
		newSlider.relativeY = 0.5+0.05*i;
		newSlider.relativeW = 0.5;
		newSlider.relativeH = 0.05;
		
		newSlider.parameterBox = new ComboBox();
		newSlider.parameterBox->setTextWhenNoChoicesAvailable ("not yet set");
		newSlider.parameterBox->addListener(this);
		newSlider.parameterBox->addItemList(prophetList, 1);//offset
		addAndMakeVisible(newSlider.prophetSlider);
		addAndMakeVisible(newSlider.parameterBox);
		newSlider.nrpn = 0;//14+i;//for now
		newSlider.outputOn = 1;
		
	//	if (i % 2)
	//		setColour(newSlider.parameterBox->backgroundColourId, Colours::red);
		
		prophetControlSliders.push_back(newSlider);
	}
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
	
	for (int i = 0; i < prophetControlSliders.size(); i++)
	{
		if (slider == prophetControlSliders[i].prophetSlider)
		{
			if (USING_NRPN && prophetControlSliders[i].nrpn){
				sendNRPN(prophetControlSliders[i].nrpn, prophetControlSliders[i].prophetSlider->getValue());
				printf("change, param %i\n", prophetControlSliders[i].nrpn);
			}
		}
	}
	
	int val = (int)slider1.getValue();
//	printf("slider %i\n", val);
	
	if (USING_NRPN)
	{		
		//sendNRPN(currentParameter, val);
		//e.g. sendNRPN(NOISE_LEVEL_A, val);
		
	//	song idea : D,Bb,E,lowG
	//	D,E,F higher
	} else {
		//CC method
		MidiMessage m(176, 29, val);
		myMidiDevice->sendMessageNow(m);
	}
	
}

//could it get a mapping back from Prophet class?
//i.e. for parameter and value that have been set by Joystick
//the synth class just sends back a yes and formats the four messages?
//after all, it's the synth that has the particular mapping
//so new synth just requires a remapping in its own new class
//eg a Moog class
void MainContentComponent::sendNRPN(int parameter, int val){
	if (myMidiDevice != NULL){
		//if synth.checkParameter(parameter, value)
			//then send messages
		
		if (parameter != lastParameter){
			if (val <= paramRange){
				printf("Parameter NRPN %i\n", parameter);
				lastParameter = parameter;
				paramMSB = prophetSettings.getMSB(parameter);
				paramLSB = prophetSettings.getLSB(parameter);
				myMidiDevice->sendMessageNow(MidiMessage(176, 99, paramMSB));
				myMidiDevice->sendMessageNow(MidiMessage(176, 98, paramLSB));
			} else {
				printf("NRPN %i, value %i is Out of range (%i)\n", parameter, val, paramRange);
			}
		}
		MidiMessage m3(176, 6, prophetSettings.getMSB(val));
		MidiMessage m4(176, 38, prophetSettings.getLSB(val));
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
	
	for (int i = 0; i < prophetControlSliders.size(); i++)
	{
		if (box == prophetControlSliders[i].parameterBox){

			//TO DO - get the range and set that in combo box
			int index = prophetControlSliders[i].parameterBox->getSelectedItemIndex();
			setSliderToParameterIndex(i, index);
		}
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

void MainContentComponent::setSliderToParameterIndex(int& i, int& index)
{	//slider has index i in the prophetSlider vector
	//the parameter has is index in the vector of ProphetSettings (i.e. possib;le parameters)
	if (i < prophetControlSliders.size() && 
		index >= 0 && index < prophetSettings.nrpnList.size())
	{//then we change to new parameter
		if (checkNotUsedAlready(i, index))
		{
			prophetControlSliders[i].prophetSlider->setRange(0.0, prophetSettings.nrpnList[index].range, 1.);
			prophetControlSliders[i].prophetSlider->setMinAndMaxValues(0.0,prophetSettings.nrpnList[index].range, sendNotification);
			prophetControlSliders[i].nrpn = prophetSettings.nrpnList[index].nrpn;
			printf("Box %i has selected %i\n", i, index);//, prophetSettings.nrpnList[index].name);
		} else {
			//int noneChosen = 0;
			//setSliderToParameterIndex(i, noneChosen);
			prophetControlSliders[i].parameterBox->setSelectedItemIndex(0);
		}
	}
}

int MainContentComponent::checkNotUsedAlready(int& i, int& index){
	int notUsed = 1;
	int possibleParameterSetting = prophetSettings.nrpnList[index].nrpn;
	for (int k = 0; k < prophetControlSliders.size(); k++)
	{
		if(possibleParameterSetting && k != i && prophetControlSliders[k].nrpn == possibleParameterSetting){
			notUsed = 0;
			printf("Parameter NRPN %i already being used\n", possibleParameterSetting);
		}
	}
	return notUsed;
}


int MainContentComponent::selectNRPNParameter(int i){
	if (i < prophetSettings.nrpnList.size() && i >= 0){
		currentParameter = prophetSettings.nrpnList[i].nrpn;
		if (prophetSettings.nrpnList[i].range != paramRange){
			paramRange = prophetSettings.nrpnList[i].range;
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
//    g.fillAll (Colour (0xffeeddff));
	g.fillAll(Colours::sienna);
    g.setFont (Font (16.0f));
    g.setColour (Colours::black);
    //g.drawText ("Mopho Jucer!", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
	
	button1.setBoundsRelative(0.05, 0.35, 0.05, 0.02);
	
	slider1.setBoundsRelative(0.4, 0.8, 0.5, 0.1);
	slider2.setBoundsRelative(0.4, 0.9, 0.5, 0.1);

	label1.setBoundsRelative(0.3, 0.8, 0.15, 0.05);
	
	midiListBox.setBoundsRelative(0.1,0.05,0.7,0.05);
	prophetParameterBox.setBoundsRelative(0.1,0.15,0.7,0.05);
	deviceListBox.setBoundsRelative(0.1,0.25,0.7, 0.05);

	for (int i = 0; i < prophetControlSliders.size();i++)
	{
		prophetControlSliders[i].prophetSlider->setBoundsRelative(prophetControlSliders[i].relativeX,
															  prophetControlSliders[i].relativeY,
															  prophetControlSliders[i].relativeW,
															  prophetControlSliders[i].relativeH
															  );
	
		prophetControlSliders[i].parameterBox->setBoundsRelative(0.05,
															  prophetControlSliders[i].relativeY,
															  0.38,
															  0.04
															  );
	}	
}

void MainContentComponent::timerCallback() override
{
//	printf("timer called\n");


	saitek.update();
	//printf("saitek %f , %f\n", *(joystickValues[0]), *(joystickValues[1]));
	/*
	if (saitek.update() == 1)
	{
		int tmp = getRangeValueInt(slider1, *joystickValues[0]);
		slider1.setValue(tmp , sendNotification);
		slider2.setValue(getRangeValueInt(slider2, *joystickValues[1]), sendNotification);
	}
	 */
	
	for (int i = 0; i < prophetControlSliders.size(); i++)
	{
		if (prophetControlSliders[i].outputOn && saitek.axesChanged[i]){
			prophetControlSliders[i].prophetSlider->setValue(getRangeValueInt(prophetControlSliders[i].prophetSlider, saitek.axes[i]), sendNotification);
		}
	}
}

int MainContentComponent::getRangeValueInt(Slider& slider, float& value){
//from val between 0 and 1, return the slider val
	return round(slider.getMinValue() + (slider.getMaxValue()-slider.getMinValue())*value);
}

int MainContentComponent::getRangeValueInt(Slider* slider, float& value){
	//from val between 0 and 1, return the slider val
	return round(slider->getMinValue() + (slider->getMaxValue()-slider->getMinValue())*value);
}

void MainContentComponent::startSaitek(){
	//this could be any controller that is called
	//we just need a way to translate the data into
	//control data for the MIDI synthesizer
	//saitek.init();
	
}