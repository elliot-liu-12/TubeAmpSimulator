/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif //m
    )
#endif
{
        waveshaper.functionToUse = [](float x)
    {
        float result = (1.5 * x) - 0.5*std::pow(x, 3);
        return result;
    };
        
   leftChain.get<0>().coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(48000, 80, 0.75, 1.2);
   rightChain.get<0>().coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(48000, 80, 0.75, 1.2);
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    int lastSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = lastSampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    waveshaper.prepare(spec);
    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    double sampleRate = getSampleRate();

    juce::dsp::AudioBlock<float> audioBlock(buffer);
    auto leftBlock = audioBlock.getSingleChannelBlock(0);
    auto rightBlock = audioBlock.getSingleChannelBlock(1);
    
    //save raw audio - only left side for now
    const float* rawReadPtr = buffer.getReadPointer(0);
    const int numRawSamples = buffer.getNumSamples();
    bufferRawAudio(rawReadPtr, numRawSamples);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);


    waveshaper.process(leftContext);
    waveshaper.process(rightContext);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    //save processed audio - only left side for now
    const float* procReadPtr = buffer.getReadPointer(0);
    const int numProcSamples = buffer.getNumSamples();
    bufferProcAudio(procReadPtr, numProcSamples);
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

void NewProjectAudioProcessor::bufferRawAudio(const float* readPtr, const int numSamples) noexcept
{
    for (int i = 0; i < numSamples; i++)
    {
        //only display raw for now
        if (rawFifoIndex == fftSize)
        {
            if (!rawBufferFull)
            {
                juce::zeromem(rawfftData, sizeof(rawfftData));
                memcpy(rawfftData, rawFifo, sizeof(rawFifo));
                rawBufferFull = true;
            }
            rawFifoIndex = 0;
        }
        float currSample = readPtr[i];
        //post-increment operator increments index after setting current value to currSample
        rawFifo[rawFifoIndex++] = currSample;
    }
}

void NewProjectAudioProcessor::bufferProcAudio(const float* readPtr, const int numSamples) noexcept
{
    for (int i = 0; i < numSamples; i++)
    {
        //only display raw for now
        if (procFifoIndex == fftSize)
        {
            if (!procBufferFull)
            {
                juce::zeromem(procfftData, sizeof(procfftData));
                memcpy(procfftData, procFifo, sizeof(procFifo));
                procBufferFull = true;
            }
            procFifoIndex = 0;
        }
        float currSample = readPtr[i];
        //post-increment operator increments index after setting current value to currSample
        procFifo[procFifoIndex++] = currSample;
    }
}

void NewProjectAudioProcessor::toggleBypassAll()
{
    //if at least one filter is bypassed, unbypass all of them
    if (EqBypassed == true || waveshaperBypassed == true)
    {
        waveshaper.functionToUse = [](float x)
        {
            float result = (1.5 * x) - 0.5 * std::pow(x, 3);
            return result;
        };

        leftChain.setBypassed<0>(false);
        rightChain.setBypassed<0>(false);

        waveshaperBypassed = false;
        EqBypassed = false;
        allBypassed = false;
    }
    //if no filter is bypassed, bypass them both
    else
    {
        if (waveshaperBypassed == true)
        {
            waveshaper.functionToUse = [](float x) {
                return x;
            };
        }

        leftChain.setBypassed<0>(true);
        rightChain.setBypassed<0>(true);

        waveshaperBypassed = true;
        EqBypassed = true;
        allBypassed = true;
    }
}

void NewProjectAudioProcessor::toggleBypassEQ()
{
    if (EqBypassed == false)
    {
        leftChain.setBypassed<0>(true);
        rightChain.setBypassed<0>(true);
    }
    else
    {
        leftChain.setBypassed<0>(false);
        rightChain.setBypassed<0>(false);
    }

    EqBypassed = !EqBypassed;
}

void NewProjectAudioProcessor::toggleBypassWaveshaper()
{
    if (waveshaperBypassed == false)
    {
        waveshaper.functionToUse = [](float x) {
            return x;
        };
    }
    else
    {
        waveshaper.functionToUse = [](float x)
        {
            float result = (1.5 * x) - 0.5 * std::pow(x, 3);
            return result;
        };
    }

    waveshaperBypassed = !waveshaperBypassed;
}
//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}