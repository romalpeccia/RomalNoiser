/*
  ==============================================================================

    StyleSheet.h
    Created: 5 Nov 2021 3:12:14pm
    Author:  Romal

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/



class CustomLNF : public juce::LookAndFeel_V4 
{

public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sldierPosProportional, float roytaryStartAngle, float rotaryEndAngle, juce::Slider& slider);


};

class CustomSlider : public juce::Slider 
{
public:

    CustomSlider();
private: 



};