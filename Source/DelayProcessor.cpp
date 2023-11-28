/*
  ==============================================================================

    DelayProcessor.cpp
    Created: 10 Oct 2023 10:54:31am
    Author:  Micha

  ==============================================================================
*/

#include "DelayProcessor.h"
#include "DelayLineBase.h"


/* sampleRate gets passed ONCE upon calling constructor */

DelayProcessor::DelayProcessor() {
    maxDelayTime = 2;
    delayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>();
    delayLineBase = std::make_unique<Lagrange3rdDelayLine>();
}

DelayProcessor::DelayProcessor(double maxDelayTimeInSeconds, double sampleRate, int interpolationType) :
    maxDelayTime(maxDelayTimeInSeconds)
    //delayLine(sampleRate * maxDelayTimeInSeconds) 
{
    if (interpolationType == 1)
        delayLineBase = std::make_unique<ThiranDelayLine>();
    else if (interpolationType == 2) {
        //delayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>();
        delayLineBase = std::make_unique<Lagrange3rdDelayLine>();
    }
    else
        delayLineBase = std::make_unique<LinearDelayLine>();
    
    setMaxDelayTime(maxDelayTimeInSeconds, sampleRate);
    *filter.state = *dsp::FilterDesign<float>::designFIRLowpassWindowMethod(20000.0, sampleRate, 21, dsp::WindowingFunction<float>::hamming);
}

DelayProcessor::~DelayProcessor() {
    //delete delayLineBase->delayLine;
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
        }
    }
    else
    {
        // Handle file not found error
    }
}

void DelayProcessor::setInterpolationType(int comboBoxChoice, juce::dsp::ProcessSpec& spec) {
    switch (comboBoxChoice) {
    case 0:
    default:
        delayLineBase = std::make_unique<LinearDelayLine>();
        break;
    case 1:
        delayLineBase = std::make_unique<ThiranDelayLine>();
        break;
    case 2:
        delayLineBase = std::make_unique<Lagrange3rdDelayLine>();
        break;
    } 
}

void DelayProcessor::setDelayTime(double delayTimeInSeconds, double sampleRate) {
    double clampedDelayTime = juce::jlimit(0.0, maxDelayTime, delayTimeInSeconds);
    delayLine.setDelay(clampedDelayTime * sampleRate);
    delayLineBase->setDelay(clampedDelayTime * sampleRate);
    currentDelayTime = clampedDelayTime; // in seconds
}

void DelayProcessor::setDelayTimeInSamples(float delayTimeInSamples, double sampleRate) {
    // double clampedDelayTimeInSamples = juce::jlimit(0.0, maxDelayTime * sampleRate, delayTimeInSamples * sampleRate);
    delayLine.setDelay(delayTimeInSamples);
    delayLineBase->setDelay(delayTimeInSamples);
    currentDelayInSamples = delayTimeInSamples;
}

void DelayProcessor::setMaxDelayTime(double maxDelayTimeInSeconds, double sampleRate) {
    delayLine.setMaximumDelayInSamples(sampleRate * maxDelayTimeInSeconds);
    delayLineBase->setMaximumDelayInSamples(sampleRate * maxDelayTimeInSeconds);
    maxDelayTime = maxDelayTimeInSeconds;
}

void DelayProcessor::prepare(juce::dsp::ProcessSpec& spec) {
    //delayLine.prepare(spec);
    delayLineBase->prepare(spec);
    setMaxDelayTime(maxDelayTime, spec.sampleRate);
    filter.reset();
    filter.prepare(spec);
    convolver.reset();
    convolver.prepare(spec);
}

// Not used
void DelayProcessor::processBlock(juce::dsp::AudioBlock<float>& block) { 
    delayLine.process(juce::dsp::ProcessContextReplacing<float>(block));
    //convolver.process(block);
}

void DelayProcessor::process(juce::dsp::ProcessContextReplacing<float>& context) {
    delayLineBase->process(context);
    //delayLine.process(context);
    filter.process(context);
    convolver.process(context);
}
