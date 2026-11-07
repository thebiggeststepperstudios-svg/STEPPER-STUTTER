#pragma once
#include <JuceHeader.h>

class HalfTimeEngine
{
public:
    void prepare(double sampleRate, int maxBlockSize, int channels);
    void process(juce::AudioBuffer<float>&, float bpm, bool enabled, int speedIndex,
                 float glideMs, bool pitchDrop, float mix, float outputGain);
private:
    juce::AudioBuffer<float> ring;
    double sampleRate = 44100.0;
    int writePosition = 0, channels = 2;
    juce::SmoothedValue<float> mixValue, gainValue;
    float wetEnvelope = 0.0f;
};
