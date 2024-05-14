/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UnholyDistortionAudioProcessor::UnholyDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    treeState.addParameterListener("input", this);
    treeState.addParameterListener("output", this);
   
}

UnholyDistortionAudioProcessor::~UnholyDistortionAudioProcessor()
{
    treeState.removeParameterListener("input", this);
    treeState.removeParameterListener("output", this);
   
}

//==============================================================================
const juce::String UnholyDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UnholyDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

juce::AudioProcessorValueTreeState::ParameterLayout UnholyDistortionAudioProcessor::createParameterLayout()
{

    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;

    auto pInput = std::make_unique<juce::AudioParameterFloat>("input", "Input", 0.0f, 48.0f, 24.0f);
    auto pOutput = std::make_unique<juce::AudioParameterFloat>("output", "Output", -24.0f, 12.0f, -6.0f);
    auto plowpasscutoff = std::make_unique<juce::AudioParameterFloat>("lowPassCutOff", "LPF Cutoff", 5000.0f, 20000.0f, 7000.0f);

    params.push_back(std::move(pInput));
    params.push_back(std::move(pOutput));
    params.push_back(std::move(plowpasscutoff));

    return { params.begin(), params.end() };
}



void UnholyDistortionAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    updateParameters();
    
}

void  UnholyDistortionAudioProcessor::updateParameters()
{
    
    outputModule.setGainDecibels(treeState.getRawParameterValue("output")->load());
    updatelowPassFilter(-24.0f, treeState.getRawParameterValue("lowPassCutOff")->load(), 0.5f);
    unholyFuzz.setDrive(treeState.getRawParameterValue("input")->load());


}

bool UnholyDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UnholyDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UnholyDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UnholyDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UnholyDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void UnholyDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String UnholyDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void UnholyDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void UnholyDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    outputModule.setGainDecibels(-6.0f);
    updatelowPassFilter(-24.0f, treeState.getRawParameterValue("lowPassCutOff")->load(), 0.5f);
    unholyFuzz.setDrive(treeState.getRawParameterValue("input")->load());
    
   
    outputModule.setRampDurationSeconds(0.02);
    outputModule.prepare(spec);
    lowPassFilter.prepare(spec);

    rmsLevelLeft.reset(sampleRate, 0.5);
    rmsLevelRight.reset(sampleRate, 0.5);

    rmsLevelLeft.setCurrentAndTargetValue(-100.f);
    rmsLevelRight.setCurrentAndTargetValue(-100.f);
}

void UnholyDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UnholyDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void UnholyDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

   
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
   

    auto block = juce::dsp::AudioBlock<float>{ buffer }.getSubsetChannelBlock(0, totalNumOutputChannels);
    juce::dsp::ProcessContextReplacing<float> context{ block };
    auto& input = context.getInputBlock();
    auto& output = context.getOutputBlock();

    //input meter

    rmsLevelLeft.skip(input.getNumSamples());
    {
        const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, input.getNumSamples()));

        if (value < rmsLevelLeft.getCurrentValue())
            rmsLevelLeft.setTargetValue(value);
        else
            rmsLevelLeft.setCurrentAndTargetValue(value);
    }

    //processing

    unholyFuzz.process(juce::dsp::ProcessContextReplacing<float>(block));
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    outputModule.process(juce::dsp::ProcessContextNonReplacing<float>(buffer, block));


    //output meter
    rmsLevelRight.skip(output.getNumSamples());
    {
        const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));

        if (value < rmsLevelRight.getCurrentValue())
            rmsLevelRight.setTargetValue(value);
        else
            rmsLevelRight.setCurrentAndTargetValue(value);
    }

}

//==============================================================================
bool UnholyDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UnholyDistortionAudioProcessor::createEditor()
{
    return new UnholyDistortionAudioProcessorEditor (*this);
}

//==============================================================================
void UnholyDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream(stream);
}

void UnholyDistortionAudioProcessor::updatelowPassFilter(float gain, float cutoff, float q)
{
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), cutoff, q, juce::Decibels::decibelsToGain(gain));
}

void UnholyDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));

    if (tree.isValid())
    {
        treeState.state = tree;
    }
}

float UnholyDistortionAudioProcessor::getRMSvalue(const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0)
        return rmsLevelLeft.getCurrentValue();
    if (channel == 1)
        return rmsLevelRight.getCurrentValue();
    return 0.f;

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UnholyDistortionAudioProcessor();
}
