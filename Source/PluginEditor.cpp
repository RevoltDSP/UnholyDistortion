/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UnholyDistortionAudioProcessorEditor::UnholyDistortionAudioProcessorEditor(UnholyDistortionAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    verticalMeterL([&]() {return audioProcessor.getRMSvalue(0); }),
    verticalMeterR([&]() {return audioProcessor.getRMSvalue(1); })
{


   

    addAndMakeVisible(verticalMeterL);
    addAndMakeVisible(verticalMeterR);

    addAndMakeVisible(inLabel);
    inLabel.setText("In", juce::NotificationType::dontSendNotification);
    inLabel.setColour(juce::Label::textColourId, juce::Colours::ghostwhite);
    inLabel.setJustificationType(juce::Justification::centredTop);
    inLabel.setFont(juce::Font(10.0f, juce::Font::bold));

    addAndMakeVisible(outLabel);
    outLabel.setText("Out", juce::NotificationType::dontSendNotification);
    outLabel.setColour(juce::Label::textColourId, juce::Colours::ghostwhite);
    outLabel.setJustificationType(juce::Justification::centredTop);
    outLabel.setFont(juce::Font(10.f, juce::Font::bold));
   


    for (int i = 0; i < sliders.size(); i++)
    {
        setCommonSliderProps(*sliders[i]);
    }

    attachSliders();


    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 500);

    startTimerHz(24);

}

UnholyDistortionAudioProcessorEditor::~UnholyDistortionAudioProcessorEditor()
{
    sliders.clear();
    sliders.shrink_to_fit();
}

void UnholyDistortionAudioProcessorEditor::timerCallback()
{
   // horizontalMeterL.setLevel(audioProcessor.getRMSvalue(0));
   // horizontalMeterR.setLevel(audioProcessor.getRMSvalue(1));
  //  horizontalMeterL.repaint();
  //  horizontalMeterR.repaint();

}

//==============================================================================
void UnholyDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{

    background = juce::ImageCache::getFromMemory(BinaryData::unholypluginnew_png, BinaryData::unholypluginnew_pngSize);
    g.drawImageWithin(background, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);

   
      
            g.setColour(juce::Colours::red);
            g.drawEllipse(190, 360, 10, 10, 10);
        
    
}

void UnholyDistortionAudioProcessorEditor::resized()
{
    volumeSlider.setBounds((((getWidth() / 5) * 2) + 7) - (100 / 2), 55, 100, 100);
    inputSlider.setBounds((((getWidth() / 5) * 3) + 9) - (100 / 2), 55, 100, 100);

    verticalMeterL.setBounds(10, 30, 10, 100);
    verticalMeterR.setBounds(480, 30, 10, 100);

    inLabel.setBounds(-5, 10, 40, 15);
    outLabel.setBounds(465, 10, 40, 15);
}

void  UnholyDistortionAudioProcessorEditor::setCommonSliderProps(juce::Slider& slider)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::ghostwhite);
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::red);
    addAndMakeVisible(slider);
}

void  UnholyDistortionAudioProcessorEditor::attachSliders()
{
    inputAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "input", inputSlider);
    volumeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "output", volumeSlider);
     
}
