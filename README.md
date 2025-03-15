# Ocs-burst-detector

This plugin for the open-ephys [plugin-GUI](https://github.com/open-ephys/plugin-GUI/) is designed to detect bursts of neural oscillations. 



## Plugin Parameters

**FREQ_LOW:** The lower bound of the frequency band of interest.

**FREQ_HIGH:** The upper bound of the frequency band of interest.

**Channels:** The channels to be monitored for burst detection.



## Installation Instructions

Follow these steps to install and build the  `ocs-burst-detector`:

1. Set Up Folder Structure

   - Create an `OEPlugins` folder at the same level as the `plugin-GUI` folder.

   - Place the `ocs-burst-detector` folder inside the `OEPlugins` folder.

   - The resulting structure should look like this:

     ```bash
     plugin-GUI/
     OEPlugins/
     ├─ ocs-burst-detector/
     		├─ Build
     		├─ Source
     		└─ ...
     ```

2. Generate Project Files

   - Navigate to the Build folder within the  `ocs-burst-detector` directory using a command prompt or terminal.

   - Run the following CMake command to generate the project files:

     ``` bash
     cmake -G "Visual Studio 17 2022" -A x64 ..
     ```

   - If you are using a different version of Visual Studio, adjust the generator accordingly like:

     ```bash
     cmake -G "Visual Studio 16 2019" -A x64 ..
     cmake -G "Visual Studio 15 2017 Win64" ..
     cmake -G "Visual Studio 14 2015 Win64" ..
     cmake -G "Visual Studio 12 2013 Win64" ..
     ```

3. Build the Plugin

   - Open Visual Studio and load the generated OE_PLUGIN_ocs-burst-detector.sln file located in the Build folder.
   - Select the desired configuration (e.g., Debug or Release).
   - Build the solution or the ALL_BUILD project to compile the plugin.

