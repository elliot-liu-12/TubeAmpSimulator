#include <JuceHeader.h>
// SPECTRUM ANALYZER
class AnalyserComponent : public juce::Component
{
public:
    AnalyserComponent() : forwardFFT(fftOrder),
        window(fftSize, juce::dsp::WindowingFunction<float>::rectangular)
    {
        setOpaque(true);
    }
    void paint(juce::Graphics& g) override
    {
        //fill background
        g.fillAll(juce::Colours::black);
        drawFrame(g);
    }

    void drawNextFrameOfSpectrum(float* rawfftData, float* procfftData)
    {
        //apply windowing function to data
        window.multiplyWithWindowingTable(rawfftData, fftSize);
        window.multiplyWithWindowingTable(procfftData, fftSize);
        //render FFT for data
        forwardFFT.performFrequencyOnlyForwardTransform(rawfftData);
        forwardFFT.performFrequencyOnlyForwardTransform(procfftData);

        float mindB = -100.0f;
        float maxdB = -0.0f;

        for (int i = 0; i < scopeSize; i++)
        {
            float skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
            int fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
            float rawLevel = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(rawfftData[fftDataIndex])
                - juce::Decibels::gainToDecibels((float)fftSize)),
                mindB, maxdB, 0.0f, 1.0f);
            float procLevel = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(procfftData[fftDataIndex])
                - juce::Decibels::gainToDecibels((float)fftSize)),
                mindB, maxdB, 0.0f, 1.0f);
            rawScopeData[i] = rawLevel;
            procScopeData[i] = procLevel;
        }
    }

    void drawFrame(juce::Graphics& g)
    {
  

        for (int i = 1; i < scopeSize; i++)
        {
            auto width = getLocalBounds().getWidth();
            auto height = getLocalBounds().getHeight();

            //draw raw
            g.setColour(juce::Colours::white);
            g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
                                juce::jmap(rawScopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
                        (float)juce::jmap(i, 0, scopeSize - 1, 0, width),
                                juce::jmap(rawScopeData[i], 0.0f, 1.0f, (float)height, 0.0f) });
            //draw processed if necessary
            if (drawProcLine)
            {
                g.setColour(juce::Colours::orange);
                g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
                        juce::jmap(procScopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
                (float)juce::jmap(i, 0, scopeSize - 1, 0, width),
                        juce::jmap(procScopeData[i], 0.0f, 1.0f, (float)height, 0.0f) });
            }

        }
    }
    enum
    {
        fftOrder = 11,
        fftSize = 1 << fftOrder,
        scopeSize = 2048
    };
    bool drawProcLine = true;
private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    float rawScopeData[scopeSize];
    float procScopeData[scopeSize];
    bool nextFFTBlockReady = false;
};
