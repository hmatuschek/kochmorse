# KochMorse -- A morse-code tutor

KochMorse is a simple morse-code tutor using the
[Koch method](http://www.qsl.net/n1irz/finley.morse.html). It is written in C++ and uses
[Qt5](https://qt-project.org) for the graphical user interface (GUI).

<img src="http://i60.tinypic.com/mlnhqc.png" alt="Koch Morse in action." align="middle"/>

KochMorse runs under Linux, MacOS X and even under Windows. See the
[Install](https://github.com/hmatuschek/kochmorse/wiki/Install) wiki page for details.


## Features
Below, you will find a list of features of the current KochMorse release.

 * A morse tutor using the Koch method.
 * A random morse tutor -- Allows to select a set of characters and prosigns for practicing.
 * A QSO tutor -- Generates a random QSO text so you can practice reading typical QSOs.
 * Customized text tutor -- This tutor allows you to listen to a plain-text file or to a text 
   [generated](https://github.com/hmatuschek/kochmorse/wiki/TextGen) from a user-defined rules.
 * A transmit tutor -- Connect your practice tone-generator to your microphone input and check your
   transmissions with KochMorse.
 * A chat tutor -- This tutor is a combination of the transmit tutor and the QSO tutor. Call CQ
   and KochMorse will answer.
 * Different speed of characters and pauses. This allows to practice the reading in full speed,
   while having enough time to write the character down.
 * Different tones for dashes and dots.
 * Noise audio effect -- Increases the difficulty to read the morse code for practicing.
 * Fading audio effect -- Periodically fades the morse signal for practicing. In conjecture with
   the noise effect, it simulates the conditions on HF bands pretty well.


## Releases 
 * 2017-06-12 - [**Version 3.3.0**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.3.0)
 * 2015-06-30 - [**Version 3.2.2**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.2.2)
 * 2015-02-28 - [**Version 3.2.1**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.2.1)
 * 2015-02-22 - [**Version 3.2.0**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.2.0)
 * 2014-10-29 - [**Version 3.1.0**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.1.0)
 * 2014-10-25 - [**Version 3.0.0**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.0.0) - First "official" release of the C++ reimplementation.


## History

This is a C++/Qt reimplementation of my "old" python morse-code tutor (also called "kochmorse",
https://kochmorse.googlecode.com). As the "old" implementation relied on ALSA for sound output,
it was restricted to Linux. This reimplementation now uses PortAudio for the sound output and the
GUI is implemented using Qt5, it is therefore more platform independent and runs under Linux,
MacOS X and Windows.

 
