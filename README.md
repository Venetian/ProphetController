ProphetController
=================

JUCE application to control Prophet 08, also useful for sending out MIDI data to other synths, eg Mopho, Moog, etc

The JuceSource directory conatins files required for a JUCE application. Create a JUCE GUI app with Component, then replace the main.cpp, mainComponent.h and .cpp with these. 
Extra files are the ProphetMidiSettings, which effectively puts the map of how the Prophet receives NRPN MIDI values into a useful structure for JUCE menu bars. The struct includes the NRPN, the name of the parameter (as a JUCE::String object) and the range of values. When we choose this parameter in a drop down menu (ComboBox), the values are used to set the slider automatically.