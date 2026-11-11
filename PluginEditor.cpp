#include "PluginEditor.h"
#include <cmath>

#ifdef HAS_SOUNDSYSTEM_ART
 #include <BinaryData.h>
#endif

using namespace DubColours;

//==============================================================================
DubLookAndFeel::DubLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, cream);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxHighlightColourId, gold);
    setColour (juce::Label::textColourId, cream);

    setColour (juce::ComboBox::backgroundColourId, gold);
    setColour (juce::ComboBox::textColourId, ink);
    setColour (juce::ComboBox::outlineColourId, ink);
    setColour (juce::ComboBox::arrowColourId, ink);

    setColour (juce::PopupMenu::backgroundColourId, ink);
    setColour (juce::PopupMenu::textColourId, cream);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, gold);
    setColour (juce::PopupMenu::highlightedTextColourId, ink);

    setColour (juce::TextButton::buttonColourId, gold);
    setColour (juce::TextButton::textColourOffId, ink);
    setColour (juce::TextButton::textColourOnId, ink);
}

void DubLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                                       float pos, float a0, float a1, juce::Slider&)
{
    auto b = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (3.f);
    const float d = std::min (b.getWidth(), b.getHeight());
    const auto c = b.getCentre();
    const float r = d * 0.5f;
    const float ang = a0 + pos * (a1 - a0);

    // piste + arc de valeur
    {
        juce::Path track;
        track.addCentredArc (c.x, c.y, r, r, 0.f, a0, a1, true);
        g.setColour (ink);
        g.strokePath (track, juce::PathStrokeType (5.f, juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));

        juce::Path val;
        val.addCentredArc (c.x, c.y, r, r, 0.f, a0, ang, true);
        g.setColour (cream);
        g.strokePath (val, juce::PathStrokeType (3.f, juce::PathStrokeType::curved,
                                                 juce::PathStrokeType::rounded));
    }

    // corps du bouton = cone de haut-parleur (jaune, anneaux concentriques, contour noir)
    const float rb = r - 7.f;
    g.setColour (gold);
    g.fillEllipse (c.x - rb, c.y - rb, rb * 2.f, rb * 2.f);

    g.setColour (ink.withAlpha (0.85f));
    for (float f : { 0.72f, 0.48f })
        g.drawEllipse (c.x - rb * f, c.y - rb * f, rb * 2.f * f, rb * 2.f * f, 1.6f);

    g.setColour (ink);
    g.drawEllipse (c.x - rb, c.y - rb, rb * 2.f, rb * 2.f, 2.5f);

    // aiguille
    const float len = rb * 0.95f;
    const float inner = rb * 0.28f;
    juce::Line<float> pointer (c.x + inner * std::sin (ang), c.y - inner * std::cos (ang),
                               c.x + len * std::sin (ang),   c.y - len * std::cos (ang));
    g.setColour (ink);
    g.drawLine (pointer, 3.f);
    g.fillEllipse (c.x - rb * 0.1f, c.y - rb * 0.1f, rb * 0.2f, rb * 0.2f);
}

//==============================================================================
DubSkankSubAudioProcessorEditor::DubSkankSubAudioProcessorEditor (DubSkankSubAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&laf);
    setSize (1060, 600);

#ifdef HAS_SOUNDSYSTEM_ART
    art = juce::ImageCache::getFromMemory (ART_DATA, (int) ART_SIZE);
#endif

    struct Spec { const char* id; const char* name; };
    const std::vector<std::vector<Spec>> sections
    {
        { { "skankLevel", "LEVEL" }, { "pluckMix", "AC / ELEC" }, { "skankDecay", "DECAY" },
          { "skankBright", "BRIGHT" }, { "splitNote", "SPLIT" } },
        { { "subLevel", "LEVEL" }, { "subTone", "TONE" }, { "subDrive", "DRIVE" },
          { "subTri", "TRIANGLE" }, { "subOct", "OCTAVE" }, { "subGlide", "GLIDE" },
          { "subPunch", "PUNCH" } },
        { { "delayTime", "TIME" }, { "delayFB", "FEEDBACK" }, { "delayMix", "MIX" },
          { "delayWobble", "WOBBLE" } },
        { { "reverbMix", "REVERB" }, { "reverbSize", "SIZE" }, { "master", "MASTER" } }
    };

    sectionTitle = { "SKANK", "SUB", "DUB ECHO", "SPACE / OUT" };

    for (size_t s = 0; s < sections.size(); ++s)
    {
        sectionCount[s] = (int) sections[s].size();
        for (const auto& spec : sections[s])
        {
            auto k = std::make_unique<Knob>();
            k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 72, 16);
            k->slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                                           juce::MathConstants<float>::pi * 2.8f, true);
            k->label.setText (spec.name, juce::dontSendNotification);
            k->label.setJustificationType (juce::Justification::centred);
            k->label.setFont (juce::Font (juce::FontOptions (12.f, juce::Font::bold)));
            k->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                proc.apvts, spec.id, k->slider);
            addAndMakeVisible (k->slider);
            addAndMakeVisible (k->label);
            knobs.push_back (std::move (k));
        }
    }

    syncBox.addItemList ({ "Free", "1/4", "Dotted 1/8", "1/8" }, 1);
    syncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        proc.apvts, "delaySync", syncBox);
    syncLabel.setText ("SYNC", juce::dontSendNotification);
    syncLabel.setJustificationType (juce::Justification::centred);
    syncLabel.setFont (juce::Font (juce::FontOptions (12.f, juce::Font::bold)));
    addAndMakeVisible (syncBox);
    addAndMakeVisible (syncLabel);

    // --- selecteur de presets (groupes par famille) ---
    const char* lastCategory = nullptr;
    for (int i = 0; i < kNumPresets; ++i)
    {
        if (lastCategory == nullptr || std::string (lastCategory) != kPresets[i].category)
        {
            presetBox.addSectionHeading (kPresets[i].category);
            lastCategory = kPresets[i].category;
        }
        presetBox.addItem (kPresets[i].name, i + 1);
    }
    presetBox.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();
        if (id > 0)
            proc.setCurrentProgram (id - 1);
    };

    auto step = [this] (int delta)
    {
        const int i = (proc.getCurrentProgram() + delta + kNumPresets) % kNumPresets;
        proc.setCurrentProgram (i);
        presetBox.setSelectedId (i + 1, juce::dontSendNotification);
    };
    prevButton.onClick = [step] { step (-1); };
    nextButton.onClick = [step] { step (1); };

    addAndMakeVisible (presetBox);
    addAndMakeVisible (prevButton);
    addAndMakeVisible (nextButton);
}

DubSkankSubAudioProcessorEditor::~DubSkankSubAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void DubSkankSubAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (ink);

    // --- visuel ---
    const auto artArea = juce::Rectangle<float> (16.f, 16.f, 470.f, 568.f);
    g.setColour (green);
    g.fillRoundedRectangle (artArea, 12.f);

    if (art.isValid())
    {
        g.drawImage (art, artArea.reduced (6.f), juce::RectanglePlacement::centred);
    }
    else
    {
        // repli si l'image n'est pas encore dans le projet : 4 cones de sound system
        for (int i = 0; i < 4; ++i)
        {
            const float cx = artArea.getX() + 70.f + (float) i * 110.f;
            const float cy = artArea.getCentreY();
            for (float rr : { 48.f, 36.f, 24.f, 12.f })
            {
                g.setColour (rr > 30.f ? gold : (rr > 18.f ? gold.darker (0.2f) : ink));
                g.fillEllipse (cx - rr, cy - rr, rr * 2.f, rr * 2.f);
                g.setColour (ink);
                g.drawEllipse (cx - rr, cy - rr, rr * 2.f, rr * 2.f, 2.f);
            }
        }
        g.setColour (cream);
        g.setFont (juce::Font (juce::FontOptions (13.f)));
        g.drawText ("soundsystem.png", artArea.withTrimmedTop (artArea.getHeight() - 40.f),
                    juce::Justification::centred);
    }
    g.setColour (ink);
    g.drawRoundedRectangle (artArea, 12.f, 4.f);

    // --- titre ---
    g.setColour (gold);
    g.setFont (juce::Font (juce::FontOptions (36.f, juce::Font::bold)));
    g.drawText ("DUB SKANK SUB", 500, 14, 300, 40, juce::Justification::centredLeft);
    g.setColour (cream);
    g.setFont (juce::Font (juce::FontOptions (13.f)));
    g.drawText ("reggae  /  dub  /  sound system", 502, 50, 544, 18, juce::Justification::centredLeft);

    // --- panneaux de sections ---
    for (size_t s = 0; s < sectionBounds.size(); ++s)
    {
        const auto r = sectionBounds[s].toFloat();
        g.setColour (green);
        g.fillRoundedRectangle (r, 10.f);
        g.setColour (ink);
        g.drawRoundedRectangle (r, 10.f, 3.f);

        const juce::Font tf (juce::FontOptions (13.f, juce::Font::bold));
        const int tw = juce::GlyphArrangement::getStringWidthInt (tf, sectionTitle[s]) + 20;
        const auto tab = juce::Rectangle<float> (r.getX() + 10.f, r.getY() - 1.f, (float) tw, 22.f);
        g.setColour (gold);
        g.fillRoundedRectangle (tab, 6.f);
        g.setColour (ink);
        g.drawRoundedRectangle (tab, 6.f, 2.f);
        g.setFont (tf);
        g.drawText (sectionTitle[s], tab.toNearestInt(), juce::Justification::centred);
    }
}

void DubSkankSubAudioProcessorEditor::resized()
{
    prevButton.setBounds (800, 20, 28, 26);
    presetBox.setBounds (832, 20, 180, 26);
    nextButton.setBounds (1016, 20, 28, 26);

    const int x0 = 500, colW = 76, secH = 116, gap = 8;
    int y = 76;
    size_t k = 0;

    for (size_t s = 0; s < sectionBounds.size(); ++s)
    {
        sectionBounds[s] = { x0, y, 544, secH };
        const int cellY = y + 26;

        int col = 0;
        if (s == 2)   // la 1re case du delay est le selecteur de sync
        {
            const int cx = x0 + 6;
            syncLabel.setBounds (cx, cellY, colW, 14);
            syncBox.setBounds (cx + 4, cellY + 30, colW - 8, 26);
            col = 1;
        }

        for (int i = 0; i < sectionCount[s]; ++i, ++col, ++k)
        {
            const int cx = x0 + 6 + col * colW;
            knobs[k]->label.setBounds (cx, cellY, colW, 14);
            knobs[k]->slider.setBounds (cx, cellY + 14, colW, 78);
        }

        y += secH + gap;
    }
}
