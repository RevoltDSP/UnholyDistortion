/*
  ==============================================================================

    chickenSlider.cpp
    Created: 3 Oct 2023 8:26:33pm
    Author:  PC

  ==============================================================================
*/

#include <JuceHeader.h>
#include "chickenSlider.h"

//==============================================================================
chickenSlider::chickenSlider() : Slider()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setLookAndFeel(&chickenSliderLookAndFeel);
}

chickenSlider::~chickenSlider()
{
    setLookAndFeel(nullptr);
}

void chickenSlider::mouseDown(const juce::MouseEvent& event)
{
    juce::Slider::mouseDown(event);
    setMouseCursor(juce::MouseCursor::NoCursor);
    mousePosition = juce::Desktop::getMousePosition();
}

void chickenSlider::mouseUp(const juce::MouseEvent& event)
{
    Slider::mouseUp(event);
    juce::Desktop::setMousePosition(mousePosition);
    setMouseCursor(juce::MouseCursor::NormalCursor);
};

