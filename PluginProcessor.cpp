#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginPresets.h"
#include <cmath>

//==============================================================================
static juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    using FP = juce::AudioParameterFloat;
    using NR = juce::NormalisableRange<float>;
    juce::AudioProcessorValueTreeState::ParameterLayout l;

    auto addF = [&] (const char* id, const char* name, float lo, float hi, float def, float skew = 1.f)
    {
        l.add (std::make_unique<FP> (juce::ParameterID { id, 1 }, name, NR (lo, hi, 0.f, skew), def));
    };

    // Skank
    addF ("skankLevel",  "Skank Level",            0.f,   1.f,    0.7f);
    addF ("pluckMix",    "Skank Acoustic-Electric", 0.f,   1.f,    0.6f);
    addF ("skankDecay",  "Skank Decay ms",         30.f,  800.f,  140.f, 0.5f);
    addF ("skankBright", "Skank Brightness",       0.f,   1.f,    0.6f);

    // Sub
    addF ("subLevel",    "Sub Level",              0.f,   1.f,    0.8f);
    addF ("subTone",     "Sub Tone Hz",            60.f,  2000.f, 350.f, 0.4f);
    addF ("subDrive",    "Sub Drive",              0.f,   1.f,    0.3f);
    addF ("subTri",      "Sub Sine-Triangle",      0.f,   1.f,    0.3f);
    addF ("subOct",      "Sub Octave Down",        0.f,   1.f,    0.25f);
    addF ("subGlide",    "Sub Glide ms",           0.f,   300.f,  30.f, 0.5f);
    addF ("subPunch",    "Sub Punch",              0.f,   1.f,    0.35f);

    l.add (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { "splitNote", 1 },
                                                      "Split Note (sub below)", 24, 96, 52));

    // Dub FX
    l.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { "delaySync", 1 },
                                                         "Delay Sync",
                                                         juce::StringArray { "Free", "1/4", "Dotted 1/8", "1/8" }, 2));
    addF ("delayTime",   "Delay Time ms",          20.f,  1500.f, 450.f, 0.5f);
    addF ("delayFB",     "Delay Feedback",         0.f,   1.1f,   0.55f);
    addF ("delayMix",    "Delay Mix",              0.f,   1.f,    0.4f);
    addF ("delayWobble", "Delay Tape Wobble",      0.f,   1.f,    0.25f);
    addF ("reverbMix",   "Reverb Mix",             0.f,   1.f,    0.2f);
    addF ("reverbSize",  "Reverb Size",            0.f,   1.f,    0.6f);
    addF ("master",      "Master",                 0.f,   1.5f,   0.8f);

    return l;
}

//==============================================================================
void DubSkankSubAudioProcessor::SkankVoice::trigger (int n, float v, double sr, const Params& p,
                                                     juce::Random& rng, uint32_t a)
{
    active = true;
    released = false;
    note = n;
    vel = v;
    age = a;

    const float f = 440.f * std::pow (2.f, (float) (n - 69) / 12.f);
    len = juce::jlimit (2, (int) buf.size(), (int) (sr / f + 0.5));
    pos = 0;

    // burst de bruit adouci : plus "brightness" est haut, plus l'attaque est claire
    const float a_ = 0.25f + 0.75f * p.skankBright;
    float prev = 0.f;
    for (int i = 0; i < len; ++i)
    {
        const float r = rng.nextFloat() * 2.f - 1.f;
        prev += a_ * (r - prev);
        buf[(size_t) i] = prev * 1.6f * v;
    }
    loss = 0.985f + 0.014f * p.skankBright;

    env = 1.f;
    const float dSamp = std::max (1.f, 0.001f * p.skankDecay * (float) sr);
    envCoef = std::pow (0.01f, 1.f / dSamp);
    relCoef = std::pow (0.01f, 1.f / (0.035f * (float) sr));

    phase = 0.f;
    phaseInc = f / (float) sr;
    lp = 0.f;
}

float DubSkankSubAudioProcessor::SkankVoice::next (const Params& p)
{
    if (! active)
        return 0.f;

    // Karplus-Strong
    const float x = buf[(size_t) pos];
    int nx = pos + 1;
    if (nx >= len) nx = 0;
    buf[(size_t) pos] = 0.5f * (x + buf[(size_t) nx]) * loss;
    pos = nx;

    // pulse electrique : carre dont le filtre se ferme avec l'enveloppe
    phase += phaseInc;
    if (phase >= 1.f) phase -= 1.f;
    const float sq = phase < 0.5f ? 1.f : -1.f;
    const float a = juce::jlimit (0.01f, 0.95f, p.skankBright * (0.12f + 0.9f * env));
    lp += a * (sq - lp);

    const float out = (p.pluckMix * x * 1.4f + (1.f - p.pluckMix) * lp * 0.5f)
                      * env * (0.4f + 0.6f * vel);

    env *= envCoef;
    if (released) env *= relCoef;
    if (env < 0.0005f) active = false;

    return out;
}

//==============================================================================
void DubSkankSubAudioProcessor::SubVoice::prepare (double sr)
{
    *this = SubVoice();
    pitchCoef = std::exp (-1.f / (0.04f * (float) sr));
    relCoef   = std::exp (-1.f / (0.12f * (float) sr));
    atkInc    = 1.f / (0.004f * (float) sr);
}

void DubSkankSubAudioProcessor::SubVoice::noteOn (int n, float v)
{
    target = 440.f * std::pow (2.f, (float) (n - 69) / 12.f);
    if (! active || env < 0.02f)
        freq = target;          // sinon : glide depuis la note precedente
    active = true;
    gate = true;
    note = n;
    vel = 0.4f + 0.6f * v;
    pitchEnv = 1.f;
}

void DubSkankSubAudioProcessor::SubVoice::noteOff (int n)
{
    if (gate && n == note)
        gate = false;
}

float DubSkankSubAudioProcessor::SubVoice::next (const Params& p, double sr)
{
    if (! active)
        return 0.f;

    const float twoPi = 6.2831853f;

    freq += p.subGlideCoef * (target - freq);
    const float f = freq * (1.f + p.subPunch * pitchEnv * 2.f);
    pitchEnv *= pitchCoef;

    ph += (double) f / sr;
    ph -= std::floor (ph);
    ph2 += (double) f * 0.5 / sr;
    ph2 -= std::floor (ph2);

    const float s = std::sin (twoPi * (float) ph);
    float x = (float) ph - 0.25f + 1.f;
    x -= std::floor (x);
    const float t = 4.f * std::abs (x - 0.5f) - 1.f;      // triangle aligne sur le sinus

    float sig = s * (1.f - p.subTri) + t * p.subTri;
    sig += p.subOct * std::sin (twoPi * (float) ph2);
    sig *= 0.8f;

    const float g = 1.f + p.subDrive * 8.f;
    sig = std::tanh (sig * g) * 0.9f;

    lp1 += p.subLpCoef * (sig - lp1);
    lp2 += p.subLpCoef * (lp1 - lp2);

    if (gate) env = std::min (1.f, env + atkInc);
    else      env *= relCoef;
    if (! gate && env < 0.0003f) active = false;

    return lp2 * env * vel;
}

//==============================================================================
void DubSkankSubAudioProcessor::TapeDelay::prepare (double sr)
{
    buf.assign ((size_t) (sr * 3.2) + 8, 0.f);
    w = 0;
    smoothed = (float) (sr * 0.4);
    lp = hpLp = lfo = 0.f;
}

float DubSkankSubAudioProcessor::TapeDelay::process (float in, float targetSamples, float fb,
                                                     float wobble, float lfoInc, float sr,
                                                     float lpA, float hpA)
{
    const int size = (int) buf.size();
    targetSamples = juce::jlimit (32.f, (float) (size - 8), targetSamples);

    // le temps de delay glisse lentement : effet "bande" quand on le change
    smoothed += 0.0004f * (targetSamples - smoothed);

    lfo += lfoInc;
    if (lfo >= 1.f) lfo -= 1.f;
    const float mod = wobble * std::sin (6.2831853f * lfo) * sr * 0.0006f;

    float rp = (float) w - (smoothed + mod);
    while (rp < 0.f) rp += (float) size;
    int i0 = (int) rp;
    const float fr = rp - (float) i0;
    i0 %= size;
    const int i1 = (i0 + 1) % size;
    const float y = buf[(size_t) i0] * (1.f - fr) + buf[(size_t) i1] * fr;

    // feedback : passe-bas (~2.8 kHz) + passe-haut (~140 Hz) + saturation
    lp   += lpA * (y - lp);
    hpLp += hpA * (lp - hpLp);
    const float fbSig = lp - hpLp;

    buf[(size_t) w] = in + fb * std::tanh (fbSig);
    w = (w + 1) % size;

    return y;
}

//==============================================================================
DubSkankSubAudioProcessor::DubSkankSubAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
}

bool DubSkankSubAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::stereo() || out == juce::AudioChannelSet::mono();
}

void DubSkankSubAudioProcessor::resizeScratch (int n)
{
    for (auto* v : { &skankBus, &subBus, &dOutL, &dOutR, &revL, &revR })
        v->assign ((size_t) n, 0.f);
}

void DubSkankSubAudioProcessor::prepareToPlay (double sr, int samplesPerBlock)
{
    sampleRate = sr;

    for (auto& v : skank)
    {
        v.buf.assign ((size_t) (sr / 30.0) + 8, 0.f);
        v.active = false;
    }

    sub.prepare (sr);
    delayL.prepare (sr);
    delayR.prepare (sr);
    reverb.setSampleRate (sr);
    reverb.reset();
    resizeScratch (std::max (1, samplesPerBlock));
}

DubSkankSubAudioProcessor::Params DubSkankSubAudioProcessor::readParams() const
{
    auto get = [this] (const char* id) { return apvts.getRawParameterValue (id)->load(); };

    Params p {};
    p.skankLevel  = get ("skankLevel");
    p.pluckMix    = get ("pluckMix");
    p.skankDecay  = get ("skankDecay");
    p.skankBright = get ("skankBright");
    p.subLevel    = get ("subLevel");
    p.subTone     = get ("subTone");
    p.subDrive    = get ("subDrive");
    p.subTri      = get ("subTri");
    p.subOct      = get ("subOct");
    p.subGlide    = get ("subGlide");
    p.subPunch    = get ("subPunch");
    p.splitNote   = (int) get ("splitNote");
    p.delaySync   = (int) get ("delaySync");
    p.delayTime   = get ("delayTime");
    p.delayFB     = get ("delayFB");
    p.delayMix    = get ("delayMix");
    p.delayWobble = get ("delayWobble");
    p.reverbMix   = get ("reverbMix");
    p.reverbSize  = get ("reverbSize");
    p.master      = get ("master");

    const float sr = (float) sampleRate;
    p.subGlideCoef = p.subGlide <= 0.5f ? 1.f : 1.f - std::exp (-1.f / (0.001f * p.subGlide * sr));
    p.subLpCoef    = 1.f - std::exp (-6.2831853f * p.subTone / sr);
    return p;
}

void DubSkankSubAudioProcessor::triggerSkank (int note, float vel, const Params& p)
{
    SkankVoice* target = nullptr;
    for (auto& v : skank)
        if (! v.active) { target = &v; break; }

    if (target == nullptr)
    {
        target = &skank[0];
        for (auto& v : skank)
            if (v.age < target->age) target = &v;
    }

    target->trigger (note, vel, sampleRate, p, rng, ++voiceCounter);
}

void DubSkankSubAudioProcessor::handleMidi (const juce::MidiMessage& m, const Params& p)
{
    if (m.isNoteOn())
    {
        const int n = m.getNoteNumber();
        const float v = m.getFloatVelocity();
        if (n < p.splitNote) sub.noteOn (n, v);
        else                 triggerSkank (n, v, p);
    }
    else if (m.isNoteOff())
    {
        const int n = m.getNoteNumber();
        sub.noteOff (n);
        for (auto& s : skank)
            if (s.active && s.note == n)
                s.released = true;
    }
    else if (m.isAllNotesOff() || m.isAllSoundOff())
    {
        sub.gate = false;
        for (auto& s : skank)
            s.released = true;
    }
}

void DubSkankSubAudioProcessor::renderVoices (int start, int end, const Params& p)
{
    for (int i = start; i < end; ++i)
    {
        float s = 0.f;
        for (auto& v : skank)
            if (v.active)
                s += v.next (p);

        skankBus[(size_t) i] = s * p.skankLevel;
        subBus[(size_t) i]   = sub.next (p, sampleRate) * p.subLevel;
    }
}

void DubSkankSubAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    const int n = buffer.getNumSamples();
    const int numCh = buffer.getNumChannels();
    buffer.clear();

    if (n > (int) skankBus.size())
        resizeScratch (n);

    const Params p = readParams();

    double bpm = 90.0;
    if (auto* head = getPlayHead())
        if (auto posInfo = head->getPosition())
            if (auto b = posInfo->getBpm())
                if (*b > 20.0)
                    bpm = *b;

    // --- MIDI + voix (sample-accurate) ---
    int pos = 0;
    for (const auto meta : midi)
    {
        const int t = juce::jlimit (pos, n, meta.samplePosition);
        renderVoices (pos, t, p);
        pos = t;
        handleMidi (meta.getMessage(), p);
    }
    renderVoices (pos, n, p);

    // --- FX dub sur le skank uniquement (le sub reste sec et propre) ---
    const float sr = (float) sampleRate;
    const float beatMs = (float) (60000.0 / bpm);
    float tL = p.delayTime;
    if      (p.delaySync == 1) tL = beatMs;
    else if (p.delaySync == 2) tL = beatMs * 0.75f;
    else if (p.delaySync == 3) tL = beatMs * 0.5f;
    tL *= 0.001f * sr;
    const float tR = tL * 1.02f;

    const float lpA = 1.f - std::exp (-6.2831853f * 2800.f / sr);
    const float hpA = 1.f - std::exp (-6.2831853f * 140.f / sr);
    const float lfoInc = 0.7f / sr;

    for (int i = 0; i < n; ++i)
    {
        const float dry = skankBus[(size_t) i];
        const float wl = delayL.process (dry, tL, p.delayFB, p.delayWobble, lfoInc, sr, lpA, hpA);
        const float wr = delayR.process (dry, tR, p.delayFB, p.delayWobble, lfoInc, sr, lpA, hpA);
        dOutL[(size_t) i] = wl;
        dOutR[(size_t) i] = wr;
        revL[(size_t) i] = dry + 0.5f * wl;
        revR[(size_t) i] = dry + 0.5f * wr;
    }

    juce::Reverb::Parameters rp;
    rp.roomSize = p.reverbSize;
    rp.damping = 0.55f;
    rp.wetLevel = 0.33f;
    rp.dryLevel = 0.f;
    rp.width = 1.f;
    rp.freezeMode = 0.f;
    reverb.setParameters (rp);
    reverb.processStereo (revL.data(), revR.data(), n);

    // --- sortie ---
    auto* L = buffer.getWritePointer (0);
    auto* R = numCh > 1 ? buffer.getWritePointer (1) : nullptr;

    for (int i = 0; i < n; ++i)
    {
        const float s = subBus[(size_t) i];
        const float dry = skankBus[(size_t) i];
        float l = dry + s + dOutL[(size_t) i] * p.delayMix + revL[(size_t) i] * p.reverbMix;
        float r = dry + s + dOutR[(size_t) i] * p.delayMix + revR[(size_t) i] * p.reverbMix;

        l = std::tanh (l) * p.master;      // securite anti-saturation numerique
        r = std::tanh (r) * p.master;

        L[i] = l;
        if (R != nullptr) R[i] = r;
        else L[i] = 0.5f * (l + r);
    }
}

int DubSkankSubAudioProcessor::getNumPrograms()
{
    return kNumPresets;
}

const juce::String DubSkankSubAudioProcessor::getProgramName (int index)
{
    if (index < 0 || index >= kNumPresets)
        return {};
    return juce::String (kPresets[index].category) + " - " + kPresets[index].name;
}

void DubSkankSubAudioProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= kNumPresets)
        return;

    currentProgram = index;
    const auto& pr = kPresets[index];

    auto set = [this] (const char* id, float v)
    {
        if (auto* prm = apvts.getParameter (id))
            prm->setValueNotifyingHost (prm->convertTo0to1 (v));
    };

    set ("skankLevel",  pr.skankLevel);
    set ("pluckMix",    pr.pluckMix);
    set ("skankDecay",  pr.skankDecay);
    set ("skankBright", pr.skankBright);
    set ("subLevel",    pr.subLevel);
    set ("subTone",     pr.subTone);
    set ("subDrive",    pr.subDrive);
    set ("subTri",      pr.subTri);
    set ("subOct",      pr.subOct);
    set ("subGlide",    pr.subGlide);
    set ("subPunch",    pr.subPunch);
    set ("splitNote",   (float) pr.splitNote);
    set ("delaySync",   (float) pr.delaySync);
    set ("delayTime",   pr.delayTime);
    set ("delayFB",     pr.delayFB);
    set ("delayMix",    pr.delayMix);
    set ("delayWobble", pr.delayWobble);
    set ("reverbMix",   pr.reverbMix);
    set ("reverbSize",  pr.reverbSize);
    set ("master",      pr.master);
}

juce::AudioProcessorEditor* DubSkankSubAudioProcessor::createEditor()
{
    return new DubSkankSubAudioProcessorEditor (*this);
}

void DubSkankSubAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void DubSkankSubAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DubSkankSubAudioProcessor();
}
