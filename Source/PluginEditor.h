/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include  "StyleSheet.h"
//==============================================================================
/**
*/
class RomalNoiserAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RomalNoiserAudioProcessorEditor (RomalNoiserAudioProcessor&);
    ~RomalNoiserAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.




    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdSliderAttachment;

    CustomSlider gainSlider;
    CustomSlider frequencySlider;
    CustomSlider thresholdSlider;

    CustomLNF myCustomLNF;
    RomalNoiserAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RomalNoiserAudioProcessorEditor)
};
