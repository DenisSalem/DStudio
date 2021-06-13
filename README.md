# DStudio

Your high performance and features rich set of virtual instruments.

# DSANDGRAINS 

An upcoming granular synthetizer for Linux. 

## Dependencies

- libFLAC
- libjack
- OpenGL
- libz
- libpng
- X11

## How to Compile

> git checkout dsandgrains

> cd src

> mkdir build

> cd build && cmake .. && make

With debug features :

> cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make

## TODO

### v0.0.0

- BEGIN 12/04/2021, DONE 11/04/2021 : Integrate new UI layout.
- BEGIN 12/04/2021, DONE 12/04/2021 : Notify Jack when audio port are renamed.
- BEGIN 12/04/2021, DONE 12/04/2021 : Disable scroll if text cursor is active.
- BEGIN 13/04/2021, DONE 19/04/2021 : Audio sample visual représentation.
- BEGIN 22/04/2021, DONE 22/04/2021 : Clean the mess of init_ui_elements, split with inline functions.
- BEGIN 22/04/2021, DONE 23/04/2021 : Implement UI animation transition while switching context.
- BEGIN 24/04/2021, DONE 24/04/2021 : Implement filter in file browser to only match supported format. Do not display hidden file.
- BEGIN 25/04/2021, DONE 25/04/2021 : SAMPLE Group : Amount (context switch, animation, applied effect).
- BEGIN 25/04/2021                  : SAMPLE Group : Stretch (context switch, animation, applied effect).
- BEGIN 08/05/2021n DONE 09/05/2021 : Implement audio range and signal Inversion.
- BEGIN 04/06/2021, DONE 04/06/2021 : Low level Midi support through Jack.
- BEGIN 06/06/2021, DONE 13/06/2021 : Multi contextual support of knob / midi binding.
- Centered text.
- Freezed ui element if context is not available.
- CONTINUOUS WIP : Connect UI elements to backend context.
- SAMPLE Group : Implement Start, end, Grain size, Decimate, Distribution Balance, Distance.
- VOICE Group : Inplement Volume, Density, Pitch, Pan.
- SAMPLE Group : Implement Cloud Motion Control.
- VOICE Group : Implement EQ with FFTW.
- Randomize.
- DAHDSR.
- LFO.
- Mouse hovering displaying info and values.
- Fix edge cases of UI elements overlapping.
- Charset should support special char.
- Open/Save file menu should have icons in files list for readability.
- Save CPU by disabling animated screen by double click.
- Send log to GUI.
- Fix text cursor glitch.
- Remove value jump in knob when selected. Use offset instead.
- Implement Configuration menu. (Knob mode, asset path, framerate, auto generated ui color, slider/knob sensitivity, etc...)
- Prevent to load audio sample already loaded.

### v1.0.0

- Allow locking "end" with "start" in settings.
- Allow to change the color of auto generated UI element (Sample screen, EQ, etc).
- Experimental support of the following plateform : ReactOS, Haiku.
- UI and window Rescale.
- Replace charmap usage with freetype, render area is then auto computed.
- Fully translatable user interface.
- Support for NSM and Ladish.
- LV2 support.
- Allow multiple window./
- Python live coding support.
- Export voice as audio file loop.
- Support of additional audio file format.
- Add sample by recording from a source.
