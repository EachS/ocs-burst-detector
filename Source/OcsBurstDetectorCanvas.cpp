/*
------------------------------------------------------------------

This file is part of a plugin for the Open Ephys GUI
Copyright (C) 2022 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "OcsBurstDetectorCanvas.h"



/* ---------------------  RadioButtonLookAndFeel  ------------------------ */
void RadioButtonLookAndFeel::drawTickBox(Graphics& g, Component& component,
    float x, float y, float w, float h, const bool ticked, const bool isEnabled,
    const bool isMouseOverButton, const bool isButtonDown)
{
    // call base function with ticked = false
    LookAndFeel_V2::drawTickBox(g, component, x, y, w, h, false, isEnabled, isMouseOverButton, isButtonDown);

    if (ticked)
    {
        // draw black circle
        const float boxSize = w * 0.7f;
        const juce::Rectangle<float> glassSphereBounds(x, y + (h - boxSize) * 0.5f, boxSize, boxSize);
        const float tickSize = boxSize * 0.55f;

        g.setColour(component.findColour(isEnabled ? ToggleButton::tickColourId
            : ToggleButton::tickDisabledColourId));
        g.fillEllipse(glassSphereBounds.withSizeKeepingCentre(tickSize, tickSize));
    }
}



/* ---------------------  VerticalGroupSet  ------------------------ */
VerticalGroupSet::VerticalGroupSet(Colour backgroundColor)
    : Component()
    , bgColor(backgroundColor)
    , leftBound(INT_MAX)
    , rightBound(INT_MIN)
{}

VerticalGroupSet::VerticalGroupSet(const String& componentName, Colour backgroundColor)
    : Component(componentName)
    , bgColor(backgroundColor)
    , leftBound(INT_MAX)
    , rightBound(INT_MIN)
{}

VerticalGroupSet::~VerticalGroupSet() {}

void VerticalGroupSet::addGroup(std::initializer_list<Component*> components)
{
    if (!getParentComponent())
    {
        jassertfalse;
        return;
    }

    DrawableRectangle* thisGroup;
    groups.add(thisGroup = new DrawableRectangle);
    addChildComponent(thisGroup);
    const Point<float> cornerSize(CORNER_SIZE, CORNER_SIZE);
    thisGroup->setCornerSize(cornerSize);
    thisGroup->setFill(bgColor);

    int topBound = INT_MAX;
    int bottomBound = INT_MIN;
    for (auto component : components)
    {
        Component* componentParent = component->getParentComponent();
        if (!componentParent)
        {
            jassertfalse;
            return;
        }
        int width = component->getWidth();
        int height = component->getHeight();
        juce::Point<int> positionFromItsParent = component->getPosition();
        juce::Point<int> localPosition = getLocalPoint(componentParent, positionFromItsParent);

        // update bounds
        leftBound = jmin(leftBound, localPosition.x - PADDING);
        rightBound = jmax(rightBound, localPosition.x + width + PADDING);
        topBound = jmin(topBound, localPosition.y - PADDING);
        bottomBound = jmax(bottomBound, localPosition.y + height + PADDING);
    }

    // set new background's bounds
    auto bounds = juce::Rectangle<float>::leftTopRightBottom(leftBound, topBound, rightBound, bottomBound);
    thisGroup->setRectangle(bounds);
    thisGroup->setVisible(true);

    // update all other children
    for (DrawableRectangle* group : groups)
    {
        if (group == thisGroup) { continue; }

        topBound = group->getPosition().y;
        bottomBound = topBound + group->getHeight();
        bounds = juce::Rectangle<float>::leftTopRightBottom(leftBound, topBound, rightBound, bottomBound);
        group->setRectangle(bounds);
    }
}



/* ---------------------  OcsBurstDetectorCanvas  ------------------------ */
OcsBurstDetectorCanvas::OcsBurstDetectorCanvas(GenericProcessor* p)
{
	processor = static_cast<OcsBurstDetector*>(p);
	editor = static_cast<OcsBurstDetectorEditor*>(processor->getEditor());
	initializeOptionsPanel();
	viewport = new Viewport();
	viewport->setViewedComponent(optionsPanel, false);
	viewport->setScrollBarsShown(true, true);
	addAndMakeVisible(viewport);
}

OcsBurstDetectorCanvas::~OcsBurstDetectorCanvas() {}

void OcsBurstDetectorCanvas::refreshState() {}

void OcsBurstDetectorCanvas::update()
{
}

void OcsBurstDetectorCanvas::refresh() {}

void OcsBurstDetectorCanvas::paint(Graphics& g)
{
	ColourGradient editorBg = editor->getBackgroundGradient();
	g.fillAll(editorBg.getColourAtPosition(0.5));
}

void OcsBurstDetectorCanvas::resized()
{
	viewport->setBounds(0, 0, getWidth(), getHeight());
}

void OcsBurstDetectorCanvas::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == windowTypeSdftBox) {
        processor->getParameter("sdft_window_type")->setNextValue(windowTypeSdftBox->getSelectedId());
    }
}

void OcsBurstDetectorCanvas::labelTextChanged(Label* labelThatHasChanged)
{
    float newValFloat, prevValFloat;
    int newValInt, prevValInt;
    if (labelThatHasChanged == constantThreshValue) {
        prevValFloat = (float)processor->getParameter("fix_threshold")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.0f, 100.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("fix_threshold")->setNextValue(newValFloat);
        }
    }
    else if (labelThatHasChanged == stdAutoThreshEditable) {
        prevValFloat = (float)processor->getParameter("Auto_STD_TH")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.0f, 100.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("Auto_STD_TH")->setNextValue(newValFloat);
        }
    }
    else if (labelThatHasChanged == lowCutBandpassEditable) {
        prevValFloat = (float)processor->getParameter("low_cut")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.1f, 15000.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("low_cut")->setNextValue(newValFloat);
        }
    }
    else if (labelThatHasChanged == highCutBandpassEditable) {
        prevValFloat = (float)processor->getParameter("high_cut")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.1f, 15000.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("high_cut")->setNextValue(newValFloat);
        }
    }
    else if (labelThatHasChanged == smoothKEditable) {
        prevValFloat = (float)processor->getParameter("smooth_K")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.001f, 1.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("smooth_K")->setNextValue(newValFloat);
        }
    }
    else if (labelThatHasChanged == rfsEditable) {
        prevValInt = (int)processor->getParameter("rfs")->getValue();
        if (updateIntLabel(labelThatHasChanged, 1, 44100, prevValInt, &newValInt))
        {
            processor->getParameter("rfs")->setNextValue(newValInt);
        }
    }
    else if (labelThatHasChanged == windowSizeSdftEditable) {
        prevValFloat = (float)processor->getParameter("sdft_window_size")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.1f, 10.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("sdft_window_size")->setNextValue(newValFloat);
        }
    }
    else if (labelThatHasChanged == minDelayEditable) {
        prevValFloat = (float)processor->getParameter("delay_min")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.0f, 10.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("delay_min")->setNextValue(newValFloat);
        }
    }
    else if (labelThatHasChanged == maxDelayEditable) {
        prevValFloat = (float)processor->getParameter("delay_max")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.0f, 10.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("delay_max")->setNextValue(newValFloat);
        }
    }
    else if (labelThatHasChanged == durationEditable) {
        prevValFloat = (float)processor->getParameter("duration_time")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.001f, 10.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("duration_time")->setNextValue(newValFloat);
        }
    }
    else if (labelThatHasChanged == lightDurEditable) {
        prevValFloat = (float)processor->getParameter("light_duration_time")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.001f, 10.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("light_duration_time")->setNextValue(newValFloat);
        }
    }
    else if (labelThatHasChanged == ignoreDurEditable) {
        prevValFloat = (float)processor->getParameter("ignore_duration_time")->getValue();
        if (updateFloatLabel(labelThatHasChanged, 0.001f, 10.0f, prevValFloat, &newValFloat))
        {
            processor->getParameter("ignore_duration_time")->setNextValue(newValFloat);
        }
    }
}

void OcsBurstDetectorCanvas::buttonClicked(Button* button)
{
    bool on = button->getToggleState();
    if (button == constantThreshButton) {
        constantThreshValue->setEnabled(on);
        if (on) {
            stdAutoThreshEditable->setEnabled(false);
            processor->getParameter("threshold_type")->setNextValue(ThresholdType::CONSTANT);
        }
    }
    else if (button == autoThreshButton) {
        stdAutoThreshEditable->setEnabled(on);
        if (on) {
            constantThreshValue->setEnabled(false);
            processor->getParameter("threshold_type")->setNextValue(ThresholdType::AUTO);
        }
    }
    else if (button == bandpassButton) {
        lowCutBandpassEditable->setEnabled(on);
        highCutBandpassEditable->setEnabled(on);
        processor->getParameter("use_bandpassfilter")->setNextValue(on);
    }
    else if (button == minusAverageButton) {
        processor->getParameter("use_minus_Average")->setNextValue(on);
    }
    else if (button == smoothButton) {
        smoothKEditable->setEnabled(on);
        processor->getParameter("use_smooth")->setNextValue(on);
    }
    else if (button == delayButton) {
        minDelayEditable->setEnabled(on);
        maxDelayEditable->setEnabled(on);
        processor->getParameter("use_delay")->setNextValue(on);
    }
}

Label* OcsBurstDetectorCanvas::createEditable(const String& name, const String& initialValue, const String& tooltip, juce::Rectangle<int> bounds)
{
    Label* editable = new Label(name, initialValue);
    editable->setEditable(true);
    editable->addListener(this);
    editable->setBounds(bounds);
    editable->setColour(Label::backgroundColourId, Colours::grey);
    editable->setColour(Label::textColourId, Colours::white);
    if (tooltip.length() > 0)
    {
        editable->setTooltip(tooltip);
    }
    return editable;
}

void OcsBurstDetectorCanvas::initializeOptionsPanel()
{
	optionsPanel = new Component("CD Options Panel");
	juce::Rectangle<int> opBounds(0, 0, 1, 1);
	const int C_TEXT_HT = 25;
	const int LEFT_EDGE = 30;
	const int TAB_WIDTH = 25;

	juce::Rectangle<int> bounds;
	int xPos = LEFT_EDGE;
	int yPos = 15;

	optionsPanelTitle = new Label("CDOptionsTitle", "Ocs Burst Detector Additional Settings");
	optionsPanelTitle->setBounds(bounds = { xPos, yPos, 300, 50 });
	optionsPanelTitle->setFont(Font("Fira Sans", "Bold", 20.0f));
	optionsPanel->addAndMakeVisible(optionsPanelTitle);
	opBounds = opBounds.getUnion(bounds);

	Font subtitleFont("Fira Sans", "Bold", 16.0f);

    /* ****************  Threshold Type  **************** */

    thresholdGroupSet = new VerticalGroupSet("Threshold controls");
    optionsPanel->addAndMakeVisible(thresholdGroupSet, 0);

    xPos = LEFT_EDGE;
    yPos += 45;

    thresholdTitle = new Label("ThresholdTitle", "Threshold type");
    thresholdTitle->setBounds(bounds = { xPos, yPos, 200, 50 });
    thresholdTitle->setFont(subtitleFont);
    optionsPanel->addAndMakeVisible(thresholdTitle);
    opBounds = opBounds.getUnion(bounds);

    /* -------- Constant threshold --------- */

    xPos = LEFT_EDGE + TAB_WIDTH;
    yPos += 45;

    constantThreshButton = new ToggleButton("Constant Threshold: ");
    constantThreshButton->setLookAndFeel(&rbLookAndFeel);
    constantThreshButton->setRadioGroupId(threshRadioId, dontSendNotification);
    constantThreshButton->setBounds(bounds = { xPos, yPos, 160, C_TEXT_HT });
    constantThreshButton->setToggleState((int)processor->getParameter("threshold_type")->getValue() == ThresholdType::CONSTANT,
        dontSendNotification);
    constantThreshButton->setTooltip("Use a constant threshold");
    constantThreshButton->addListener(this);
    optionsPanel->addAndMakeVisible(constantThreshButton);
    opBounds = opBounds.getUnion(bounds);

    constantThreshValue = createEditable("ConstantThresholdValue", String((float)processor->getParameter("fix_threshold")->getValue()),
        "Constant threshold voltage", bounds = { xPos += 160, yPos, 50, C_TEXT_HT });
    constantThreshValue->setEnabled(constantThreshButton->getToggleState());
    optionsPanel->addAndMakeVisible(constantThreshValue);
    opBounds = opBounds.getUnion(bounds);

    thresholdGroupSet->addGroup({ constantThreshButton, constantThreshValue });

    /* -------- Auto threshold --------- */

    xPos = LEFT_EDGE + TAB_WIDTH;
    yPos += 40;

    autoThreshButton = new ToggleButton("Auto Threshold: 1 mean + ");
    autoThreshButton->setLookAndFeel(&rbLookAndFeel);
    autoThreshButton->setRadioGroupId(threshRadioId, dontSendNotification);
    autoThreshButton->setBounds(bounds = { xPos, yPos, 200, C_TEXT_HT });
    autoThreshButton->setToggleState((int)processor->getParameter("threshold_type")->getValue() == ThresholdType::AUTO,
        dontSendNotification);
    autoThreshButton->setTooltip("Use a auto threshold");
    autoThreshButton->addListener(this);
    optionsPanel->addAndMakeVisible(autoThreshButton);
    opBounds = opBounds.getUnion(bounds);

    stdAutoThreshEditable = createEditable("AutoThresholdStd", String((float)processor->getParameter("Auto_STD_TH")->getValue()),
        "auto threshold N std", bounds = { xPos += 210, yPos, 50, C_TEXT_HT });
    stdAutoThreshEditable->setEnabled(constantThreshButton->getToggleState());
    optionsPanel->addAndMakeVisible(stdAutoThreshEditable);
    opBounds = opBounds.getUnion(bounds);

    stdAutoThreshLabel = new Label("AutoThresholdStd", "std");
    stdAutoThreshLabel->setBounds(bounds = { xPos += 50, yPos, 30, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(stdAutoThreshLabel);
    opBounds = opBounds.getUnion(bounds);

    thresholdGroupSet->addGroup({ autoThreshButton, stdAutoThreshEditable, stdAutoThreshLabel });



    /* ****************  Detect Options  **************** */

    detectGroupSet = new VerticalGroupSet("Detect options");
    optionsPanel->addAndMakeVisible(detectGroupSet, 0);

    xPos = LEFT_EDGE;
    yPos += 40;

    detectTitle = new Label("detectTitle", "Detect options");
    detectTitle->setBounds(bounds = { xPos, yPos, 150, 50 });
    detectTitle->setFont(subtitleFont);
    optionsPanel->addAndMakeVisible(detectTitle);
    opBounds = opBounds.getUnion(bounds);
    
    /* -------- Bandpass Filter --------- */

    xPos += TAB_WIDTH;
    yPos += 45;

    bandpassButton = new ToggleButton("Bandpass Filter: ");
    bandpassButton->setBounds(bounds = { xPos, yPos, 140, C_TEXT_HT });
    bandpassButton->setToggleState((bool)processor->getParameter("use_bandpassfilter")->getValue(), dontSendNotification);
    bandpassButton->addListener(this);
    optionsPanel->addAndMakeVisible(bandpassButton);
    opBounds = opBounds.getUnion(bounds);

    lowCutBandpassLabel = new Label("LowCutL", "Low Cut (Hz):");
    lowCutBandpassLabel->setBounds(bounds = { xPos += 140 + 50, yPos, 100, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(lowCutBandpassLabel);
    opBounds = opBounds.getUnion(bounds);

    lowCutBandpassEditable = createEditable("lowCutE", String((float)processor->getParameter("low_cut")->getValue()), "",
        bounds = { xPos += 100, yPos, 50, C_TEXT_HT });
    lowCutBandpassEditable->setEnabled(bandpassButton->getToggleState());
    optionsPanel->addAndMakeVisible(lowCutBandpassEditable);
    opBounds = opBounds.getUnion(bounds);

    highCutBandpassLabel = new Label("highCutL", "High Cut (Hz):");
    highCutBandpassLabel->setBounds(bounds = { xPos += TAB_WIDTH + 50, yPos, 100, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(highCutBandpassLabel);
    opBounds = opBounds.getUnion(bounds);

    highCutBandpassEditable = createEditable("highCutE", String((float)processor->getParameter("high_cut")->getValue()), "",
        bounds = { xPos += 100, yPos, 50, C_TEXT_HT });
    highCutBandpassEditable->setEnabled(bandpassButton->getToggleState());
    optionsPanel->addAndMakeVisible(highCutBandpassEditable);
    opBounds = opBounds.getUnion(bounds);

    thresholdGroupSet->addGroup({ bandpassButton, lowCutBandpassLabel, lowCutBandpassEditable,
    highCutBandpassLabel, highCutBandpassEditable });
    
    /* -------- Minus Average --------- */
    xPos = LEFT_EDGE + TAB_WIDTH;
    yPos += 40;

    minusAverageButton = new ToggleButton("Minus Average");
    minusAverageButton->setBounds(bounds = { xPos, yPos, 120, C_TEXT_HT });
    minusAverageButton->setToggleState((bool)processor->getParameter("use_minus_Average")->getValue(), dontSendNotification);
    minusAverageButton->addListener(this);
    optionsPanel->addAndMakeVisible(minusAverageButton);
    opBounds = opBounds.getUnion(bounds);

    thresholdGroupSet->addGroup({ minusAverageButton });

    /* -------- Smooth --------- */
    xPos = LEFT_EDGE + TAB_WIDTH;
    yPos += 40;

    smoothButton = new ToggleButton("Smooth, K =  ");
    smoothButton->setBounds(bounds = { xPos, yPos, 120, C_TEXT_HT });
    smoothButton->setToggleState((bool)processor->getParameter("use_smooth")->getValue(), dontSendNotification);
    smoothButton->addListener(this);
    optionsPanel->addAndMakeVisible(smoothButton);
    opBounds = opBounds.getUnion(bounds);

    smoothKEditable = createEditable("smoothKE", String((float)processor->getParameter("smooth_K")->getValue()), "",
        bounds = { xPos += 120, yPos, 50, C_TEXT_HT });
    smoothKEditable->setEnabled(smoothButton->getToggleState());
    optionsPanel->addAndMakeVisible(smoothKEditable);
    opBounds = opBounds.getUnion(bounds);

    thresholdGroupSet->addGroup({ smoothButton, smoothKEditable });

    /* -------- rfs --------- */
    xPos = LEFT_EDGE + TAB_WIDTH;
    yPos += 40;
    rfsLabel = new Label("rfs", "Rfs: ");
    rfsLabel->setBounds(bounds = { xPos, yPos, 60, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(rfsLabel);
    opBounds = opBounds.getUnion(bounds);
    
    rfsEditable = createEditable("rfs", String((float)processor->getParameter("rfs")->getValue()), "",
        bounds = { xPos += 60, yPos, 50, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(rfsEditable);
    opBounds = opBounds.getUnion(bounds);
    
    thresholdGroupSet->addGroup({ rfsLabel, rfsEditable });

    /* -------- sdft --------- */
    xPos = LEFT_EDGE + TAB_WIDTH;
    yPos += 40;
    sdftLabel = new Label("sdft", "SDFT: ");
    sdftLabel->setBounds(bounds = { xPos, yPos, 60, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(sdftLabel);
    opBounds = opBounds.getUnion(bounds);

    windowTypeSdftLabel = new Label("windowTypeSdft", "Window Type: ");
    windowTypeSdftLabel->setBounds(bounds = { xPos += 60, yPos, 110, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(windowTypeSdftLabel);
    opBounds = opBounds.getUnion(bounds);

    windowTypeSdftBox = new ComboBox("windowTypeSelection");
    windowTypeSdftBox->setBounds(bounds = { xPos += 110, yPos, 160, C_TEXT_HT });
    windowTypeSdftBox->addListener(this);
    windowTypeSdftBox->addItem("Rectangle", SdftType::RECTANGLE);
    windowTypeSdftBox->addItem("Exp", SdftType::EXP);
    windowTypeSdftBox->addItem("ZeroPaddingExp", SdftType::ZeroPaddingExp);
    windowTypeSdftBox->setSelectedId((int)processor->getParameter("sdft_window_type")->getValue(), dontSendNotification);
    optionsPanel->addAndMakeVisible(windowTypeSdftBox);
    opBounds = opBounds.getUnion(bounds);

    windowSizeSdftLabel = new Label("windowSizeSdftL", "Window Size (s) : ");
    windowSizeSdftLabel->setBounds(bounds = { xPos += 180, yPos, 140, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(windowSizeSdftLabel);
    opBounds = opBounds.getUnion(bounds);

    windowSizeSdftEditable = createEditable("windowSizeSdftE", String((float)processor->getParameter("sdft_window_size")->getValue()), "",
        bounds = { xPos += 140, yPos, 50, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(windowSizeSdftEditable);
    opBounds = opBounds.getUnion(bounds);

    thresholdGroupSet->addGroup({ sdftLabel, windowTypeSdftLabel, windowTypeSdftBox, 
        windowSizeSdftLabel, windowSizeSdftEditable});


    /* ****************  Output Options  **************** */
    outputGroupSet = new VerticalGroupSet("Output options");
    optionsPanel->addAndMakeVisible(outputGroupSet, 0);

    xPos = LEFT_EDGE;
    yPos += 40;

    outputTitle = new Label("outputTitle", "Output options");
    outputTitle->setBounds(bounds = { xPos, yPos, 150, 50 });
    outputTitle->setFont(subtitleFont);
    optionsPanel->addAndMakeVisible(outputTitle);
    opBounds = opBounds.getUnion(bounds);

    /* -------- random delay --------- */
    xPos += TAB_WIDTH;
    yPos += 45;

    delayButton = new ToggleButton("Random Delay: ");
    delayButton->setBounds(bounds = { xPos, yPos, 120, C_TEXT_HT });
    delayButton->setToggleState((bool)processor->getParameter("use_delay")->getValue(), dontSendNotification);
    delayButton->addListener(this);
    optionsPanel->addAndMakeVisible(delayButton);
    opBounds = opBounds.getUnion(bounds);

    minDelayLabel = new Label("DelayMinL", "Min Time (s):");
    minDelayLabel->setBounds(bounds = { xPos += 120 + 30, yPos, 120, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(minDelayLabel);
    opBounds = opBounds.getUnion(bounds);

    minDelayEditable = createEditable("DelayMinE", String((float)processor->getParameter("delay_min")->getValue()), "",
        bounds = { xPos += 120, yPos, 50, C_TEXT_HT });
    minDelayEditable->setEnabled(delayButton->getToggleState());
    optionsPanel->addAndMakeVisible(minDelayEditable);
    opBounds = opBounds.getUnion(bounds);

    maxDelayLabel = new Label("DelayMaxL", "Max Time (s):");
    maxDelayLabel->setBounds(bounds = { xPos += TAB_WIDTH + 50, yPos, 120, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(maxDelayLabel);
    opBounds = opBounds.getUnion(bounds);

    maxDelayEditable = createEditable("DelayMaxE", String((float)processor->getParameter("delay_max")->getValue()), "",
        bounds = { xPos += 120, yPos, 50, C_TEXT_HT });
    maxDelayEditable->setEnabled(delayButton->getToggleState());
    optionsPanel->addAndMakeVisible(maxDelayEditable);
    opBounds = opBounds.getUnion(bounds);

    outputGroupSet->addGroup({ delayButton, minDelayLabel, minDelayEditable, maxDelayLabel, maxDelayEditable });

    /* -------- duration time --------- */
    xPos = LEFT_EDGE + TAB_WIDTH;
    yPos += 40;
    durationLabel = new Label("DurationL", "Duration Time (s): ");
    durationLabel->setBounds(bounds = { xPos, yPos, 160, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(durationLabel);
    opBounds = opBounds.getUnion(bounds);

    durationEditable = createEditable("DurationE", String((float)processor->getParameter("duration_time")->getValue()), "",
        bounds = { xPos += 160, yPos, 50, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(durationEditable);
    opBounds = opBounds.getUnion(bounds);

    outputGroupSet->addGroup({ durationLabel, durationEditable });

    /* -------- light duration --------- */
    xPos = LEFT_EDGE + TAB_WIDTH;
    yPos += 40;
    lightDurLabel = new Label("LightL", "Light Duration (s): ");
    lightDurLabel->setBounds(bounds = { xPos, yPos, 160, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(lightDurLabel);
    opBounds = opBounds.getUnion(bounds);

    lightDurEditable = createEditable("LightE", String((float)processor->getParameter("light_duration_time")->getValue()), "",
        bounds = { xPos += 160, yPos, 50, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(lightDurEditable);
    opBounds = opBounds.getUnion(bounds);

    outputGroupSet->addGroup({ lightDurLabel, lightDurEditable });

    /* -------- ignore duration --------- */
    xPos = LEFT_EDGE + TAB_WIDTH;
    yPos += 40;
    ignoreDurLabel = new Label("IgnoreL", "Ignore Duration (s): ");
    ignoreDurLabel->setBounds(bounds = { xPos, yPos, 160, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(ignoreDurLabel);
    opBounds = opBounds.getUnion(bounds);

    ignoreDurEditable = createEditable("IgnoreE", String((float)processor->getParameter("ignore_duration_time")->getValue()), "",
        bounds = { xPos += 160, yPos, 50, C_TEXT_HT });
    optionsPanel->addAndMakeVisible(ignoreDurEditable);
    opBounds = opBounds.getUnion(bounds);

    outputGroupSet->addGroup({ ignoreDurLabel, ignoreDurEditable });


    /* ****************  some extra padding  **************** */
    opBounds.setBottom(opBounds.getBottom() + 10);
    opBounds.setRight(opBounds.getRight() + 10);

    optionsPanel->setBounds(opBounds);
    thresholdGroupSet->setBounds(opBounds);
    detectGroupSet->setBounds(opBounds);
    outputGroupSet->setBounds(opBounds);
}

bool OcsBurstDetectorCanvas::updateIntLabel(Label* label, int min, int max, int defaultValue, int* out)
{
    const String& in = label->getText();
    int parsedInt;
    try
    {
        parsedInt = std::stoi(in.toRawUTF8());
    }
    catch (const std::logic_error&)
    {
        label->setText(String(defaultValue), dontSendNotification);
        return false;
    }

    *out = jmax(min, jmin(max, parsedInt));

    label->setText(String(*out), dontSendNotification);
    return true;
}

bool OcsBurstDetectorCanvas::updateFloatLabel(Label* label, float min, float max, float defaultValue, float* out)
{
    const String& in = label->getText();
    float parsedFloat;
    try
    {
        parsedFloat = std::stof(in.toRawUTF8());
    }
    catch (const std::logic_error&)
    {
        label->setText(String(defaultValue), dontSendNotification);
        return false;
    }

    *out = jmax(min, jmin(max, parsedFloat));

    label->setText(String(*out), dontSendNotification);
    return true;
}
