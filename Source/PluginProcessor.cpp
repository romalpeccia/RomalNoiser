/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RomalNoiserAudioProcessor::RomalNoiserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters()) //TODO what does this do?
#endif 
{
}

RomalNoiserAudioProcessor::~RomalNoiserAudioProcessor()
{
}

//==============================================================================
const juce::String RomalNoiserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RomalNoiserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RomalNoiserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RomalNoiserAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RomalNoiserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RomalNoiserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RomalNoiserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RomalNoiserAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RomalNoiserAudioProcessor::getProgramName (int index)
{
    return {};
}

void RomalNoiserAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RomalNoiserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}




void RomalNoiserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RomalNoiserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RomalNoiserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    //for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
   //     buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    auto numSamples = buffer.getNumSamples();
    float gain = apvts.getRawParameterValue("GAIN")->load(); //load gets the value from the std::atomic float ptr
    int frequency = apvts.getRawParameterValue("FREQUENCY")->load()  ;
    float threshold =  apvts.getRawParameterValue("THRESHOLD")->load();
    for (int channel = 0; channel < totalNumInputChannels; ++channel) //iterate over channels
    {
        auto* channelData = buffer.getWritePointer (channel); 
        for (int sample = 0; sample < numSamples; ++sample)
        {
            //generate noise
            float noise_bit = 0;
                if (abs(channelData[sample]) >= threshold){
                    //random float from 0-1 * user gain float from 0-1 * (-1 or 1 depending on sample) * 0.5 to prevent clipping
                    noise_bit= 0.5*(randomGenerator.nextFloat() * gain* (abs(channelData[sample]) / channelData[sample]) ) ;
                    // add the channelData and the 
                    noise_bit = 0.5*channelData[sample] + noise_bit;
                }
            if (frequency != 0) {//variable frequency of noise
                if (sample  % frequency == 0) {
                    // add noise to input signal
                    channelData[sample] = noise_bit;
                }
                else {
                    ;
                  //channelData stays the same?
                }
            }
            else { //full array of white noise
                    channelData[sample] = noise_bit;
            }
        }
    }
}

//==============================================================================
bool RomalNoiserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RomalNoiserAudioProcessor::createEditor()
{
    return new RomalNoiserAudioProcessorEditor (*this);
}

//==============================================================================
void RomalNoiserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RomalNoiserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RomalNoiserAudioProcessor();
}




juce::AudioProcessorValueTreeState::ParameterLayout RomalNoiserAudioProcessor::createParameters() {
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("FREQUENCY", "Frequency", 0, 441, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("THRESHOLD", "Threshold", 0.0f, 1.0f, 0.0f));
    return { params.begin(), params.end() };
}

