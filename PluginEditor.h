#pragma once

#include "PluginProcessor.h"
#include "PluginPresets.h"
#include <array>
#include <memory>
#include <vector>

//==============================================================================
// Palette tiree du visuel "sound system" : vert, jaune dore, noir d'encre, creme
namespace DubColours
{
    static const juce::Colour green { 0xff3f9b3d };
    static const juce::Colour greenDark { 0xff2c6e2b };
    static const juce::Colour gold  { 0xffe8b93a };
    static const juce::Colour ink   { 0xff1a1914 };
    static const juce::Colour cream { 0xfff4e8c1 };
}

//==============================================================================
struct DubLookAndFeel : public juce::LookAndFeel_V4
{
    DubLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float startAngle, float endAngle,
                           juce::Slider&) override;
};

//==============================================================================
class DubSkankSubAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DubSkankSubAudioProcessorEditor (DubSkankSubAudioProcessor&);
    ~DubSkankSubAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        juce::Slider slider { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    DubSkankSubAudioProcessor& proc;
    DubLookAndFeel laf;
    juce::Image art;

    std::vector<std::unique_ptr<Knob>> knobs;
    std::array<int, 4> sectionCount {};
    std::array<juce::String, 4> sectionTitle;
    std::array<juce::Rectangle<int>, 4> sectionBounds;

    juce::ComboBox syncBox;
    juce::Label syncLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> syncAttachment;

    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" }, nextButton { ">" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DubSkankSubAudioProcessorEditor)
};
