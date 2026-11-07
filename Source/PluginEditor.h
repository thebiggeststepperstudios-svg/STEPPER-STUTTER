#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PixelLookAndFeel.h"

class StepperStutterAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit StepperStutterAudioProcessorEditor(StepperStutterAudioProcessor&);
    ~StepperStutterAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    struct Control { juce::Slider slider; juce::Label label; std::unique_ptr<SliderAttachment> attachment; };
    void addSlider(Control&, const juce::String&, const juce::String&, juce::Rectangle<int>);
    void addToggle(juce::ToggleButton&, const juce::String&, const juce::String&, juce::Rectangle<int>);
    StepperStutterAudioProcessor& processor;
    PixelLookAndFeel pixel;
    Control glide, halfMix, halfGain, gate, decay, pitch, probability, stutterMix, stutterGain, globalMix, master, bpm;
    juce::ToggleButton halfOn, pitchDrop, stutterOn, reverse, scanlines;
    std::unique_ptr<ButtonAttachment> halfOnAtt, pitchDropAtt, stutterOnAtt, reverseAtt;
    juce::ComboBox halfSpeed, rate, order;
    std::unique_ptr<ComboAttachment> halfSpeedAtt, rateAtt, orderAtt;
    float meter = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepperStutterAudioProcessorEditor)
};
