#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

class DubSkankSubAudioProcessor : public juce::AudioProcessor
{
public:
    DubSkankSubAudioProcessor();
    ~DubSkankSubAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Dub Skank Sub"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    //==============================================================================
    struct Params
    {
        float skankLevel, pluckMix, skankDecay, skankBright;
        float subLevel, subTone, subDrive, subTri, subOct, subGlide, subPunch;
        int   splitNote;
        int   delaySync;
        float delayTime, delayFB, delayMix, delayWobble;
        float reverbMix, reverbSize, master;
        // derived (depend on sample rate)
        float subGlideCoef, subLpCoef;
    };

    //==============================================================================
    // Skank : Karplus-Strong (corde pincee, cote acoustique)
    //         + impulsion carree filtree (cote electrique)
    struct SkankVoice
    {
        bool active = false, released = false;
        int note = 0;
        uint32_t age = 0;

        std::vector<float> buf;
        int len = 2, pos = 0;
        float loss = 0.997f;

        float env = 0.f, envCoef = 0.999f, relCoef = 0.99f, vel = 1.f;
        float phase = 0.f, phaseInc = 0.f, lp = 0.f;

        void trigger (int n, float v, double sr, const Params& p, juce::Random& rng, uint32_t a);
        float next (const Params& p);
    };

    //==============================================================================
    // Sub : sinus + triangle + sous-octave, glide, "punch" de pitch, drive, passe-bas
    struct SubVoice
    {
        bool active = false, gate = false;
        int note = -1;
        float freq = 40.f, target = 40.f, env = 0.f, pitchEnv = 0.f, vel = 1.f;
        double ph = 0.0, ph2 = 0.0;
        float lp1 = 0.f, lp2 = 0.f;
        float pitchCoef = 0.999f, relCoef = 0.999f, atkInc = 0.01f;

        void prepare (double sr);
        void noteOn (int n, float v);
        void noteOff (int n);
        float next (const Params& p, double sr);
    };

    //==============================================================================
    // Delay a bande : feedback filtre + sature, derive de pitch (wobble)
    struct TapeDelay
    {
        std::vector<float> buf;
        int w = 0;
        float smoothed = 0.f, lp = 0.f, hpLp = 0.f, lfo = 0.f;

        void prepare (double sr);
        float process (float in, float targetSamples, float fb, float wobble,
                       float lfoInc, float sr, float lpA, float hpA);
    };

    //==============================================================================
    Params readParams() const;
    void handleMidi (const juce::MidiMessage& m, const Params& p);
    void triggerSkank (int note, float vel, const Params& p);
    void renderVoices (int start, int end, const Params& p);
    void resizeScratch (int n);

    std::array<SkankVoice, 8> skank;
    SubVoice sub;
    TapeDelay delayL, delayR;
    juce::Reverb reverb;
    juce::Random rng;
    uint32_t voiceCounter = 0;
    int currentProgram = 0;
    double sampleRate = 44100.0;

    std::vector<float> skankBus, subBus, dOutL, dOutR, revL, revR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DubSkankSubAudioProcessor)
};
