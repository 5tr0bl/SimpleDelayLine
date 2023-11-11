/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SimpleDelayLineAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleDelayLineAudioProcessorEditor (SimpleDelayLineAudioProcessor&);
    ~SimpleDelayLineAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SimpleDelayLineAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& tree; // Reference to the processor's tree
    
    juce::TextButton loadBtn;
    juce::Slider delaySlider;
    juce::ToggleButton convolutionButton;
    juce::Slider delayMixKnob;
    juce::ComboBox interpolationTypeComboBox;


    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delaySliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> convolutionButtonAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayMixKnobAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> interpolationTypeComboBoxAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayLineAudioProcessorEditor)
};
