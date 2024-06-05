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
class NewProjectAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    NewProjectAudioProcessor();
    ~NewProjectAudioProcessor();

    //==============================================================================
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;
    void toggleBypassEQ();
    void toggleBypassWaveshaper();
    void toggleBypassAll();
    void bufferRawAudio(const float* readptr, const int numSamples) noexcept;
    void bufferProcAudio(const float* readptr, const int numSamples) noexcept;
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
    bool allBypassed = false;

    //add constants later
    enum
    {
        fftOrder = 11,             // [1]
        fftSize = 1 << fftOrder,  // [2]
        scopeSize = 512             // [3]
    };

    float rawFifo[fftSize];
    float procFifo[fftSize];
    float rawfftData[fftSize * 2];
    float procfftData[fftSize * 2];
    int rawFifoIndex = 0;
    int procFifoIndex = 0;
    bool rawBufferFull = false;
    bool procBufferFull = false;

private:
    bool waveshaperBypassed = false;
    bool EqBypassed = false;
    juce::dsp::WaveShaper<float> waveshaper;
    juce::dsp::ProcessorChain <juce::dsp::IIR::Filter<float>> leftChain;
    juce::dsp::ProcessorChain <juce::dsp::IIR::Filter<float>> rightChain;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)
};
