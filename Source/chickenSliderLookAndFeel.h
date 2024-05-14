/*
  ==============================================================================

    chickenSliderLookAndFeel.h
    Created: 3 Oct 2023 8:27:17pm
    Author:  PC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class chickenSliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    chickenSliderLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
    void drawLabel(juce::Graphics& g, juce::Label& label);
    

private:
    juce::
        Image knobRelease;
};