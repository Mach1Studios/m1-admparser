# m1-admparser
ADM Parser to help tool for transcodings to Mach1 Spatial

### Contents
- `metadata-spec/` contains research on the topic of transcoding to and from ADM metadata driven formats
- `ADMParser/` contains a test metadata parser that converts points to a Mach1 Spatial friendly API, built with [OpenFrameworks](https://openframeworks.cc/)
- `ReaperScript` utility script for importing object panning automation data from Dolby Atmos or ADM metadata to apply directly within Reaper

### Setup for ADMParser
- Download OpenFrameworks: https://openframeworks.cc/download/
- Use OpenFrameworks ProjectGenerator to generate the `m1-admparser/ADMParser/` dir for your preferred IDE

### Using ReaperScript AtmosParser
- Edit lines 1 & 2 of `ReaperScript/AtmosParser.lua` to point to your local full path of `AtmosParser.lua` and copy and paste the Atmos metadata to that path and supply the filename in line 2
- Highlight and select the track containing the M1-Panner you wish to import the object panning data to
- From Reaper go to `Actions>Show Action List...?`
- Select `New action...`
- `Load ReaScript...` and navigate to the location of the edited `AtmosParser.lua`
- When prompted type in which object ID of the Atmos mix you are importing