cmake_minimum_required(VERSION 3.22)
project(DubSkankSub VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(DubSkankSub
    COMPANY_NAME "DubLab"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE Dblb
    PLUGIN_CODE Dsks
    FORMATS VST3 Standalone
    PRODUCT_NAME "Dub Skank Sub"
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(DubSkankSub)

target_sources(DubSkankSub PRIVATE
    PluginProcessor.cpp
    PluginProcessor.h
    PluginEditor.cpp
    PluginEditor.h
    PluginPresets.h)

# Visuel "sound system" : depose soundsystem.png (ou .jpg) a cote de ce fichier
foreach(ext png jpg jpeg)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/soundsystem.${ext}")
        juce_add_binary_data(DubSkankSubAssets SOURCES soundsystem.${ext})
        target_link_libraries(DubSkankSub PRIVATE DubSkankSubAssets)
        target_compile_definitions(DubSkankSub PUBLIC
            HAS_SOUNDSYSTEM_ART=1
            ART_DATA=BinaryData::soundsystem_${ext}
            ART_SIZE=BinaryData::soundsystem_${ext}Size)
        break()
    endif()
endforeach()

target_compile_definitions(DubSkankSub PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(DubSkankSub
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
