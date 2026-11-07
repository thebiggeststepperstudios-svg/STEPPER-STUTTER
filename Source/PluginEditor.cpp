#include "PluginEditor.h"

namespace
{
juce::Rectangle<int> box(int x, int y, int w, int h) { return { x, y, w, h }; }
}

StepperStutterAudioProcessorEditor::StepperStutterAudioProcessorEditor(StepperStutterAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&pixel); setSize(640, 400); setResizable(false, false);
    addSlider(glide, "GLIDE", "glide", box(18, 146, 90, 82));
    addSlider(halfMix, "MIX", "halfMix", box(110, 146, 90, 82));
    addSlider(halfGain, "OUTPUT", "halfGain", box(20, 245, 85, 78));
    addToggle(halfOn, "HALF-TIME", "halfOn", box(20, 92, 105, 28));
    addToggle(pitchDrop, "PITCH -12", "pitchDrop", box(128, 92, 105, 28));
    halfSpeed.addItemList({ "1/2 SPEED", "1/4 SPEED" }, 1); addAndMakeVisible(halfSpeed);
    halfSpeedAtt = std::make_unique<ComboAttachment>(processor.parameters, "halfSpeed", halfSpeed);
    addSlider(gate, "GATE", "gate", box(246, 146, 78, 82));
    addSlider(decay, "DECAY", "decay", box(326, 146, 78, 82));
    addSlider(pitch, "PITCH", "pitch", box(406, 146, 78, 82));
    addSlider(probability, "CHANCE", "probability", box(486, 146, 78, 82));
    addSlider(stutterMix, "MIX", "stutterMix", box(246, 245, 78, 78));
    addSlider(stutterGain, "OUTPUT", "stutterGain", box(326, 245, 78, 78));
    addToggle(stutterOn, "STUTTER", "stutterOn", box(248, 92, 100, 28));
    addToggle(reverse, "REVERSE", "reverse", box(348, 92, 85, 28));
    rate.addItemList({ "1/4", "1/8", "1/16", "1/32", "1/4T", "1/8T", "1/4.", "1/8." }, 1);
    addAndMakeVisible(rate); rateAtt = std::make_unique<ComboAttachment>(processor.parameters, "rate", rate);
    order.addItemList({ "HALF > STUTTER", "STUTTER > HALF" }, 1); addAndMakeVisible(order);
    orderAtt = std::make_unique<ComboAttachment>(processor.parameters, "order", order);
    addSlider(globalMix, "GLOBAL MIX", "globalMix", box(22, 334, 98, 54));
    addSlider(master, "MASTER", "master", box(133, 334, 98, 54));
    addSlider(bpm, "MANUAL BPM", "bpm", box(245, 334, 98, 54));
    addToggle(scanlines, "SCANLINES", "", box(370, 352, 95, 25));
    startTimerHz(20);
}

StepperStutterAudioProcessorEditor::~StepperStutterAudioProcessorEditor() { setLookAndFeel(nullptr); }

void StepperStutterAudioProcessorEditor::addSlider(Control& c, const juce::String& title, const juce::String& id, juce::Rectangle<int> area)
{
    c.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); c.slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    c.slider.setName(title); addAndMakeVisible(c.slider);
    c.label.setText(title, juce::dontSendNotification); c.label.setJustificationType(juce::Justification::centred);
    c.label.setColour(juce::Label::textColourId, juce::Colour(0xffc2d5a2)); addAndMakeVisible(c.label);
    c.slider.setBounds(area.withTrimmedTop(15)); c.label.setBounds(area.withHeight(16));
    if (id.isNotEmpty()) c.attachment = std::make_unique<SliderAttachment>(processor.parameters, id, c.slider);
}

void StepperStutterAudioProcessorEditor::addToggle(juce::ToggleButton& b, const juce::String& name, const juce::String& id, juce::Rectangle<int> area)
{
    b.setButtonText(name); addAndMakeVisible(b); b.setBounds(area);
    if (id.isNotEmpty())
    {
        if (id == "halfOn") halfOnAtt = std::make_unique<ButtonAttachment>(processor.parameters, id, b);
        else if (id == "pitchDrop") pitchDropAtt = std::make_unique<ButtonAttachment>(processor.parameters, id, b);
        else if (id == "stutterOn") stutterOnAtt = std::make_unique<ButtonAttachment>(processor.parameters, id, b);
        else if (id == "reverse") reverseAtt = std::make_unique<ButtonAttachment>(processor.parameters, id, b);
    }
}

void StepperStutterAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto bg = juce::Colour(0xff111820), panel = juce::Colour(0xff202e35), trim = juce::Colour(0xff75bd5a);
    g.fillAll(bg); g.setColour(juce::Colour(0xffe5b94e)); g.fillRect(0, 0, 640, 46);
    g.setColour(juce::Colour(0xff182329)); g.setFont(juce::Font(22.0f, juce::Font::bold)); g.drawText("STEPPER STUTTER", 18, 8, 330, 30, juce::Justification::centredLeft);
    g.setFont(juce::Font(10.0f, juce::Font::bold)); g.drawText("PIXEL CIRCUIT // FX-01", 414, 10, 210, 24, juce::Justification::centredRight);
    g.setColour(panel); g.fillRect(12, 58, 220, 264); g.fillRect(240, 58, 388, 264);
    g.setColour(trim); g.drawRect(12, 58, 220, 264, 3); g.drawRect(240, 58, 388, 264, 3);
    g.setColour(juce::Colour(0xffc2d5a2)); g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.drawText("01  HALF-TIME", 22, 64, 190, 22, juce::Justification::centredLeft);
    g.drawText("02  STUTTER", 250, 64, 190, 22, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff34464a)); g.fillRect(16, 124, 210, 2); g.fillRect(244, 124, 380, 2);
    g.setColour(panel); g.fillRect(12, 328, 616, 64); g.setColour(trim); g.drawRect(12, 328, 616, 64, 3);
    g.setColour(juce::Colour(0xff182329)); g.setFont(juce::Font(11.0f, juce::Font::bold)); g.drawText("ORDER", 480, 333, 52, 18, juce::Justification::centred);
    g.setColour(juce::Colour(0xff0b1114)); g.fillRect(540, 335, 75, 42);
    g.setColour(juce::Colour(0xff75bd5a)); g.fillRect(544, 371 - (int)(juce::jlimit(0.0f, 1.0f, meter) * 34.0f), 67, (int)(juce::jlimit(0.0f, 1.0f, meter) * 34.0f));
    g.setColour(juce::Colour(0xff9ee16a)); g.fillRect(545, 339, (int)(juce::jlimit(0.0f, 1.0f, meter) * 64.0f), 3);
    if (scanlines.getToggleState()) { g.setColour(juce::Colour(0x18000000)); for (int y=0; y<400; y+=4) g.fillRect(0, y, 640, 1); }
    if (processor.beatPulse.load(std::memory_order_relaxed) < 0.06f) { g.setColour(juce::Colour(0xffff5252)); g.fillRect(605, 14, 14, 14); }
    else { g.setColour(juce::Colour(0xff542e32)); g.fillRect(605, 14, 14, 14); }
}

void StepperStutterAudioProcessorEditor::resized()
{
    halfSpeed.setBounds(22, 286, 190, 25); rate.setBounds(438, 92, 174, 26); order.setBounds(468, 352, 65, 25);
}
void StepperStutterAudioProcessorEditor::timerCallback()
{ meter += (processor.meterLevel.load(std::memory_order_relaxed) - meter) * 0.35f; repaint(); }
