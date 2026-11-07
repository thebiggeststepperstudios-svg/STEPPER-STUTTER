#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
std::unique_ptr<juce::AudioParameterFloat> floatParam(const char* id, const char* name, float lo, float hi, float def, juce::String suffix = {})
{ return std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { id, 1 }, name, juce::NormalisableRange<float>(lo, hi), def, suffix); }
}

juce::AudioProcessorValueTreeState::ParameterLayout StepperStutterAudioProcessor::createParameterLayout()
{
    using Choice = juce::AudioParameterChoice;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "halfOn", 1 }, "Half-Time On", false));
    p.push_back(std::make_unique<Choice>(juce::ParameterID { "halfSpeed", 1 }, "Half-Time Speed", juce::StringArray { "1/2", "1/4" }, 0));
    p.push_back(floatParam("glide", "Glide", 2, 250, 35, " ms"));
    p.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "pitchDrop", 1 }, "Pitch Drop", true));
    p.push_back(floatParam("halfMix", "Half-Time Mix", 0, 1, 1));
    p.push_back(floatParam("halfGain", "Half-Time Output", -24, 12, 0, " dB"));
    p.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "stutterOn", 1 }, "Stutter On", false));
    p.push_back(std::make_unique<Choice>(juce::ParameterID { "rate", 1 }, "Stutter Rate", juce::StringArray { "1/4", "1/8", "1/16", "1/32", "1/4T", "1/8T", "1/4.", "1/8." }, 2));
    p.push_back(floatParam("gate", "Gate", 0.05f, 1, 1));
    p.push_back(floatParam("decay", "Decay", -12, 6, 0, " dB/repeat"));
    p.push_back(floatParam("pitch", "Pitch", -12, 12, 0, " st"));
    p.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "reverse", 1 }, "Reverse", false));
    p.push_back(floatParam("probability", "Probability", 0, 1, 1));
    p.push_back(floatParam("stutterMix", "Stutter Mix", 0, 1, 1));
    p.push_back(floatParam("stutterGain", "Stutter Output", -24, 12, 0, " dB"));
    p.push_back(std::make_unique<Choice>(juce::ParameterID { "order", 1 }, "Order", juce::StringArray { "HALF > STUTTER", "STUTTER > HALF" }, 0));
    p.push_back(floatParam("globalMix", "Global Mix", 0, 1, 1));
    p.push_back(floatParam("master", "Master Output", -24, 12, 0, " dB"));
    p.push_back(floatParam("bpm", "Manual BPM", 30, 300, 120, " BPM"));
    return { p.begin(), p.end() };
}

StepperStutterAudioProcessor::StepperStutterAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

bool StepperStutterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet(), out = layouts.getMainOutputChannelSet();
    return in == out && (in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo());
}

void StepperStutterAudioProcessor::prepareToPlay(double sr, int maxBlock)
{
    sampleRateHz = sr; const int channels = getTotalNumOutputChannels();
    dryBuffer.setSize(channels, juce::jmax(1, maxBlock)); stageBuffer.setSize(channels, juce::jmax(1, maxBlock));
    halfTime.prepare(sr, maxBlock, channels); stutter.prepare(sr, maxBlock, channels);
}
void StepperStutterAudioProcessor::releaseResources() {}

void StepperStutterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int n = buffer.getNumSamples(), chs = getTotalNumOutputChannels();
    for (int ch = getTotalNumInputChannels(); ch < chs; ++ch) buffer.clear(ch, 0, n);
    double bpm = parameters.getRawParameterValue("bpm")->load();
    if (auto* playhead = getPlayHead())
        if (auto pos = playhead->getPosition())
            if (auto hostTempo = pos->getBpm()) bpm = *hostTempo;
    const bool halfOn = parameters.getRawParameterValue("halfOn")->load() > 0.5f;
    const bool stutterOn = parameters.getRawParameterValue("stutterOn")->load() > 0.5f;
    const auto val = [this](const char* id) { return parameters.getRawParameterValue(id)->load(); };
    const int halfSpeed = (int) std::round(val("halfSpeed"));
    const int rateIndex = juce::jlimit(0, 7, (int) std::round(val("rate") * 7.0f));
    const bool halfFirst = val("order") < 0.5f;
    const float globalMix = val("globalMix"), masterGain = juce::Decibels::decibelsToGain(val("master"));
    float peak = 0.0f;
    const int capacity = juce::jmax(1, dryBuffer.getNumSamples());
    for (int offset = 0; offset < n; offset += capacity)
    {
        const int count = juce::jmin(capacity, n - offset);
        float* chunkData[2] {};
        for (int ch = 0; ch < chs; ++ch) chunkData[ch] = buffer.getWritePointer(ch, offset);
        juce::AudioBuffer<float> chunk(chunkData, chs, count);
        for (int ch = 0; ch < chs; ++ch) dryBuffer.copyFrom(ch, 0, buffer, ch, offset, count);
        const auto runHalf = [&] { halfTime.process(chunk, (float) bpm, halfOn, halfSpeed, val("glide"), val("pitchDrop") > 0.5f, val("halfMix"), val("halfGain")); };
        const auto runStutter = [&] { stutter.process(chunk, (float) bpm, stutterOn, rateIndex, val("gate"), val("decay"), val("pitch"), val("reverse") > 0.5f, val("probability"), val("stutterMix"), val("stutterGain")); };
        if (halfFirst) { runHalf(); runStutter(); } else { runStutter(); runHalf(); }
        for (int ch = 0; ch < chs; ++ch)
            for (int i = 0; i < count; ++i)
        {
                const float dry = dryBuffer.getSample(ch, i);
                const float y = dry + globalMix * (chunk.getSample(ch, i) - dry);
                chunk.setSample(ch, i, y * masterGain); peak = juce::jmax(peak, std::abs(y));
            }
        }
    meterLevel.store(peak, std::memory_order_relaxed);
    beatPulse.store((float) std::fmod(juce::Time::getMillisecondCounterHiRes(), 60000.0 / bpm) / (60000.0 / bpm), std::memory_order_relaxed);
}

juce::AudioProcessorEditor* StepperStutterAudioProcessor::createEditor() { return new StepperStutterAudioProcessorEditor(*this); }
void StepperStutterAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{ auto state = parameters.copyState(); std::unique_ptr<juce::XmlElement> xml(state.createXml()); copyXmlToBinary(*xml, dest); }
void StepperStutterAudioProcessor::setStateInformation(const void* data, int size)
{ std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size)); if (xml && xml->hasTagName(parameters.state.getType())) parameters.replaceState(juce::ValueTree::fromXml(*xml)); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new StepperStutterAudioProcessor(); }
