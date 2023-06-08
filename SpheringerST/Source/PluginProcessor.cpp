/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
// this is the constructor
SpheringerSTAudioProcessor::SpheringerSTAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true))

{
    // allows plugin to use basic audio formats, e.g. .mp3, .wav, ...
    mFormatManager.registerBasicFormats();
    // Initialize MIDI keyboard state
    keyboardState.reset();
    
    for (int i = 0; i < mNumVoices; i++)
    {
        mSampler.addVoice(new juce::SamplerVoice() );
    }
}

// this is the destructor
SpheringerSTAudioProcessor::~SpheringerSTAudioProcessor()
{
    mFormatReader = nullptr;
}

//==============================================================================
const juce::String SpheringerSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpheringerSTAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SpheringerSTAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SpheringerSTAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SpheringerSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpheringerSTAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SpheringerSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpheringerSTAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SpheringerSTAudioProcessor::getProgramName (int index)
{
    return {};
}

void SpheringerSTAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SpheringerSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // specify playback sample rate
    mSampler.setCurrentPlaybackSampleRate(sampleRate);
    
    // Update ADSR from user input, via SamplerSound built-in function
    updateADSR();
    
    // Reset volume value
    volume.reset(sampleRate, 0.02f); // ramp length in seconds: 0.02
    
}

void SpheringerSTAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpheringerSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono())
        return false;
    else
        return true;
    
}
#endif

void SpheringerSTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
        
    //std::cout << buffer.getNumChannels() << std::endl;
    
    // Read MIDI message for keyboard visualization
    keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
    
    // let the buffer do the parsing automatically
    mSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    // Add volume change from slider value input
    
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= juce::Decibels::decibelsToGain(volume.getNextValue());
            // gain in volumes
        }
    }
     
    // Clear MidiBuffer as the plugin does not have MIDI output
    midiMessages.clear();

    
    
}

//==============================================================================
bool SpheringerSTAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpheringerSTAudioProcessor::createEditor()
{
     return new SpheringerSTAudioProcessorEditor (*this);
}

//==============================================================================
void SpheringerSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SpheringerSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

// define the loadFile() function
void SpheringerSTAudioProcessor::loadFile()
{
    mSampler.clearSounds();
    // file chooser class under API
    juce::FileChooser chooser {"Please load a stereo WAV file..."};
    
    // sub-function in FileChooser(), returns bool.; also function for multiple files
    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult(); // get and store file?
        mFormatReader = mFormatManager.createReaderFor(file); // create a pointer for file?
        
        // read MIDI base number from file name
        baseNum = file.getFileNameWithoutExtension().getTrailingIntValue();
        
        // output log
        std::cout << "File loaded! File name: " << file.getFileName() << ", Base MIDI number: " << baseNum << std::endl;
    }
    
    // MIDI note numbers use BigInteger? Specify MIDI number range
    juce::BigInteger range;
    range.setRange(0, 128, true);
    
    mSampler.addSound(new juce::SamplerSound("Sample", *mFormatReader, range, baseNum, 0.1, 0.1, 10.0));
    // to reference a pointer, add a star operator
    // midi note for normnal pitch is C3 = 60, Yamaha notation
    // attack and release times: both set to 0.1 seconds for now
    // max sample length set to 10 seconds
}

/*
void SpheringerSTAudioProcessor::loadFile(const juce::String &path)
{
    // make sure old sample is cleared when new sample is loaded
    mSampler.clearSounds();
    
    auto file = juce::File(path);
    mFormatReader = mFormatManager.createReaderFor(file); // create reader
    
    // set range for MIDI keys
    juce::BigInteger range;
    range.setRange(0, 128, true);
    
    mSampler.addSound(new juce::SamplerSound("Sample", *mFormatReader, range, 60, 0.1, 0.1, 10.0));
}
*/

// updateADSR() is essentially a user-defined function that inherits the juce::ADSR class and does 2 functions:
// 1. check if the input the sound sample is a SamplerSound class object;
// 2. set ADSR parameters to the sound with the JUCE native ADSR::setEnvelopeParameters() method
// that's why it seems we have not called the actual function but juce::ADSR class is very involved thruout the process.

void SpheringerSTAudioProcessor::updateADSR()
{
    for (int i = 0; i < mSampler.getNumSounds(); ++i) // int or auto? need to declare...
    {
        if (auto sound = dynamic_cast<juce::SamplerSound*>(mSampler.getSound(i).get()))
        {
            sound->setEnvelopeParameters(mADSRParams);
        }
    }
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpheringerSTAudioProcessor();
}
