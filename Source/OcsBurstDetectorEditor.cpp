/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
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

#include "OcsBurstDetectorEditor.h"
#include "OcsBurstDetectorCanvas.h"
#include "OcsBurstDetector.h"

OcsBurstDetectorEditor::OcsBurstDetectorEditor(GenericProcessor* parentNode) 
    : VisualizerEditor(parentNode, "Ocs Burst")
{
    
    desiredWidth = 220;

    addTextBoxParameterEditor("freq_low", 20, 25);
    addTextBoxParameterEditor("freq_high", 20, 80);
    addMaskChannelsParameterEditor("Channels", 120, 70);
}

Visualizer* OcsBurstDetectorEditor::createNewCanvas()
{
    OcsBurstDetectorCanvas* obdc = new OcsBurstDetectorCanvas(getProcessor());
    return obdc;
}


void OcsBurstDetectorEditor::selectedStreamHasChanged()
{
    OcsBurstDetector* processor = (OcsBurstDetector*)getProcessor();
    processor->setSelectedStream(getCurrentStream());

    // inform the canvas about selected stream updates
    updateVisualizer();
}


ManualClearButton::ManualClearButton(Parameter* param)
    : ParameterEditor(param)
{
    clearButton = std::make_unique<UtilityButton>("Clear", Font("Fira Code", "Regular", 12.0f)); // button text, font to use
    clearButton->addListener(this); // add listener to the button
    addAndMakeVisible(clearButton.get());  // add the button to the editor and make it visible

    setBounds(0, 0, 70, 20); // set the bounds of custom parameter editor
}

void ManualClearButton::buttonClicked(Button* b)
{
    param->setNextValue(clearButton->getLabel());
}

void ManualClearButton::resized()
{

    clearButton->setBounds(0, 0, 70, 20);
}
