/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionPluginAudioProcessorEditor::DistortionPluginAudioProcessorEditor (DistortionPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 200);

	addAndMakeVisible(gain);
	addAndMakeVisible(volume);
	addAndMakeVisible(hiFilter);
	addAndMakeVisible(lowFilter);


	addAndMakeVisible(gainLabel);
	addAndMakeVisible(volumeLabel);
	addAndMakeVisible(hiFilterLabel);
	addAndMakeVisible(lowFilterLabel);

	gainLabel.setText("Gain",juce::dontSendNotification);
	volumeLabel.setText("Volume", juce::dontSendNotification);
	hiFilterLabel.setText("Bass", juce::dontSendNotification);
	lowFilterLabel.setText("Treble", juce::dontSendNotification);;

	gain.setSliderStyle(juce::Slider::Rotary);
	volume.setSliderStyle(juce::Slider::Rotary);
	hiFilter.setSliderStyle(juce::Slider::Rotary);
	lowFilter.setSliderStyle(juce::Slider::Rotary);

	gain.setTextBoxStyle(juce::Slider::NoTextBox,false,50,25);
	volume.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 25);
	hiFilter.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 25);
	lowFilter.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 25);

	gain.setRange(p.GetGainRange(0), p.GetGainRange(1), p.GetGainRange(2));
	volume.setRange(p.GetVolumeRange(0), p.GetVolumeRange(1), p.GetVolumeRange(2));
	hiFilter.setRange(p.GetHiFilterRange(0), p.GetHiFilterRange(1), p.GetHiFilterRange(2));
	lowFilter.setRange(p.GetLowFilterRange(0), p.GetLowFilterRange(1), p.GetLowFilterRange(2));
	 
	gain.addListener(this);
	volume.addListener(this);
	hiFilter.addListener(this);
	lowFilter.addListener(this);
}

DistortionPluginAudioProcessorEditor::~DistortionPluginAudioProcessorEditor()
{
}

//==============================================================================
void DistortionPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
	
  
}

void DistortionPluginAudioProcessorEditor::resized()
{
	gain.setBounds(50, 50, 100, 100);
	volume.setBounds(150, 50, 100, 100);
	hiFilter.setBounds(250, 50, 100, 100);
	lowFilter.setBounds(350, 50, 100, 100);

	gainLabel.setBounds(78, 20, 50, 25);
	volumeLabel.setBounds(174, 20, 50, 25);
	hiFilterLabel.setBounds(275, 20, 50, 25);
	lowFilterLabel.setBounds(378, 20, 50, 25);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
void DistortionPluginAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
	if (slider == &gain) {
		audioProcessor.GetGainValue() = (float)gain.getValue();
	}
	else if (slider == &volume) {
		audioProcessor.GetVolumeValue() = (float)volume.getValue();
	}
	else if (slider == &hiFilter) {
		audioProcessor.GetHiFilterValue() = (float)hiFilter.getValue();
	}
	else if (slider == &lowFilter) {
		audioProcessor.GetLowFilterValue() = (float)lowFilter.getValue();
	}
}