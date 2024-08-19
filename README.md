# Description

Tube Amp Simulator (TAS) is a VST plugin built with the JUCE framework that creates a warmer sound by simulates the sound of a tube amplifier by applying a waveshaping function and slightly tweaking the frequency response. 

![TAS interface](\images\interface.png)
# Getting Started
You can find TAS as both an executable and a VST in the releases section. Only use the VST if you want to run TAS within a DAW. Since TAS is designed to modify system audio, which JUCE does not support natively, **you will have to run TAS through  a virtual output** like [Voicemeeter](https://vb-audio.com/Voicemeeter/). You can also run it in a digital audio workstation (DAW) by loading the VST file as you would any other plugin. 

After installing Voicemeeter, set your sound output device to "Voicemeeter Input". This will route all of your system audio to Voicemeeter so that it can be redirected to TAS. Voicemeeter should look like the below image, but you should not be able to hear anything.
![Voicemeeter interface](\images\voicemeeter.png)

Open TAS and click on the "Options" menu in the top right corner of the window. Set the input to Voicemeeter Out B1 and the output to your audio device. If the first checkbox is checked, uncheck it. If you hear crackling or static, change the sample rate to that of your desired audio device.

Example settings:
![Voicemeeter settings](\images\settings.png)

If you want to build TAS yourself, you need a [copy](https://juce.com/get-juce/) of JUCE. Run Projucer.exe and select Basic under the Plug-In header. Expand the Modules section and select the `juce_dsp` module. Replace the files in the `Source` directory of the project with the TAS files. To debug, you can open the solution files in the `Builds` directory.

## Flowchart
![Audio path flowchart](\images\flowchart.png)

# Usage

TAS implements two features, a waveshaping function and an EQ. You can toggle these individually by clicking on their respective toggles, which will be green if they are on and red if they are off. You can also toggle both of these effects at once with the "Bypass All" button.

The orange visualizer is the processed audio, while the white visualizer is the raw input. If both effects are off, the orange visualizer will disappear because the processed and raw audio are identical.

![TAS interface](\images\interface.png)

# Technical Details
The waveshaping function is y = 1.5x - 0.5x^3, and the EQ is a 1.2 dB peak filter at 80 Hz with a Q of 0.75.

![Waveshaping function](\images\desmos-graph.png)
Graph of waveshaping function