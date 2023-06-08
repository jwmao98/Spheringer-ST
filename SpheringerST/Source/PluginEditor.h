/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.
    20230317: Plugin editor handles the UI side of the plugin.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
// FileDragAndDropTarget and Slider::Listener are an abstract class: just inherit its functions
class SpheringerSTAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         //public juce::FileDragAndDropTarget,
                                         public juce::Slider::Listener,
                                         public juce::MidiKeyboardState::Listener
{
public:
    SpheringerSTAudioProcessorEditor (SpheringerSTAudioProcessor&);
    ~SpheringerSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    // virtual functions MUST be specified for this class before building
    // override for independent implementation
    //bool isInterestedInFileDrag (const juce::StringArray& files) override;
    //void filesDropped (const juce::StringArray& files, int x, int y) override;
    
    // pure virtual function must be implemented before the code can compile
    // override means refering to existing functions
    // slider is referring to a pointer
    void sliderValueChanged(juce::Slider* slider) override;
    
    // specify pure virtual methods as juce::MidiKeyboardState::Listener is an abstract class
    void handleNoteOn (juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

private:
    
    // Create a text button for loading samples
    juce::TextButton mLoadButton {"Please load an audio file..."};
    
    // Create 4 rotary sliders for ADSR envelope customization
    // Create 4 labels for these sliders
    // can be declared all on the same line 
    juce::Slider mAttackSlider, mDecaySlider, mSustainSlider, mReleaseSlider;
    juce::Label mAttackLabel, mDecayLabel, mSustainLabel, mReleaseLabel;
    
    // Create output volume slider
    juce::Slider mVolumeSlider;
    juce::Label mVolumeLabel;
    
    // Create MIDI keyboard visualization
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;

    
    // GUI constants
    static const int MARGIN = 4, MAX_WINDOW_HEIGHT = 800, MAX_WINDOW_WIDTH = 1200 + 2 * MARGIN,
    MAX_KEYB_WIDTH = 1200, MAX_KEYB_HEIGHT = 82, BUTTON_WIDTH = 50, BUTTON_HEIGHT = 30;
    //Colour backgroundColor { 44,54,60 }; // stock bckgrd colour
    
    SpheringerSTAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpheringerSTAudioProcessorEditor)
};
