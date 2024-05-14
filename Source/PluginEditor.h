/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "chickenSlider.h"
#include "Component/VerticalMeter.h"


//==============================================================================
/**
*/
class UnholyDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    UnholyDistortionAudioProcessorEditor (UnholyDistortionAudioProcessor&);
    ~UnholyDistortionAudioProcessorEditor() override;

    //==============================================================================

    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:


    juce::Image background;

    chickenSlider inputSlider, volumeSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        inputAttach, volumeAttach;

    void attachSliders();

    juce::Label inputLabel, volumeLabel;

    std::vector<juce::Slider*> sliders =
    {
        &inputSlider,
        &volumeSlider, 
    };

    void setCommonSliderProps(juce::Slider& slider);

    UnholyDistortionAudioProcessor& audioProcessor;

   
    Gui::VerticalGradientMeter verticalMeterL, verticalMeterR;

    juce::Label inLabel, outLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnholyDistortionAudioProcessorEditor)
};
