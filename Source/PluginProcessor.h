#pragma once
#include <JuceHeader.h>
#include "HalfTimeEngine.h"
#include "StutterEngine.h"

class StepperStutterAudioProcessor final : public juce::AudioProcessor
{
public:
    StepperStutterAudioProcessor();
    ~StepperStutterAudioProcessor() override = default;
    void prepareToPlay(double, int) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float> meterLevel { 0.0f };
    std::atomic<float> beatPulse { 0.0f };
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
private:
    HalfTimeEngine halfTime;
    StutterEngine stutter;
    double sampleRateHz = 44100.0;
    juce::AudioBuffer<float> dryBuffer, stageBuffer;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepperStutterAudioProcessor)
};
