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

#include "OcsBurstDetector.h"

#include "OcsBurstDetectorEditor.h"


OcsBurstDetector::OcsBurstDetector()
    : GenericProcessor("Ocs Burst Detector")
    , selectedStreamId(0)
    , rfs_idx (0)
{
    controllerPtr = new OcsController();
    
    // Main Page
    addFloatParameter(Parameter::GLOBAL_SCOPE, "freq_low", "OCS Freq Low", controllerPtr->getFreqLow(), 0.1, 15000, 0.001, true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "freq_high", "OCS Freq High", controllerPtr->getFreqHigh(), 0.1, 15000, 0.001, true);
    addMaskChannelsParameter(Parameter::STREAM_SCOPE, "Channels", "Channels to filter for this stream", true);

    // Threshold Type
    addIntParameter(Parameter::GLOBAL_SCOPE, "threshold_type", "Type of Threshold to use", controllerPtr->getThresholdType(), 0, 1, true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "fix_threshold", "Constant Threshold", controllerPtr->getFixThreshold(), 0.0f, 100.0f, 0.01f, true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "Auto_STD_TH", "Auto Threshold N STD", controllerPtr->getStdTH(), 0.0f, 100.0f, 0.01f, true);

    // Detect Options
    addBooleanParameter(Parameter::GLOBAL_SCOPE, "use_bandpassfilter", "USE BandPass Filter", controllerPtr->get_use_bandpass_filter(), true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "low_cut", "Filter low cut", controllerPtr->getBandpassLow(), 0.1, 15000, 0.001, true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "high_cut", "Filter high cut", controllerPtr->getBandpassHigh(), 0.1, 15000, 0.001, true);

    addBooleanParameter(Parameter::GLOBAL_SCOPE, "use_minus_Average", "USE Minus Average", controllerPtr->get_use_minus_average(), true);

    addBooleanParameter(Parameter::GLOBAL_SCOPE, "use_smooth", "USE Smooth", controllerPtr->get_use_smooth(), true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "smooth_K", "Smooth Parameter K", controllerPtr->getSmoothK(), 0.001f, 1.0f, 0.001f, true);

    addIntParameter(Parameter::GLOBAL_SCOPE, "rfs", "rfs", controllerPtr->get_rfs(), 1, 44100, true);

    addIntParameter(Parameter::GLOBAL_SCOPE, "sdft_window_type", "Type of SDFT Window to use", controllerPtr->getSdftType(), 1, 3, true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "sdft_window_size", "Window Size of SDFT", controllerPtr->getSdftWindowSize(), 0.1, 10, 0.001, true);

    // Outout Options
    addBooleanParameter(Parameter::GLOBAL_SCOPE, "use_delay", "is delay enabled", controllerPtr->get_use_delay(), true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "delay_min", "Delay Min Time", controllerPtr->getDelayMin(), 0, 10, 0.001, true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "delay_max", "Delay Max Time", controllerPtr->getDelayMax(), 0, 10, 0.001, true);

    addFloatParameter(Parameter::GLOBAL_SCOPE, "duration_time", "Duration Time", controllerPtr->getDurTime(), 0.001, 10, 0.001, true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "light_duration_time", "Light Duration Time", controllerPtr->getLightDur(), 0.001, 10, 0.001, true);
    addFloatParameter(Parameter::GLOBAL_SCOPE, "ignore_duration_time", "Ignore Duration Time", controllerPtr->getIgnoreDur(), 0.001, 10, 0.001, true);

    //// Parameter for manually generating events
    //addStringParameter(Parameter::GLOBAL_SCOPE, // parameter scope
    //    "manual_clear",        // parameter name
    //    "clear",  // parameter description
    //    String());               // default value
}


OcsBurstDetector::~OcsBurstDetector()
{
    controllerPtr->~OcsController();
}


AudioProcessorEditor* OcsBurstDetector::createEditor()
{
    editor = std::make_unique<OcsBurstDetectorEditor>(this);
    return editor.get();
}


void OcsBurstDetector::updateSettings()
{
    for (auto stream : getDataStreams())
    {
        const uint16 streamId = stream->getStreamId();

        std::cout << "[UPDATE SETTING]: Ready to create event channel " << std::endl;
        EventChannel::Settings thetaEventChannelPtrSettings{
            EventChannel::Type::TTL,
            "Ocs Burst Detector Output",
            "theta Event Channel",
            "ttl.events",
            getDataStream(stream->getStreamId())
        };
        eventChannels.add(new EventChannel(thetaEventChannelPtrSettings));
        eventChannels.getLast()->addProcessor(processorInfo.get());
        thetaEventChannelPtr = eventChannels.getLast();

        EventChannel::Settings lightEventChannelPtrSettings{
                EventChannel::Type::TTL,
                "Ocs Burst Detector Output",
                "light EventChannel",
                "ttl.events",
                getDataStream(stream->getStreamId())
        };
        eventChannels.add(new EventChannel(lightEventChannelPtrSettings));
        eventChannels.getLast()->addProcessor(processorInfo.get());
        lightEventChannelPtr = eventChannels.getLast();
    }
}


void OcsBurstDetector::process(AudioBuffer<float>& buffer)
{
    for (auto stream : getDataStreams())
    {

        if ((*stream)["enable_stream"] && stream->getStreamId() == selectedStreamId)
        {
            const uint16 streamId = stream->getStreamId();
            const int64 startSampleForBlock = getFirstSampleNumberForBlock(streamId);
            const uint32 nSamples = getNumSamplesInBlock(streamId);

            int rfs_factor = controllerPtr->get_rfs_factor();

            float* ptrBuffer = buffer.getWritePointer(1); // Should //

            const float** ptrRs = buffer.getArrayOfReadPointers();

            for (int i = 0; i < nSamples; i++) {
                rfs_idx++;
                if (rfs_idx < rfs_factor) {
                    *ptrBuffer = last_power; // Should //
                    ptrBuffer++; // Should //
                    continue;
                }
                rfs_idx = 0;

                float avgInput = 0;
                for (int j = 0; j < channelListLength; j++) {
                    avgInput += ptrRs[channelList[j]][i];
                }
                auto [res, power] = controllerPtr->process(avgInput/channelListLength);

                *ptrBuffer = power*100; // Should //
                last_power = power*100; // Should //

                if (res & 0b1000) {
                    bool ans = res & 0b0100;
                    TTLEventPtr m_powerEventChannel_eventPtr = TTLEvent::createTTLEvent(thetaEventChannelPtr,
                        startSampleForBlock + i,
                        powerEventChannel, ans);

                    addEvent(m_powerEventChannel_eventPtr, i);
                }
                if (res & 0b0010) {
                    bool ans = res & 0b0001;
                    TTLEventPtr m_lightEventChannel_eventPtr = TTLEvent::createTTLEvent(lightEventChannelPtr,
                        startSampleForBlock + i,
                        lightEventChannel, ans);

                    addEvent(m_lightEventChannel_eventPtr, i);
                }

                ptrBuffer++; // Should //
            }

        }
    }
}


void OcsBurstDetector::parameterValueChanged(Parameter* param) {
    std::string name = param->getName().toStdString();
    if ((!param->getName().equalsIgnoreCase("Channels")) && 
        (!param->getName().equalsIgnoreCase("enable_stream"))) {
        controllerPtr->parameterValueChange(name, param->getValue());
    }

    rfs_idx = 0;
}

void OcsBurstDetector::setSelectedStream(juce::uint16 streamId)
{
    selectedStreamId = streamId;
}


bool OcsBurstDetector::startAcquisition() {
    rfs_idx = 0;
    controllerPtr->clear_all();

    channelListLength = 0;
    for (auto stream : getDataStreams())
    {
        if ((*stream)["enable_stream"])
        {
            for (auto localChannelIndex : *((*stream)["Channels"].getArray()))
            {
                channelList[channelListLength] = int(localChannelIndex);
                channelListLength++;
            }
        }
    }

    std::cout << "[Start Acquisition]" << std::endl;
    std::cout << "FreqLow: " << controllerPtr->getFreqLow() << std::endl;
    std::cout << "FreqHigh: " << controllerPtr->getFreqHigh() << std::endl;
    std::cout << "BandpassLow: " << controllerPtr->getBandpassLow() << std::endl;
    std::cout << "BandpassHigh: " << controllerPtr->getBandpassHigh() << std::endl;
    std::cout << "FixThreshold: " << controllerPtr->getFixThreshold() << std::endl;
    std::cout << "DelayMin: " << controllerPtr->getDelayMin() << std::endl;
    std::cout << "DelayMax: " << controllerPtr->getDelayMax() << std::endl;
    std::cout << "ThresholdType: " << controllerPtr->getThresholdType() << std::endl;
    std::cout << "Threshold: " << controllerPtr->getThreshold() << std::endl;
    std::cout << "StdTH: " << controllerPtr->getStdTH() << std::endl;
    std::cout << "SmoothK: " << controllerPtr->getSmoothK() << std::endl;
    std::cout << "SdftType: " << controllerPtr->getSdftType() << std::endl;
    std::cout << "SdftWindowSize: " << controllerPtr->getSdftWindowSize() << std::endl;
    std::cout << "DurTime: " << controllerPtr->getDurTime() << std::endl;
    std::cout << "LightDur: " << controllerPtr->getLightDur() << std::endl;
    std::cout << "IgnoreDur: " << controllerPtr->getIgnoreDur() << std::endl;

    std::cout << "UseDelay: " << std::boolalpha << controllerPtr->get_use_delay() << std::endl;
    std::cout << "UseSTFT: " << std::boolalpha << controllerPtr->get_use_stft() << std::endl;
    std::cout << "UseAutoTH: " << std::boolalpha << controllerPtr->get_use_auto_th() << std::endl;
    std::cout << "UseMinusAverage: " << std::boolalpha << controllerPtr->get_use_minus_average() << std::endl;
    std::cout << "UseBandpassFilter: " << std::boolalpha << controllerPtr->get_use_bandpass_filter() << std::endl;
    std::cout << "UseSmooth: " << std::boolalpha << controllerPtr->get_use_smooth() << std::endl;

    std::cout << "RFSFactor: " << controllerPtr->get_rfs_factor() << std::endl;
    std::cout << "FS: " << controllerPtr->get_fs() << std::endl;
    std::cout << "RFS: " << controllerPtr->get_rfs() << std::endl;

    std::cout << "Selected Channels : ";
    for (int i = 0; i < channelListLength; i++) {
        std::cout << channelList[i] << " ";
    }
    std::cout << std::endl;

    return true;
}