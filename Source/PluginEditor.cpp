/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DelayProcessor.h"

//==============================================================================
SimpleDelayLineAudioProcessorEditor::SimpleDelayLineAudioProcessorEditor (SimpleDelayLineAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), tree(p.tree)
{
    setSize (400, 300);
    /*
    addAndMakeVisible(loadBtn);
    loadBtn.setButtonText("Load IR");
    loadBtn.onClick = [this]()
    {
        fileChooser = std::make_unique<juce::FileChooser>("Choose IR", audioProcessor.root, "*");
        const auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectDirectories;
        fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
            {
                juce::File result(chooser.getResult());

                if (result.getFileExtension() == ".wav" || result.getFileExtension() == ".mp3")
                {
                    audioProcessor.savedFile = result;
                    audioProcessor.root = result.getParentDirectory().getFullPathName();
                    audioProcessor.convolver.reset();
                    audioProcessor.convolver.loadImpulseResponse(audioProcessor.savedFile,
                        juce::dsp::Convolution::Stereo::yes,
                        juce::dsp::Convolution::Trim::yes,
                        0);
                    //audioProcessor.convolver.prepare(audioProcessor.spec);
                }

            });
    };
    */
    convolutionButton.setClickingTogglesState(true);
    //convolutionButton.setToggleable(true);
    convolutionButton.onClick = [this]() {};
    convolutionButton.setButtonText("Enable Convolution");
    convolutionButton.setEnabled(false);
    addAndMakeVisible(convolutionButton);
    convolutionButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(tree, "convolutionToggle", convolutionButton);

    delaySlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    delaySlider.setRange(audioProcessor.delayTimeRange.start, audioProcessor.delayTimeRange.end, audioProcessor.delayTimeRange.interval);
    delaySlider.setValue(100);
    delaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, delaySlider.getWidth() * 0.5f, 25); // check the last two values!! 
    delaySlider.setTextValueSuffix(" samples");
    //delaySlider.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(delaySlider);
    delaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(tree, "delay", delaySlider);

    distanceSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    distanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, distanceSlider.getWidth() * 0.3f, 25); // check the last two values!!
    distanceSlider.setTextValueSuffix(" m");
    addAndMakeVisible(distanceSlider);
    distanceSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(tree, "distance", distanceSlider);

    interpolationTypeComboBox.addItemList({ "Linear", "Thiran", "Lagrange3rd"}, 1);
    interpolationTypeComboBox.setSelectedItemIndex(2);
    interpolationTypeComboBox.onChange = [this]() {
        DBG("DelayLine Interpolation Type: " << interpolationTypeComboBox.getItemText(interpolationTypeComboBox.getSelectedItemIndex()));
        };
    interpolationTypeComboBox.setTitle("DelayLineInterpolationType");
    addAndMakeVisible(interpolationTypeComboBox);
    interpolationTypeComboBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(tree, "interpolationType", interpolationTypeComboBox);
}

SimpleDelayLineAudioProcessorEditor::~SimpleDelayLineAudioProcessorEditor()
{
}

//==============================================================================
void SimpleDelayLineAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SimpleDelayLineAudioProcessorEditor::resized()
{
    auto width = getWidth();
    auto height = getHeight();

    // Load IR Button bounds; IR Button not rendered at the moment
    const auto btnX = width * 0.7;
    const auto btnY = width * 0.3;
    const auto btnWidth = width * 0.3;
    const auto btnHeight = btnWidth * 0.5;
    loadBtn.setBounds(btnX, btnY, btnWidth, btnHeight);

    // delay slider bounds
    const auto delSliWidth = width * 0.5;
    const auto delSliHeight = delSliWidth * 0.5;
    delaySlider.setBounds(0,
                          0,
                          delSliWidth,
                          delSliHeight);

    // distance slider
    distanceSlider.setBounds(0,
                             delSliHeight,
                             width * 0.5,
                             width * 0.25);

    // convolution toggler
    const auto convBtnX = width * 0.7;
    const auto convBtnY = 0;
    const auto convBtnWidth = width * 0.3;
    const auto convBtnHeight = convBtnWidth;
    convolutionButton.setBounds(convBtnX, convBtnY, convBtnWidth, convBtnHeight);

    interpolationTypeComboBox.setBounds(285,
                                        100,
                                        105,
                                        35);
}
