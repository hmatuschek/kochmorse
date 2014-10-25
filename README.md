# KochMorse -- A morse-code tutor

KochMorse is a simple morse-code tutor using the [Koch method](http://www.qsl.net/n1irz/finley.morse.html). It is written in C++ and uses [Qt5](https://qt-project.org) for the grafical user interface (GUI) and [PortAudio](http://www.portaudio.com/) as its audio backend. 

<img src="http://i60.tinypic.com/mlnhqc.png" alt="Koch Morse in action." align="middle"/>

## Features
Below, you will find a list of features of the current KochMorse release. The highlighted featrues are new features compared to the "old" python implementation

 * *A simplified GUI* -- All settings are now accessable via the Preferences dialog.
 * A morse tutor using the Koch method (of cause).
 * *A random morse tutor* -- Allows to select a set of characters and prosigns for practicing.
 * *A QSO tutor* -- Selects a random QSO from a set of over 900 for practicing.
 * Different speed ofr characters and pauses. This allows to practice the reading in full speed, while having enough time to write the character.  
 * *Different tones for dashes and dots.*
 * *Noise audio effect* -- Increses the difficulty to read the morse code for practicing.
 * *Fading audio effect* -- Periodically fades the morse signal for practicing. In conjecture with the noise effect, it simulates the conditions on HF bands pretty well.


## Releases 

 * 2014-10-25 - **Version 3.0** - First "official" release of the C++ reimplementation. See [release details](https://github.com/hmatuschek/kochmorse/releases/tag/v3.0.0) for more information.


## History

This is a C++/Qt reimplementation of my "old" python morse-code tutor (also called "kochmorse", https://kochmorse.googlecode.com). As the "old" implementation relied on ALSA for sound output, it was restricted to Linux. This reimplementation now uses PortAudio for the sound output and the GUI is implemented using Qt5, it is therefore more platform independent and runs under Linux, MacOS X and Windows.

For this reimplementation, I simplified the GUI a lot. All settings are now "hidden" in a preferences dialog. All features of the "old" version are now present in this one and even some more...

 
