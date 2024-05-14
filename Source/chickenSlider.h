/*
  ==============================================================================

    chickenSlider.h
    Created: 3 Oct 2023 8:26:33pm
    Author:  PC

  ==============================================================================
*/

#pragma once

#pragma once

#include <JuceHeader.h>
#include "chickenSliderLookAndFeel.h"

//==============================================================================
/*
*/
class chickenSlider : public juce::Slider
{
public:
    chickenSlider();
    ~chickenSlider();

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
   


private:
    chickenSliderLookAndFeel chickenSliderLookAndFeel;
    juce::Point<int> mousePosition;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(chickenSlider)
};
