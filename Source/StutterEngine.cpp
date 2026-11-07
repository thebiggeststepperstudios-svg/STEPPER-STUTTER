#include "StutterEngine.h"

void StutterEngine::prepare(double sr, int, int ch)
{
    sampleRate = sr; channels = juce::jlimit(1, 2, ch);
    ring.setSize(channels, juce::jmax(65536, (int) std::ceil(sr * 4.0)), false, true, true);
    slice.setSize(channels, ring.getNumSamples(), false, true, true);
    ring.clear(); slice.clear(); writePosition = 0; samplesToBoundary = 0; playing = false;
    mixValue.reset(sr, 0.02); gainValue.reset(sr, 0.02);
}

void StutterEngine::process(juce::AudioBuffer<float>& audio, float bpm, bool enabled, int rateIndex,
                            float gate, float decayDb, float pitchSemitones, bool reverse,
                            float probability, float mix, float outputGain)
{
    static constexpr double divisions[] = { 1.0, 0.5, 0.25, 0.125, 0.6666667, 0.3333333, 1.5, 0.75 };
    const double beat = sampleRate * 60.0 / juce::jlimit(30.0f, 300.0f, bpm);
    const int period = juce::jmax(16, (int) std::round(beat * divisions[juce::jlimit(0, 7, rateIndex)]));
    mixValue.setTargetValue(mix); gainValue.setTargetValue(juce::Decibels::decibelsToGain(outputGain));
    for (int i = 0; i < audio.getNumSamples(); ++i)
    {
        if (samplesToBoundary <= 0)
        {
            samplesToBoundary = period;
            const bool trigger = enabled && random.nextFloat() <= probability;
            if (trigger)
            {
                sliceLength = juce::jlimit(16, slice.getNumSamples(), period);
                for (int ch = 0; ch < channels; ++ch)
                    for (int s = 0; s < sliceLength; ++s)
                        slice.setSample(ch, s, ring.getSample(ch, (writePosition - sliceLength + s + ring.getNumSamples()) % ring.getNumSamples()));
                slicePosition = 0; repeatIndex = 0; playing = true; reversePlayback = reverse;
            }
            else playing = false;
        }
        const float wetMix = mixValue.getNextValue();
        const float gain = gainValue.getNextValue();
        const int gateSamples = juce::jmax(1, (int) (period * juce::jlimit(0.05f, 1.0f, gate)));
        const bool gateOpen = samplesToBoundary >= period - gateSamples;
        const float repeatGain = juce::Decibels::decibelsToGain(juce::jlimit(-48.0f, 12.0f, decayDb * repeatIndex));
        for (int ch = 0; ch < audio.getNumChannels(); ++ch)
        {
            const int rc = juce::jmin(ch, channels - 1);
            const float input = audio.getSample(ch, i);
            ring.setSample(rc, writePosition, input);
            float wet = 0.0f;
            if (playing && gateOpen)
            {
                const double rate = std::pow(2.0, pitchSemitones / 12.0);
                const int pos = reversePlayback ? sliceLength - 1 - slicePosition : slicePosition;
                const int idx = juce::jlimit(0, sliceLength - 1, pos);
                const float fade = juce::jmin(1.0f, juce::jmin((float) slicePosition / 16.0f, (float) (sliceLength - slicePosition) / 16.0f));
                wet = slice.getSample(rc, idx) * repeatGain * fade;
                slicePosition += (int) juce::jmax(1.0, rate);
                if (slicePosition >= sliceLength) { slicePosition = 0; ++repeatIndex; }
            }
            audio.setSample(ch, i, (input + wetMix * (wet - input)) * gain);
        }
        writePosition = (writePosition + 1) % ring.getNumSamples();
        --samplesToBoundary;
    }
}
