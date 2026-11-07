#include "HalfTimeEngine.h"

void HalfTimeEngine::prepare(double sr, int, int ch)
{
    sampleRate = sr; channels = juce::jlimit(1, 2, ch);
    ring.setSize(channels, juce::jmax(65536, (int) std::ceil(sr * 8.0)), false, true, true);
    ring.clear(); writePosition = 0; wetEnvelope = 0.0f;
    mixValue.reset(sr, 0.03); gainValue.reset(sr, 0.03);
}

void HalfTimeEngine::process(juce::AudioBuffer<float>& audio, float bpm, bool enabled, int speedIndex,
                             float glideMs, bool pitchDrop, float mix, float outputGain)
{
    const int length = ring.getNumSamples();
    const double safeBpm = juce::jlimit(30.0, 300.0, (double) bpm);
    const double beatSamples = sampleRate * 60.0 / safeBpm;
    const double loopSamples = beatSamples * (speedIndex == 0 ? 4.0 : 2.0);
    const double playbackRate = pitchDrop ? 0.5 : (speedIndex == 0 ? 0.25 : 0.5);
    const float glide = (float) juce::jlimit(0.002, 1.0, glideMs * 0.001);
    const int n = audio.getNumSamples();
    mixValue.setTargetValue(mix); gainValue.setTargetValue(juce::Decibels::decibelsToGain(outputGain));
    for (int i = 0; i < n; ++i)
    {
        const double phase = std::fmod((double) writePosition, loopSamples) / loopSamples;
        const float target = enabled && phase < 0.5 ? 1.0f : 0.0f;
        const float slew = (float) (1.0 / juce::jmax(1.0, glide * sampleRate));
        wetEnvelope += juce::jlimit(-slew, slew, target - wetEnvelope);
        const int delay = juce::jlimit(2, length - 2, (int) loopSamples);
        const int start = (writePosition - delay + length) % length;
        const double offset = std::fmod((double) writePosition * playbackRate, (double) delay);
        const double read = (double) start + offset;
        const int r0 = ((int) read) % length, r1 = (r0 + 1) % length;
        const float frac = (float) (read - std::floor(read));
        const float wetMix = mixValue.getNextValue() * wetEnvelope;
        const float gain = gainValue.getNextValue();
        for (int ch = 0; ch < audio.getNumChannels(); ++ch)
        {
            const int rc = juce::jmin(ch, channels - 1);
            const float input = audio.getSample(ch, i);
            ring.setSample(rc, writePosition, input);
            const float wet = ring.getSample(rc, r0) + frac * (ring.getSample(rc, r1) - ring.getSample(rc, r0));
            audio.setSample(ch, i, (input + wetMix * (wet - input)) * gain);
        }
        writePosition = (writePosition + 1) % length;
    }
}
