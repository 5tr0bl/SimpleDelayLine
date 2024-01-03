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

    interpolationType = DEFAULT_INTERPOLATION_INDEX;

    irFileFrontal = "C:\\Users\\Micha\\source\\repos\\SimpleDelayLine\\Resources\\hrir_frontal.wav";
    irFileLateral = "C:\\Users\\Micha\\source\\repos\\SimpleDelayLine\\Resources\\hrir_lateral.wav";
    
    initDelayProcessors(sampleRate, spec, interpolationType);
    delaySmoothed.reset(sampleRate, 1); // rampLength = ?? 
    // also maybe put these in initDelayProcessors()
    delaySmoothed.setCurrentAndTargetValue(DEFAULT_DELAY_IN_SAMPLES);
}

void SimpleDelayLineAudioProcessor::initDelayProcessors(double sampleRate, juce::dsp::ProcessSpec& spec,
                                                        int interpolationType) {
    // in the end iterate over an array of DelayProcessors
    directProcessor = std::make_unique<DelayProcessor>(MAX_DELAY, sampleRate, interpolationType);
    delayProcessor = std::make_unique<DelayProcessor>(MAX_DELAY, sampleRate, interpolationType);

    directProcessor->setPosition(DEFAULT_LISTENER_POSITION); // always resets to the same default position !!!  fix
    directProcessor->setDistance(DEFAULT_SOURCE_POSITION);
    directProcessor->setMaxDelayTime(MAX_DELAY, sampleRate);
    directProcessor->setFirFilter(20000.0, sampleRate);
    directProcessor->setDelayTimeInSamples(0, sampleRate);
    directProcessor->setHRIR(irFileFrontal);
    directProcessor->prepare(spec);

    delayProcessor->setPosition(DEFAULT_PHANTOM_SOURCE_POSITION); // always resets to the same default position !!!  fix
    delayProcessor->setDistance(DEFAULT_LISTENER_POSITION);
    delayProcessor->setMaxDelayTime(MAX_DELAY, sampleRate);
    delayProcessor->setFirFilter(DEFAULT_CUTOFF, sampleRate);
    delayProcessor->setDelayTimeInSamples(DEFAULT_DELAY_IN_SAMPLES, sampleRate);
    delayProcessor->setHRIR(irFileLateral);
    delayProcessor->prepare(spec);
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
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    double sampleRate = getSampleRate();
    auto numSamples = buffer.getNumSamples();

    updateInterpolationType(); // maybe better do this early on
    updateDistance();
    //updateDelayTime(sampleRate);
    updateDelayTimeRamped(sampleRate, numSamples);
    bool directSoundEnabled = *tree.getRawParameterValue("directSoundToggle");
    bool delayedSoundEnabled = *tree.getRawParameterValue("delayedSoundToggle");
    updateConvolutionState();

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
 
    directProcessor->process(directContext);
    inputBlock.multiplyBy(directSoundEnabled ? directProcessor->getGainFactor() : 0.0f);
    
    delayProcessor->process(delayedContext);
    delayBlock.multiplyBy(delayedSoundEnabled ? delayProcessor->getGainFactor() : 0.0f);

    inputBlock.add(delayBlock);
}

void SimpleDelayLineAudioProcessor::updateConvolutionState()
{
    bool convolutionEnabledGUI = *tree.getRawParameterValue("convolutionToggle");
    if (convolutionEnabledGUI != convolutionEnabled)
    {
        convolutionEnabled = convolutionEnabledGUI;
        directProcessor->convolutionEnabled = convolutionEnabledGUI;
        delayProcessor->convolutionEnabled = convolutionEnabledGUI;
    }
}

void SimpleDelayLineAudioProcessor::updateDelayTimeRamped(double sampleRate, float numSamples)
{
    // based on https://git.iem.at/audioplugins/IEMPluginSuite/-/blob/master/RoomEncoder/Source/PluginProcessor.cpp?ref_type=heads l.538
    float delayGUI = *tree.getRawParameterValue("delay");
    
    //const float maxDist = MAX_MOVING_SPEED / sampleRate * numSamples; // according to IEM
    const float maxDiff = sampleRate / SONIC_SPEED * MAX_MOVING_SPEED;

    // compare lastDelay and newDelay (of every DelayProcessor)
    auto lastDelay = delayProcessor->getDelayTimeInSamples(sampleRate); // maybe save this within the DelayProcessor
    auto diff = delayGUI - lastDelay;
    if (diff <= maxDiff)
    {
        delayProcessor->setDelayTimeInSamples(delayGUI, sampleRate);
    }
    else 
    {
        float newDelay = lastDelay + maxDiff;
        delayProcessor->setDelayTimeInSamples(newDelay, sampleRate);
    }
}

void SimpleDelayLineAudioProcessor::updateDelayTime(double sampleRate) // Maybe obsolete
{
    float delay = *tree.getRawParameterValue("delay");

    // this is wrong. after starting the ramp the ramps current value will not have reached the GUI delay value 
    if(delay != delaySmoothed.getCurrentValue()) 
    {
    // if gap between oldDelay & newDelay is too big
    // 
    // set the ramp length reasonably with reset()
        delaySmoothed.reset(sampleRate, 1);
    // set target value (first reset or first setTargetValue?)
        delaySmoothed.setTargetValue(delay); 
    }

    {
        delaySmoothed.setCurrentAndTargetValue(delay); // if it falls under "walking pace"; newDelay-oldDelay = small    
    }

    // only delay of the reflection (delay) processor gets modified
    delayProcessor->setDelayTimeInSamples(delay, sampleRate);
    // delayProcessor->setDelayTimeInSamples(delaySmoothed.getNextValue(), sampleRate);
}

void SimpleDelayLineAudioProcessor::updateDistance()
{
    float distanceGUI = *tree.getRawParameterValue("distance");
    directProcessor->setPosition(0, distanceGUI, 0); // Listener moves on a straight line (y-Axis) towards source
    directProcessor->setDistance(DEFAULT_SOURCE_POSITION);
    delayProcessor->setDistance(directProcessor->position);
}

void SimpleDelayLineAudioProcessor::updateInterpolationType() {
    int interpolationTypeGUI = *tree.getRawParameterValue("interpolationType");
    if (interpolationTypeGUI != interpolationType)
    {
        interpolationType = interpolationTypeGUI;
        initDelayProcessors(getSampleRate(), spec, interpolationType);
    }
}

void SimpleDelayLineAudioProcessor::updateFilter(double sampleRate) // unused right now
{
    //float freq = *tree.getRawParameterValue("cutoff");
    delayProcessor->setFirFilter(8000.0, sampleRate);
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
    params.push_back(std::make_unique<juce::AudioParameterBool>("directSoundToggle", "Direct Sound On", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>("delayedSoundToggle", "Delayed Sound On", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delay", "Delay", 0.0f, 22100.0f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("distance", "Distance", 1.0f, 5.0f, 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("interpolationType", "DelayLineInterpolationType", 0, 2, DEFAULT_INTERPOLATION_INDEX));

    return {params.begin(), params.end()};
}
