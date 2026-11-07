#include "PixelLookAndFeel.h"

PixelLookAndFeel::PixelLookAndFeel()
{
    setColour(juce::Slider::thumbColourId, juce::Colour(0xffe5b94e));
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff75bd5a));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff435954));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff111820));
    setColour(juce::ComboBox::textColourId, juce::Colour(0xffc2d5a2));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff75bd5a));
    setColour(juce::ToggleButton::textColourId, juce::Colour(0xffc2d5a2));
}

void PixelLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float pos,
                                         float start, float end, juce::Slider&)
{
    const int size = juce::jmin(w, h) - 12, left = x + (w - size) / 2, top = y + (h - size) / 2;
    const juce::Rectangle<int> r(left, top, size, size);
    g.setColour(juce::Colour(0xff111820)); g.fillRect(r);
    g.setColour(juce::Colour(0xff50605b)); g.drawRect(r, 3);
    const int bars = 12, active = juce::roundToInt(pos * (bars - 1));
    for (int i = 0; i < bars; ++i)
    {
        const float a = start + (end - start) * (float)i / (bars - 1);
        const int cx = left + size / 2 + juce::roundToInt(std::sin(a) * size * 0.36f);
        const int cy = top + size / 2 - juce::roundToInt(std::cos(a) * size * 0.36f);
        g.setColour(i <= active ? juce::Colour(0xff75bd5a) : juce::Colour(0xff34464a)); g.fillRect(cx - 2, cy - 2, 5, 5);
    }
    g.setColour(juce::Colour(0xffe5b94e)); g.fillRect(left + size / 2 - 3, top + size / 2 - 3, 7, 7);
}

void PixelLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& b, const juce::Colour&, bool over, bool down)
{
    auto r = b.getLocalBounds(); const bool on = b.getToggleState();
    g.setColour(down ? juce::Colour(0xffe5b94e) : on ? juce::Colour(0xff365a3d) : juce::Colour(0xff111820));
    g.fillRect(r); g.setColour(over || on ? juce::Colour(0xff9ee16a) : juce::Colour(0xff50605b)); g.drawRect(r, 2);
}

void PixelLookAndFeel::drawComboBox(juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox&)
{
    g.setColour(juce::Colour(0xff111820)); g.fillRect(0, 0, w, h);
    g.setColour(juce::Colour(0xff75bd5a)); g.drawRect(0, 0, w, h, 2);
    g.setColour(juce::Colour(0xffe5b94e)); g.fillRect(w - 13, h / 2 - 2, 6, 4);
}
juce::Font PixelLookAndFeel::getLabelFont(juce::Label&) { return juce::Font(10.0f, juce::Font::bold); }
