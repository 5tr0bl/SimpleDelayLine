/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DelayProcessor.h"

//==============================================================================
SimpleDelayLineAudioProcessor::SimpleDelayLineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )    , tree(*this, nullptr, "parameters", SimpleDelayLineAudioProcessor::createParameters())
                            //, delayProcessor()
#endif
{
    //NormalisableRange<float> cutoffRange(20.0f, 20000.0f);
    //tree.createAndAddParameter("cutoff", "cutoff", "Cutoff", cutoffRange, 8000.0f, nullptr, nullptr);
    
    // Load IRs from pre-defined locations
    /*juce::File irFileFrontal = juce::File::getCurrentWorkingDirectory()
        .getChildFile("Resources")
        .getChildFile("hrir_frontal.wav");*/
    
    /*juce::File irFileLateral = juce::File::getCurrentWorkingDirectory()
        .getChildFile("Resources")
        .getChildFile("hrir_lateral.wav");*/
}

SimpleDelayLineAudioProcessor::~SimpleDelayLineAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleDelayLineAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleDelayLineAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayLineAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayLineAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleDelayLineAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleDelayLineAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleDelayLineAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleDelayLineAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleDelayLineAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleDelayLineAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleDelayLineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    double maxDelay = 2;
    float defaultDelayInSamples = 100;
    double defaultCutoff = 3000.0;

    int defaultInterpolationIndex = 0; // Linear
    interpolationType = defaultInterpolationIndex;

    juce::File irFileFrontal("C:\\Users\\Micha\\source\\repos\\SimpleDelayLine\\Resources\\hrir_frontal.wav");
    bool a = irFileFrontal.existsAsFile();
    directProcessor.setMaxDelayTime(maxDelay, sampleRate);
    //directProcessor.setDelayTime(0.01, sampleRate);
    directProcessor.setFirFilter(20000.0, sampleRate);
    //directProcessor.setDistance(1.0f, SONIC_SPEED, sampleRate);
    directProcessor.setDelayTimeInSamples(0, sampleRate);
    directProcessor.setHRIR(irFileFrontal);
    directProcessor.prepare(spec);

    juce::File irFileLateral("C:\\Users\\Micha\\source\\repos\\SimpleDelayLine\\Resources\\hrir_lateral.wav");
    bool c = irFileLateral.existsAsFile();
    // DelayProcessor delayProcessor(maxDelay, sampleRate);
    delayProcessor.setMaxDelayTime(maxDelay, sampleRate);
    //delayProcessor.setDelayTime(defaultDelayTime, sampleRate);
    delayProcessor.setFirFilter(defaultCutoff, sampleRate);
    //delayProcessor.setDistance(8.0f, SONIC_SPEED, sampleRate);
    delayProcessor.setDelayTimeInSamples(defaultDelayInSamples, sampleRate);
    delayProcessor.setHRIR(irFileLateral);
    delayProcessor.prepare(spec);

    //convolver.reset();
    //convolver.prepare(spec);
}

void SimpleDelayLineAudioProcessor::releaseResources()
{
    // free the DelayProcessors and their members ...
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleDelayLineAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SimpleDelayLineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    updateInterpolationType(); // maybe better do this early on

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    double sampleRate = getSampleRate();
    auto numSamples = buffer.getNumSamples();

    juce::dsp::AudioBlock<float> inputBlock(buffer);
    auto directContext = dsp::ProcessContextReplacing<float>(inputBlock);

    juce::AudioBuffer<float> delayBuffer(buffer.getNumChannels(), numSamples);
    const auto inputChannels = buffer.getArrayOfReadPointers();
    auto delayedChannels = delayBuffer.getArrayOfWritePointers();

    // Copy the data from buffer to delayedBuffer
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        std::copy(inputChannels[channel], inputChannels[channel] + buffer.getNumSamples(), delayedChannels[channel]);
    }

    juce::dsp::AudioBlock<float> delayBlock(delayBuffer);
    auto delayedContext = dsp::ProcessContextReplacing<float>(delayBlock);

    updateDelayTime(sampleRate);
    directProcessor.process(directContext);
    delayProcessor.process(delayedContext);

    //// Add this when mixing between the two is desired
    // float mixLevel = delayProcessor.getMix();
    // delayBlock.multiplyBy(mixLevel);     
    // inputBlock.multiplyBy(1.0f - mixLevel);
    inputBlock.add(delayBlock);

    //// Is convolution toggling is desired, this parameter now needs to be passed down to the DelayProcessors
    /*bool convolutionEnabled = *tree.getRawParameterValue("convolutionToggle");
    if (convolutionEnabled && convolver.getCurrentIRSize() > 0)
    {
        convolver.process(directContext);
    }*/
}

void SimpleDelayLineAudioProcessor::updateDelayTime(double sampleRate) 
{
    float delay = *tree.getRawParameterValue("delay");
    delayProcessor.setDelayTimeInSamples(delay, sampleRate);
    
    /*
    float distance = *tree.getRawParameterValue("distance");
    delayProcessor.setDistance(distance, SONIC_SPEED, sampleRate);
    float mixLevel = *tree.getRawParameterValue("delayMix");
    delayProcessor.setMix(mixLevel);
    */
}

void SimpleDelayLineAudioProcessor::updateInterpolationType() {
    int interpolationTypeGUI = *tree.getRawParameterValue("interpolationType");
    if (interpolationTypeGUI != interpolationType)
    {
        interpolationType = interpolationTypeGUI;
        // call each DelayProcessors setInterpolationType() 
    }
}

void SimpleDelayLineAudioProcessor::updateFilter(double sampleRate)
{
    //float freq = *tree.getRawParameterValue("cutoff");
    delayProcessor.setFirFilter(8000.0, sampleRate);
    // *lowPassFilter.state = *dsp::FilterDesign<float>::designFIRLowpassWindowMethod(freq, getSampleRate(), 21, dsp::WindowingFunction<float>::hamming);
}


void SimpleDelayLineAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{

}

//==============================================================================
bool SimpleDelayLineAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleDelayLineAudioProcessor::createEditor()
{
    return new SimpleDelayLineAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleDelayLineAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleDelayLineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleDelayLineAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleDelayLineAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterBool>("convolutionToggle", "Convolution Enabled", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delay", "Delay", 0.0f, 22100.0f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delayMix", "Delay Mix", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("distance", "Distance", 0.1f, 20.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("interpolationType", "DelayLineInterpolationType", 0, 2, 0));

    return {params.begin(), params.end()};
}
