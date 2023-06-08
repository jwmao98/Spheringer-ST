/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SpheringerSTAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SpheringerSTAudioProcessor();
    ~SpheringerSTAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // create a load file function for the button
    void loadFile();
    int baseNum = 60; // default to central C in case the file does not have MIDI num tag
    // another load file function for drag n drop
    // input: take file path (string)
    //void loadFile(const juce::String& path);
    
    // create a method/getter to detect if sound is loaded and the number of sounds
    int getNumSamplerSounds()
    {
        return mSampler.getNumSounds();
    }
    
    void updateADSR();
    
    // mADSRParams is private but can read with swgetParameters()
    juce::ADSR::Parameters& getADSRParams()
    {
        return mADSRParams; // reference to private object via pointer
    }
    
    // Volume value
    juce::SmoothedValue<float> volume {0.0f};
    
    // declare keyboard state as public to associate w processor
    juce::MidiKeyboardState keyboardState;

private:
    juce::Synthesiser mSampler; // if not using namespace, remember to add juce::
    const int mNumVoices {16}; // not that much is needed but put in the capacity all the same or it will clip
    
    // Create an ADSR class project for storing parameters
    juce::ADSR::Parameters mADSRParams;
    
    // audio format manager classs
    juce::AudioFormatManager mFormatManager;
    
    // audio format reader, as a pointer
    // start as null pointer: when choosing another file, we don't want to delete the original pointer, just point to somewhere else
    juce::AudioFormatReader* mFormatReader {nullptr};
    
   
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpheringerSTAudioProcessor)
};
