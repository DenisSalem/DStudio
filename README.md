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

> cd src

> mkdir build

> cd build && cmake .. && make

With debug features :

> cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make

## TODO

### v0.0.0

- DONE 11/04/2021 : Integrate new UI layout. 
- Visual audio sample représentation.
- Connect UI elements to backend context.
- SAMPLE Group : Implement Start, end, Grain size, Amount, Decimate, Distribution Balance, Distance.
- VOICE Group : Inplement Volume, Density, Pitch, Pan.
- SAMPLE Group : Implement Cloud Motion Control.
- VOICE Group : Implement EQ with FFTW.
- Randomize.
- DAHDSR.
- LFO.
- Mouse hovering displaying info and values.
- Allow port renaming through Jack.
- Notify Jack when audio port are renamed.
- Fix edge cases of UI elements overlapping.
- Charset should support special char.
- Open/Save file menu should have icons in files list for readability.
- Implement UI animation transition while switching context.
- Save CPU by disabling animated screen whitch double click.
 
### v1.0.0

- Experimental support of the following plateform : ReactOS, Haiku.
- UI and window Rescale.
- Fully translatable user interface.
- Support for NSM and Ladish.
- LV2 support.
- Allow multiple window.
- Python live coding support.
