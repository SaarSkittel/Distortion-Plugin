/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionPluginAudioProcessor::DistortionPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	oversample.reset(new juce::dsp::Oversampling<float>(2, 3, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, false));

}

DistortionPluginAudioProcessor::~DistortionPluginAudioProcessor()
{
}

//==============================================================================
const juce::String DistortionPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortionPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortionPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortionPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistortionPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistortionPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistortionPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();
	preGain.setGainDecibels(gainValue);
	filterhigh.state = juce::dsp::IIR::Coefficients < float>::makeHighPass(spec.sampleRate,hiFilterValue, 0.707f);
	waveShaper.functionToUse = juce::dsp::FastMathApproximations::tanh;
	filterlow.state = juce::dsp::IIR::Coefficients <float>::makeLowPass(spec.sampleRate, lowFilterValue, 0.707f);
	postGain.setGainDecibels(volumeValue);

	prepare(spec);
}

void DistortionPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void DistortionPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = 1;
    auto totalNumOutputChannels = getTotalNumOutputChannels();
	juce::dsp::AudioBlock<float> block(buffer);
	auto numSamples = buffer.getNumSamples();
    
	for (auto i = juce::jmin(2, totalNumInputChannels); i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, numSamples);
    process(juce::dsp::ProcessContextReplacing<float>(block));
	updateParameters();
	if (block.getNumChannels() > 2)
		block = block.getSubsetChannelBlock(0, 2);
	
        // ..do something to the data...
}

//==============================================================================
bool DistortionPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistortionPluginAudioProcessor::createEditor()
{
    return new DistortionPluginAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DistortionPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionPluginAudioProcessor();
}

void DistortionPluginAudioProcessor::updateParameters()
{
	preGain.setGainDecibels(gainValue);
	postGain.setGainDecibels(volumeValue);
	*filterhigh.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, 620-hiFilterValue,0.707f);
	*filterlow.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(spec.sampleRate, lowFilterValue, 0.707f);
}
void DistortionPluginAudioProcessor::process(juce::dsp::ProcessContextReplacing<float> context)
{
	juce::ScopedNoDenormals noDenormals;
	
	preGain.process(context);
	filterhigh.process(context);
	filterlow.process(context);
	juce::dsp::AudioBlock<float>oversampleBlock = oversample->processSamplesUp(context.getInputBlock());
	auto waveshaperContext = juce::dsp::ProcessContextReplacing<float>(oversampleBlock);
	waveShaper.process(waveshaperContext);
	waveshaperContext.getOutputBlock() *= 0.7f;
	oversample->processSamplesDown(context.getOutputBlock());
	postGain.process(context);
	
}

void DistortionPluginAudioProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
	preGain.prepare(spec);
	filterhigh.prepare(spec);
	filterlow.prepare(spec);
	oversample->initProcessing(static_cast<size_t>(spec.maximumBlockSize));
	reset();
}

void DistortionPluginAudioProcessor::reset()
{
	oversample->reset();
	filterhigh.reset();
	filterlow.reset();
}

float DistortionPluginAudioProcessor::GetGainRange(int num){
	return gainRange[num];
}
float DistortionPluginAudioProcessor::GetVolumeRange(int num) {
	return volumeRange[num];
}
float DistortionPluginAudioProcessor::GetHiFilterRange(int num) {
	return hiFilterRange[num];
}
float DistortionPluginAudioProcessor::GetLowFilterRange(int num) {
	return lowFilterRange[num];
}
float& DistortionPluginAudioProcessor::GetGainValue() {
	return gainValue;
}
float& DistortionPluginAudioProcessor::GetVolumeValue() {
	return volumeValue;
}
float& DistortionPluginAudioProcessor::GetHiFilterValue() {
	return hiFilterValue;
}
float& DistortionPluginAudioProcessor::GetLowFilterValue() {
	return lowFilterValue;
}
