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
    setSize (400, 300);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    waveshaperBypassButton.setButtonText("Waveshaping Bypass");
    waveshaperBypassButton.setClickingTogglesState(true);
    waveshaperBypassButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
    //off color is just buttonColourId and is the default
    waveshaperBypassButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::green);
    addAndMakeVisible(waveshaperBypassButton);

    EqBypassButton.setButtonText("EQ Bypass");
    EqBypassButton.setClickingTogglesState(true);
    EqBypassButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
    EqBypassButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::green);
    addAndMakeVisible(EqBypassButton);

    allBypassButton.setButtonText("Bypass All");
    allBypassButton.setClickingTogglesState(true);
    allBypassButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    allBypassButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::orange);
    addAndMakeVisible(allBypassButton);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int width = getWidth();
    int height = getHeight();
    //x coordinate is leftmost pixel
    waveshaperBypassButton.setBounds(0.1 * width, 0, 0.8 * width, 0.25 * height);
    EqBypassButton.setBounds(0.1 * width, 0.25 * height, 0.8 * width, 0.25 * height);
    allBypassButton.setBounds(0.1 * width, 0.5 * height, 0.8 * width, 0.25 * height);
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