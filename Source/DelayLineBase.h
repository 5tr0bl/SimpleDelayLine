#pragma once
#include <JuceHeader.h>

class DelayLineBase {
public:
    //virtual DelayLineBase() {}
    virtual ~DelayLineBase() {};
    virtual void prepare(juce::dsp::ProcessSpec& spec) = 0;
    virtual void process(juce::dsp::ProcessContextReplacing<float>& context) = 0;
    virtual void setDelay(float delayInSamples) = 0;
    virtual void setMaximumDelayInSamples(double maximumDelayInSamples) = 0;
    void* delayLine;
};

class LinearDelayLine : public DelayLineBase {
public:
    //juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine = static_cast<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>>(delayLine);
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    
    LinearDelayLine() {
        //delayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>();
        delayLine = static_cast<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>>(delayLine);
    }
    void prepare(juce::dsp::ProcessSpec& spec) override {
        delayLine.prepare(spec);
    }
    void process(juce::dsp::ProcessContextReplacing<float>& context) override {
        delayLine.process(context);
    }
    virtual void setDelay(float delayInSamples) override {
        delayLine.setDelay(delayInSamples);
    }
    virtual void setMaximumDelayInSamples(double maximumDelayInSamples) override {
        delayLine.setMaximumDelayInSamples(maximumDelayInSamples);
    }
};

class ThiranDelayLine : public DelayLineBase {
public:
    //juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Thiran> delayLine = static_cast<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Thiran>>(delayLine);
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Thiran> delayLine;
    
    ThiranDelayLine() {
        //delayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Thiran>();
        delayLine = static_cast<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Thiran>>(delayLine);
    }
    void prepare(juce::dsp::ProcessSpec& spec) override {
        delayLine.prepare(spec);
    }
    void process(juce::dsp::ProcessContextReplacing<float>& context) override {
        delayLine.process(context);
    }
    virtual void setDelay(float delayInSamples) override {
        delayLine.setDelay(delayInSamples);
    }
    virtual void setMaximumDelayInSamples(double maximumDelayInSamples) override {
        delayLine.setMaximumDelayInSamples(maximumDelayInSamples);
    }
};

class Lagrange3rdDelayLine : public DelayLineBase {
public:
    //juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLine = static_cast<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>>(delayLine);
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLine;

    Lagrange3rdDelayLine() {
        //delayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>();
        delayLine = static_cast<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>>(delayLine);
    }
    void prepare(juce::dsp::ProcessSpec& spec) override {
        delayLine.prepare(spec);
    }
    void process(juce::dsp::ProcessContextReplacing<float>& context) override {
        delayLine.process(context);
    }
    virtual void setDelay(float delayInSamples) override {
        delayLine.setDelay(delayInSamples);
    }
    virtual void setMaximumDelayInSamples(double maximumDelayInSamples) override {
        delayLine.setMaximumDelayInSamples(maximumDelayInSamples);
    }
};