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

#ifndef OcsBurstDetectorEDITOR_H_DEFINED
#define OcsBurstDetectorEDITOR_H_DEFINED

#include <VisualizerEditorHeaders.h>

class OcsBurstDetectorEditor : public VisualizerEditor
{
public:

	/** Constructor */
	OcsBurstDetectorEditor(GenericProcessor* parentNode);

	/** Destructor */
	~OcsBurstDetectorEditor() { }

	Visualizer* createNewCanvas() override;

private:
	void selectedStreamHasChanged() override;

	/** Generates an assertion if this class leaks */
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OcsBurstDetectorEditor);
};

class ManualClearButton : public ParameterEditor,
	public Button::Listener
{
public:

	//** Constructor //
	ManualClearButton(Parameter* param);

	//** Destructor //
	virtual ~ManualClearButton() { }

	//** Respond to trigger button clicks //
	void buttonClicked(Button* label) override;

	//** Update view of the parameter editor component //
	void updateView() {};

	//** Sets component layout //
	void resized() override;

private:
	std::unique_ptr<UtilityButton> clearButton;
};

#endif // OcsBurstDetectorEDITOR_H_DEFINED