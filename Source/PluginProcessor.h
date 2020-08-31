/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class DistortionPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DistortionPluginAudioProcessor();
    ~DistortionPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
	void prepare(const juce::dsp::ProcessSpec&);
	void reset();
	void updateParameters();
	void process(juce::dsp::ProcessContextReplacing<float>);
	float GetGainRange(int);
	float GetVolumeRange(int);
	float GetHiFilterRange(int);
	float GetLowFilterRange(int);

	float& GetGainValue();
	float& GetVolumeValue();
	float& GetHiFilterValue();
	float& GetLowFilterValue();

private:
	juce::dsp::Gain<float>preGain, postGain;
	juce::dsp::WaveShaper<float>waveShaper;
	std::unique_ptr<juce::dsp::Oversampling<float>> oversample;
	juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>filterlow,filterhigh;
	
	float gainRange[3] = { 0.0f,60.0f,0.f };
	float volumeRange[3] = { 0.0f,20.0f,0.f };
	float hiFilterRange[3] = { 20.0f ,600.0f};
	float lowFilterRange[3] = { 1500.0f,20000.0f };

	float gainValue = 0.0f;
	float volumeValue = 0.0f;
	float hiFilterValue = 20.0f;
	float lowFilterValue = 400.0f;

	juce::dsp::ProcessSpec spec;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionPluginAudioProcessor)
};
