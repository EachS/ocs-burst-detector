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

#ifndef OcsBurstDetector_H_DEFINED
#define OcsBurstDetector_H_DEFINED

#include <ProcessorHeaders.h>

#include "OcsController.h"

class OcsBurstDetector : public GenericProcessor
{
public:
	OcsController* controllerPtr;

	OcsBurstDetector();

	~OcsBurstDetector();

	AudioProcessorEditor* createEditor() override;

	void updateSettings() override;

	void process(AudioBuffer<float>& buffer) override;

	bool startAcquisition() override;

	void parameterValueChanged(Parameter* param) override;

	void setSelectedStream(juce::uint16 streamId);

	// void sendPowerEventTTL(int i, bool onset);
	// void sendLightEventTTL(int i, bool onset);

private:
	EventChannel* thetaEventChannelPtr = nullptr;
	EventChannel* lightEventChannelPtr = nullptr;
	int powerEventChannel = 0;
	int lightEventChannel = 1;

	int rfs_idx = 0;

	double last_power = 0;

	juce::uint16 selectedStreamId;
	int channelList[128];
	int channelListLength = 0;
};

#endif