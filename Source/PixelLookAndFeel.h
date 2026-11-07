#pragma once
#include <JuceHeader.h>

class PixelLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    PixelLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawComboBox(juce::Graphics&, int, int, bool, int, int, int, int, juce::ComboBox&) override;
    juce::Font getLabelFont(juce::Label&) override;
};
