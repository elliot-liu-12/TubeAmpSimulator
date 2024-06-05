/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //must add a listener to the button
    waveshaperBypassButton.addListener(this);
    EqBypassButton.addListener(this);
    allBypassButton.addListener(this);
    startTimerHz(120);
    setSize (400, 500);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    waveshaperBypassButton.setButtonText("Waveshaping Toggle");
    waveshaperBypassButton.setClickingTogglesState(true);
    waveshaperBypassButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
    //off color is just buttonColourId and is the default
    waveshaperBypassButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::green);
    addAndMakeVisible(waveshaperBypassButton);

    EqBypassButton.setButtonText("EQ Toggle");
    EqBypassButton.setClickingTogglesState(true);
    EqBypassButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
    EqBypassButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::green);
    addAndMakeVisible(EqBypassButton);

    allBypassButton.setButtonText("Bypass All");
    allBypassButton.setClickingTogglesState(true);
    allBypassButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    allBypassButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::orange);
    addAndMakeVisible(allBypassButton);
    
    //paint analyser
    addAndMakeVisible(analyser);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int width = getWidth();
    int height = getHeight();
    //x coordinate is leftmost pixel
    analyser.setBounds(0.1 * width, 0.05 * height, 0.8 * width, 0.25 * height);
    waveshaperBypassButton.setBounds(0.1 * width, 0.325 * height, 0.8 * width, 0.2 * height);
    EqBypassButton.setBounds(0.1 * width, 0.525 * height, 0.8 * width, 0.2 * height);
    allBypassButton.setBounds(0.1 * width, 0.725 * height, 0.8 * width, 0.2 * height);
}

//==============================================================================
void NewProjectAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    //must confirm that the address of the button clicked is correct
    if (button == &waveshaperBypassButton)
    {
        audioProcessor.toggleBypassWaveshaper();
    }
    else if (button == &EqBypassButton)
    {
        audioProcessor.toggleBypassEQ();
    }

    else
    {
        audioProcessor.toggleBypassAll();
        if (audioProcessor.allBypassed == true)
        {
            waveshaperBypassButton.setToggleState(true, false);
            EqBypassButton.setToggleState(true, false);
        }
        else
        {
            waveshaperBypassButton.setToggleState(false, false);
            EqBypassButton.setToggleState(false, false);
        }
        repaint();
    }
}

//==============================================================================
void NewProjectAudioProcessorEditor::timerCallback()
{
    //only draw if both buffers are full
    if (audioProcessor.rawBufferFull && audioProcessor.procBufferFull)
    {
        if (!EqBypassButton.getToggleState() || !waveshaperBypassButton.getToggleState())
            analyser.drawProcLine = true;
        else
        {
            analyser.drawProcLine = false;
        }
        analyser.drawNextFrameOfSpectrum(audioProcessor.rawfftData, audioProcessor.procfftData);
        audioProcessor.rawBufferFull = false;
        audioProcessor.procBufferFull = false;
        analyser.repaint();
    }
}