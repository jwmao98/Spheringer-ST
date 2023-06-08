/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpheringerSTAudioProcessorEditor::SpheringerSTAudioProcessorEditor (SpheringerSTAudioProcessor& p)
: AudioProcessorEditor (&p),
keyboardComponent(p.keyboardState,juce::MidiKeyboardComponent::horizontalKeyboard),
audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    // click button to load file; reference processor
    mLoadButton.onClick = [&]() { audioProcessor.loadFile(); }; // on click execute function in clause
    addAndMakeVisible(mLoadButton); // make button visible
    
    // Link audio processor to keyboard state Make MIDI keyboard visible
    p.keyboardState.addListener(this);
    addAndMakeVisible(keyboardComponent);

    
    // Add ADSR sliders to the interface
    
    // Attack
    // Call and define SliderStyle, an enum sub-class of Slider class?
    mAttackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    // Add text box (value) under the slider
    mAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20); // text box content is fixed, cannot re-enter value by double click and can only change value by dragging the slider vertical dial
    // Set range of the slider
    mAttackSlider.setRange(0.01f, 2.0f, 0.01f); // min, max, increment
    mAttackSlider.setDoubleClickReturnValue(true, 0.1f); // default: 0.1s
    // Add the audioProcessorEditor as a listener: monitors value change
    mAttackSlider.addListener(this);
    addAndMakeVisible(mAttackSlider); // make slider visible and add to child component of editor
    
    // Decay
    mDecaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDecaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    mDecaySlider.setRange(0.01f, 2.0f, 0.01f);
    mDecaySlider.setDoubleClickReturnValue(true, 0.1f); // default: 0.1s
    mDecaySlider.addListener(this);
    addAndMakeVisible(mDecaySlider);
    
    // Sustain
    mSustainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mSustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    mSustainSlider.setRange(0.01f, 10.0f, 0.01f);
    mSustainSlider.setDoubleClickReturnValue(true, 1.0f); // default: 1.0s
    mSustainSlider.addListener(this);
    addAndMakeVisible(mSustainSlider);
    
    // Release
    mReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    mReleaseSlider.setRange(0.01f, 5.0f, 0.01f);
    mReleaseSlider.setDoubleClickReturnValue(true, 0.1f); // default: 0.1s
    mReleaseSlider.addListener(this);
    //change color
    //mReleaseSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::red); // can also input integer for color id
    addAndMakeVisible(mReleaseSlider);
    
    // Add volume slider
    mVolumeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    mVolumeSlider.setRange(-20.0f, 20.0f, 0.1f); // in dB
    mVolumeSlider.setDoubleClickReturnValue(true, 0.0f); // double click to return to detente/neutral position
    mVolumeSlider.addListener(this);
    addAndMakeVisible(mVolumeSlider);
    
    ////////////// Font and UI =================================================================
    
    // Set label texts for the sliders
    setSize (600, 400); // UI window  background size
    
    const auto fontSize = 10.0f;
    
    // Attack
    mAttackLabel.setFont(fontSize); // set font size
    mAttackLabel.setText("Attack (s)", juce::NotificationType::dontSendNotification); // set text content
    mAttackLabel.setJustificationType(juce::Justification::centredTop); // set text alignment
    mAttackLabel.attachToComponent(&mAttackSlider, false); // attach label to slider pointer; boolean onLeft = false as we need to set on centred top
    
    // Decay
    mDecayLabel.setFont(fontSize);
    mDecayLabel.setText("Decay (s)", juce::NotificationType::dontSendNotification);
    mDecayLabel.setJustificationType(juce::Justification::centredTop);
    mDecayLabel.attachToComponent(&mDecaySlider, false);
    
    // Sustain
    mSustainLabel.setFont(fontSize);
    mSustainLabel.setText("Sustain (s)", juce::NotificationType::dontSendNotification);
    mSustainLabel.setJustificationType(juce::Justification::centredTop);
    mSustainLabel.attachToComponent(&mSustainSlider, false);
    
    // Release
    mReleaseLabel.setFont(fontSize);
    mReleaseLabel.setText("Release (s)", juce::NotificationType::dontSendNotification);
    mReleaseLabel.setJustificationType(juce::Justification::centredTop);
    mReleaseLabel.attachToComponent(&mReleaseSlider, false);
    
    // Volume
    mVolumeLabel.setFont(fontSize);
    mVolumeLabel.setText("Volume (dB)", juce::NotificationType::dontSendNotification);
    mVolumeLabel.setJustificationType(juce::Justification::centredTop);
    mVolumeLabel.attachToComponent(&mVolumeSlider, false);
    
    // on volume value change
    mVolumeSlider.onValueChange = [this]()
    {
        audioProcessor.volume.setTargetValue(mVolumeSlider.getValue());
    };
    
}

//

SpheringerSTAudioProcessorEditor::~SpheringerSTAudioProcessorEditor()
{
}

//==============================================================================
void SpheringerSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    
    
    /*======================================
    // The following is the GUI setup for file drag and drop, uncomment to use
    // start with a black background
    g.fillAll(juce::Colours::black);
    
    g.setColour(juce::Colours::white);
    g.setFont(15.0f); // set font height to 15 pts
    
    // if file dragged in: detect and show "sound loaded"
    if (audioProcessor.getNumSamplerSounds() > 0)
    {
        // set new background color
        g.fillAll(juce::Colours::green);
        
        // set comment text
        g.drawText("Sound loaded :P", getWidth()/2 - 50, getHeight()/2 - 10, 100, 20, juce::Justification::centred);
        // set text in the center with width 10 and height 20
    
    }
    // else: instruct user to load a sound
    else
    {
        g.drawText("Please load a sound file...", getWidth()/2 - 50, getHeight()/2 - 10, 100, 20, juce::Justification::centred);
    }
     
     ========================================*/
     
     
}

void SpheringerSTAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // Set button size and position
    mLoadButton.setBounds(getWidth()/2 - 100, getHeight()/3 - 30, 200, 60);
    
    // Set MIDI keyboard bounds
    juce::Rectangle<int> r = getLocalBounds();
    float resizedKeybWidth = r.getWidth() - MARGIN * 2, resizedKeybHeight = r.getHeight() - 5;
    float keybWidth = resizedKeybWidth > MAX_KEYB_WIDTH ? MAX_KEYB_WIDTH : resizedKeybWidth;
    float keybHeight = resizedKeybHeight > MAX_KEYB_HEIGHT ? MAX_KEYB_HEIGHT : resizedKeybHeight;
    keyboardComponent.setBounds (MARGIN, MARGIN, keybWidth, keybHeight);
    
    // Add ADSR sliders to window and set positions
    // positions are relative values proportional to the physical window length
    // current window size is 600*400
    const auto startX = 0.4f;
    const auto startY = 0.7f;
    const auto dialWidth = 0.15f;
    const auto dialHeight = 0.225f;
    
    mAttackSlider.setBoundsRelative(startX, startY, dialWidth, dialHeight); // proportional x,y, height, width
    mDecaySlider.setBoundsRelative(startX + dialWidth, startY, dialWidth, dialHeight);
    mSustainSlider.setBoundsRelative(startX + dialWidth*2, startY, dialWidth, dialHeight);
    mReleaseSlider.setBoundsRelative(startX + dialWidth*3 , startY, dialWidth, dialHeight);
    
    // Set volume slider position
    const auto startXX = 0.2f;
    mVolumeSlider.setBoundsRelative(startXX , startY, dialWidth, dialHeight);
    

}
/*
bool SpheringerSTAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray &files)
{
    // check if dropped files are audio files: check on file name suffix
    // for loop: for a *file* in the input array *files*
    for (auto file : files)
    {
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif"))
        {
            return true;
        }
    }
    
    return false;
}

void SpheringerSTAudioProcessorEditor::filesDropped(const juce::StringArray &files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            // if returns true, is an audio file: load this file
            audioProcessor.loadFile(file);
        }
    }
    
    repaint(); // dunno what's this for tho...
}
*/
// =========================================================

// pure virtual methods for the Listener classes are re-defined here...

void SpheringerSTAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &mAttackSlider)
    {
        audioProcessor.getADSRParams().attack = mAttackSlider.getValue();
    }
    else if (slider == &mDecaySlider)
    {
        audioProcessor.getADSRParams().decay = mDecaySlider.getValue();
    }
    else if (slider == &mSustainSlider)
    {
        audioProcessor.getADSRParams().sustain = mSustainSlider.getValue();
    }
    else if (slider == &mReleaseSlider)
    {
        audioProcessor.getADSRParams().release = mReleaseSlider.getValue();
    }
    
    // Update ADSR upon user input
    audioProcessor.updateADSR();
}

void SpheringerSTAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

void SpheringerSTAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

