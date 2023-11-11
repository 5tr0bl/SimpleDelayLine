/*
  ==============================================================================

    DelayProcessor.cpp
    Created: 10 Oct 2023 10:54:31am
    Author:  Micha

  ==============================================================================
*/

#include "DelayProcessor.h"

/* sampleRate gets passed ONCE upon calling constructor */

DelayProcessor::DelayProcessor() {
    // Default constructor implementation, if needed.
    maxDelayTime = 4;
    delayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>();

}

DelayProcessor::DelayProcessor(double maxDelayTimeInSeconds, double sampleRate) :
    maxDelayTime(maxDelayTimeInSeconds),
    delayLine(sampleRate * maxDelayTimeInSeconds) 
{
    delayLine.setMaximumDelayInSamples(sampleRate * maxDelayTimeInSeconds);
    *filter.state = *dsp::FilterDesign<float>::designFIRLowpassWindowMethod(1000.0, sampleRate, 21, dsp::WindowingFunction<float>::hamming);
}

DelayProcessor::~DelayProcessor() {

}

void DelayProcessor::setFirFilter(double freq, double sampleRate) {
    *filter.state = *dsp::FilterDesign<float>::designFIRLowpassWindowMethod(freq, sampleRate, 21, dsp::WindowingFunction<float>::hamming);
}

void DelayProcessor::setHRIR(const juce::File& impulseResponse) {
    convolver.reset();

    if (impulseResponse.existsAsFile())
    {
        juce::FileInputStream inputStream(impulseResponse);
        if (inputStream.openedOk())
        {
            convolver.loadImpulseResponse(impulseResponse,
                juce::dsp::Convolution::Stereo::yes,
                juce::dsp::Convolution::Trim::no,
                0, juce::dsp::Convolution::Normalise::no);
        }
        else
        {
            auto b = impulseResponse.getFullPathName();
            // Handle file open error
            // Print an error message, throw an exception, or handle it in your preferred way
        }
    }
    else
    {
        // Handle file not found error
        // Print an error message, throw an exception, or handle it in your preferred way
    }
}

void DelayProcessor::setInterpolationType(int comboBoxChoice, juce::dsp::ProcessSpec& spec) {
    /*
    delete delayLine;
    switch (comboBoxChoice) {
    case 0:
    default:
        delayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>();
        break;
    case 1:
        delayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Thiran>();
        break;
    case 2:
        delayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>();
        break;
    }
    */
}

void DelayProcessor::setDelayTime(double delayTimeInSeconds, double sampleRate) {
    // Ensure the delay time is within the valid range
    double clampedDelayTime = juce::jlimit(0.0, maxDelayTime, delayTimeInSeconds);
    delayLine.setDelay(clampedDelayTime * sampleRate);
    currentDelayTime = clampedDelayTime; // in seconds
}

void DelayProcessor::setDelayTimeInSamples(int delayTimeInSamples, double sampleRate) {
    // double clampedDelayTimeInSamples = juce::jlimit(0.0, maxDelayTime * sampleRate, delayTimeInSamples * sampleRate);
    delayLine.setDelay(delayTimeInSamples);
    currentDelayInSamples = delayTimeInSamples;
}

void DelayProcessor::setMaxDelayTime(double maxDelayTimeInSeconds, double sampleRate) {
    delayLine.setMaximumDelayInSamples(sampleRate * maxDelayTimeInSeconds);
    maxDelayTime = maxDelayTimeInSeconds;
}

void DelayProcessor::prepare(juce::dsp::ProcessSpec& spec) {
    delayLine.prepare(spec);
    setMaxDelayTime(maxDelayTime, spec.sampleRate);
    filter.reset();
    filter.prepare(spec);
    convolver.reset();
    convolver.prepare(spec);
}

void DelayProcessor::processBlock(juce::dsp::AudioBlock<float>& block) {
    // Process the block of audio samples using the delay line
    delayLine.process(juce::dsp::ProcessContextReplacing<float>(block));
    //convolver.process(block);
}

void DelayProcessor::process(juce::dsp::ProcessContextReplacing<float>& context) {
    // Process the block of audio samples using the delay line
    delayLine.process(context);
    filter.process(context);
    convolver.process(context);
}
