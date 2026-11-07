#pragma once
#include <JuceHeader.h>

class StutterEngine
{
public:
    void prepare(double sampleRate, int maxBlockSize, int channels);
    void process(juce::AudioBuffer<float>&, float bpm, bool enabled, int rateIndex,
                 float gate, float decayDb, float pitchSemitones, bool reverse,
                 float probability, float mix, float outputGain);
private:
    juce::AudioBuffer<float> ring, slice;
    double sampleRate = 44100.0;
    int writePosition = 0, channels = 2, sliceLength = 1, slicePosition = 0;
    int samplesToBoundary = 0, repeatIndex = 0;
    bool playing = false, reversePlayback = false;
    juce::SmoothedValue<float> mixValue, gainValue;
    juce::Random random;
};
