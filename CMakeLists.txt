cmake_minimum_required(VERSION 3.22)
project(StepperStutter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(JUCE_WEB_BROWSER OFF CACHE BOOL "" FORCE)
set(JUCE_USE_CURL OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(StepperStutter
    COMPANY_NAME "Pixel Circuit Audio"
    BUNDLE_ID "audio.pixelcircuit.stepperstutter"
    PLUGIN_MANUFACTURER_CODE PxCA
    PLUGIN_CODE StSt
    PRODUCT_NAME "STEPPER STUTTER"
    FORMATS VST3 Standalone
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(StepperStutter)
target_sources(StepperStutter PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/HalfTimeEngine.cpp
    Source/StutterEngine.cpp
    Source/PixelLookAndFeel.cpp)
target_include_directories(StepperStutter PRIVATE Source)
target_compile_definitions(StepperStutter PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(StepperStutter PRIVATE
    juce::juce_audio_utils juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)
