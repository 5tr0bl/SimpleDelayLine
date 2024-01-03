/*
  ==============================================================================

    DelayProcessor.h
    Created: 10 Oct 2023 10:55:01am
    Author:  Micha

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DelayLineBase.h"

class DelayProcessor {
public:
    DelayProcessor::DelayProcessor();
    DelayProcessor::DelayProcessor(double maxDelayTimeInSeconds, double sampleRate, int interpolationType);
    DelayProcessor::~DelayProcessor();

    void DelayProcessor::setPosition(float newX, float newY, float newZ);
    void DelayProcessor::setPosition(const juce::Vector3D<float>& newPos);
    void DelayProcessor::setDistance(const juce::Vector3D<float>& sourceLocation);
    void DelayProcessor::setFirFilter(double freq, double sampleRate);
    void DelayProcessor::setHRIR(const juce::File& impulseResponse);
    void DelayProcessor::setInterpolationType(int comboBoxChoice, juce::dsp::ProcessSpec& spec); // obsolete
    void setDelayTime(double delayTimeInSeconds, double sampleRate);
    void DelayProcessor::setDelayTimeInSamples(float delayTimeInSamples, double sampleRate);
    void DelayProcessor::setMaxDelayTime(double maxDelayTimeInSeconds, double sampleRate);
    void DelayProcessor::prepare(juce::dsp::ProcessSpec& spec);
    void processBlock(juce::dsp::AudioBlock<float>& block);
    void DelayProcessor::process(juce::dsp::ProcessContextReplacing<float>& context);

    double getDelayTime() const {
        return currentDelayTime;
    }

    int getDelayTimeInSamples(double sampleRate) const {
        return currentDelayInSamples;
    }

    float DelayProcessor::getGainFactor() const
    {
        return gainFactor;
    }

    float DelayProcessor::getDistance() const
    {
        return distance;
    }

    //std::unique_ptr<juce::dsp::ProcessorDuplicator<dsp::FIR::Filter<float>, dsp::FIR::Coefficients<float>>> lowPassFilter;
    juce::dsp::ProcessorDuplicator<dsp::FIR::Filter<float>, dsp::FIR::Coefficients<float>> filter;
    juce::dsp::Convolution convolver;
    bool convolutionEnabled = true;

    juce::Vector3D<float> position;

private:
    double maxDelayTime;
    float currentDelayInSamples;
    double currentDelayTime; // in seconds
    float distance, gainFactor;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLine;
    std::unique_ptr<DelayLineBase> delayLineBase;
};