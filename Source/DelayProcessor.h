/*
  ==============================================================================

    DelayProcessor.h
    Created: 10 Oct 2023 10:55:01am
    Author:  Micha

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DelayProcessor {
public:
    DelayProcessor::DelayProcessor();
    DelayProcessor::DelayProcessor(double maxDelayTimeInSeconds, double sampleRate);
    DelayProcessor::~DelayProcessor();

    void DelayProcessor::setFirFilter(double freq, double sampleRate);
    void DelayProcessor::setHRIR(const juce::File& impulseResponse);
    void DelayProcessor::setInterpolationType(int comboBoxChoice, juce::dsp::ProcessSpec& spec);
    void setDelayTime(double delayTimeInSeconds, double sampleRate);
    void DelayProcessor::setDelayTimeInSamples(int delayTimeInSamples, double sampleRate);
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

    void DelayProcessor::setMix(float newValue)
    {
        mix = newValue;
    }

    float DelayProcessor::getMix() const
    {
        return mix;
    }

    void DelayProcessor::setDistance(float newValue, const float sonicSpeed, double sampleRate)
    {
        distance = newValue;
        setDelayTime(static_cast<double>(distance/sonicSpeed), sampleRate);
    }

    float DelayProcessor::getDistance() const
    {
        return distance;
    }

    //std::unique_ptr<juce::dsp::ProcessorDuplicator<dsp::FIR::Filter<float>, dsp::FIR::Coefficients<float>>> lowPassFilter;
    juce::dsp::ProcessorDuplicator<dsp::FIR::Filter<float>, dsp::FIR::Coefficients<float>> filter;
    juce::dsp::Convolution convolver;

private:
    double maxDelayTime;
    double currentDelayInSamples;
    double currentDelayTime; // in seconds
    float distance, mix;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLine;
};