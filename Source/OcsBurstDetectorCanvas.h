#ifndef OCS_BURST_DETECTOR_CANVAS_H_INCLUDED
#define OCS_BURST_DETECTOR_CANVAS_H_INCLUDED

#include <VisualizerWindowHeaders.h>
#include "OcsBurstDetectorEditor.h"
#include "OcsBurstDetector.h"

class RadioButtonLookAndFeel : public LookAndFeel_V2
{
    void drawTickBox(Graphics& g, Component& component, float x, float y, float w, float h,
        const bool ticked, const bool isEnabled,
        const bool isMouseOverButton, const bool isButtonDown) override;
};


class VerticalGroupSet : public Component
{
public:
    VerticalGroupSet(Colour backgroundColor = Colours::silver);
    VerticalGroupSet(const String& componentName, Colour backgroundColor = Colours::silver);
    ~VerticalGroupSet();

    void addGroup(std::initializer_list<Component*> components);

private:
    Colour bgColor;
    int leftBound;
    int rightBound;
    OwnedArray<DrawableRectangle> groups;
    static const int PADDING = 5;
    static const int CORNER_SIZE = 8;
};


class OcsBurstDetectorCanvas : public Visualizer,
    public ComboBox::Listener,
    public Label::Listener,
    public Button::Listener
{
public:
    OcsBurstDetectorCanvas(GenericProcessor* n);
    ~OcsBurstDetectorCanvas();

    void refreshState() override;
    void update() override;
    void refresh() override;

    void paint(Graphics& g) override;
    void resized() override;

    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    void labelTextChanged(Label* labelThatHasChanged) override;
    void buttonClicked(Button* button) override;

private:
    ScopedPointer<Viewport> viewport;

    OcsBurstDetector* processor;
    OcsBurstDetectorEditor* editor;
    Label* createEditable(const String& name, const String& initialValue,
        const String& tooltip, juce::Rectangle<int> bounds);
    void initializeOptionsPanel();

    static bool updateIntLabel(Label* label, int min, int max,
        int defaultValue, int* out);
    static bool updateFloatLabel(Label* label, float min, float max,
        float defaultValue, float* out);

    RadioButtonLookAndFeel rbLookAndFeel;

    ScopedPointer<Component> optionsPanel;
    ScopedPointer<Label> optionsPanelTitle;

    /****** threshold section ******/
    ScopedPointer<Label> thresholdTitle;
    const static int threshRadioId = 1;
    ScopedPointer<VerticalGroupSet> thresholdGroupSet;

    // threshold constant
    ScopedPointer<ToggleButton> constantThreshButton;
    ScopedPointer<Label> constantThreshValue;

    // threshold auto
    ScopedPointer<ToggleButton> autoThreshButton;
    ScopedPointer<Label> stdAutoThreshEditable;
    ScopedPointer<Label> stdAutoThreshLabel;


    /****** detect section ******/
    ScopedPointer<Label> detectTitle;
    ScopedPointer<VerticalGroupSet> detectGroupSet;

    // bandpass filter
    ScopedPointer<ToggleButton> bandpassButton;
    ScopedPointer<Label> lowCutBandpassLabel;
    ScopedPointer<Label> lowCutBandpassEditable;
    ScopedPointer<Label> highCutBandpassLabel;
    ScopedPointer<Label> highCutBandpassEditable;

    // minus average
    ScopedPointer<ToggleButton> minusAverageButton;

    // smooth
    ScopedPointer<ToggleButton> smoothButton;
    ScopedPointer<Label> smoothKEditable;

    // rfs
    ScopedPointer<Label> rfsLabel;
    ScopedPointer<Label> rfsEditable;

    // sdft
    ScopedPointer<Label> sdftLabel;
    ScopedPointer<Label> windowTypeSdftLabel;
    ScopedPointer<ComboBox> windowTypeSdftBox;
    ScopedPointer<Label> windowSizeSdftLabel;
    ScopedPointer<Label> windowSizeSdftEditable;


    /****** output section ******/
    ScopedPointer<Label> outputTitle;
    ScopedPointer<VerticalGroupSet> outputGroupSet;
    
    // random delay
    ScopedPointer<ToggleButton> delayButton;
    ScopedPointer<Label> minDelayLabel;
    ScopedPointer<Label> minDelayEditable;
    ScopedPointer<Label> maxDelayLabel;
    ScopedPointer<Label> maxDelayEditable;

    // duration time
    ScopedPointer<Label> durationLabel;
    ScopedPointer<Label> durationEditable;

    // light duration
    ScopedPointer<Label> lightDurLabel;
    ScopedPointer<Label> lightDurEditable;

    // ignore duration
    ScopedPointer<Label> ignoreDurLabel;
    ScopedPointer<Label> ignoreDurEditable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OcsBurstDetectorCanvas);
};


#endif


