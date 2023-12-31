/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DelayProcessor.h"

const double DEFAULT_CUTOFF = 4000.0;
const float DEFAULT_DELAY_IN_SAMPLES = 0.1f;
const int DEFAULT_INTERPOLATION_INDEX = 2;
const juce::Vector3D DEFAULT_LISTENER_POSITION(0.0f, 5.0f, 0.0f);
const juce::Vector3D DEFAULT_SOURCE_POSITION(0.0f, 0.0f, 0.0f);
const juce::Vector3D DEFAULT_PHANTOM_SOURCE_POSITION(5.0f, 0.0f, 0.0f);
const double MAX_DELAY = 2;
const float MAX_MOVING_SPEED = 30.0f; // in m/s
const float SONIC_SPEED = 343.0f;


//==============================================================================
/**
*/
class SimpleDelayLineAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SimpleDelayLineAudioProcessor();
    ~SimpleDelayLineAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void SimpleDelayLineAudioProcessor::initDelayProcessors(double sampleRate, juce::dsp::ProcessSpec& spec, int interpolationType);
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void SimpleDelayLineAudioProcessor::updateConvolutionState();
    void SimpleDelayLineAudioProcessor::updateDelayTimeRamped(double sampleRate, float numSamples);
    void updateDelayTime(double sampleRate);
    void updateDistance();
    void updateFilter(double sampleRate);
    void SimpleDelayLineAudioProcessor::updateInterpolationType();
    void SimpleDelayLineAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue);

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    AudioProcessorValueTreeState tree;
    juce::dsp::ProcessSpec spec;

    int interpolationType;

    juce::File root, savedFile;
    juce::dsp::Convolution convolver;

    NormalisableRange<float> delayTimeRange = NormalisableRange<float>(0.0f, 22100.0f, 0.1f);

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
    //DelayProcessor directProcessor;
    //DelayProcessor delayProcessor;
    std::unique_ptr<DelayProcessor> directProcessor;
    std::unique_ptr<DelayProcessor> delayProcessor;

    LinearSmoothedValue<float> delaySmoothed;

    juce::File irFileFrontal, irFileLateral;
    bool convolutionEnabled = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayLineAudioProcessor)
};
