# KochMorse -- A morse-code tutor

KochMorse is a simple morse-code tutor using the
[Koch method](http://www.qsl.net/n1irz/finley.morse.html). It is written in C++ and uses
[Qt5](https://qt-project.org) for the graphical user interface (GUI). [More ...](https://github.com/hmatuschek/kochmorse/wiki#how-to-use-kochmorse)

<img src="https://raw.githubusercontent.com/hmatuschek/kochmorse/master/doc/Koch_example.png" alt="Koch Morse in action." align="middle"/>

KochMorse runs under Linux, MacOS X and even under Windows. See the
[Install](https://github.com/hmatuschek/kochmorse/wiki/Install) wiki page for details.


## Features
Below, you will find a list of features of the current KochMorse release.

 * A morse tutor using the [Koch method](https://github.com/hmatuschek/kochmorse/wiki/Tutors#koch-tutor).
 * A [random morse tutor](https://github.com/hmatuschek/kochmorse/wiki/Tutors#random-tutor) -- Allows to select a set of characters and prosigns for practicing.
 * A [QSO tutor](https://github.com/hmatuschek/kochmorse/wiki/Tutors#qso-tutor) -- Generates a random QSO text so you can practice reading typical QSOs.
 * A [customized text tutor](https://github.com/hmatuschek/kochmorse/wiki/Tutors#generated-text-tutor) -- This tutor allows you to listen to a plain-text file or to a text 
   [generated](https://github.com/hmatuschek/kochmorse/wiki/TextGen) from a user-defined rules.
 * A [transmit tutor](https://github.com/hmatuschek/kochmorse/wiki/Tutors#transmit-tutor) -- Connect your practice tone-generator to your microphone input and check your
   transmissions with KochMorse.
 * A [chat tutor](https://github.com/hmatuschek/kochmorse/wiki/Tutors#chat-tutor) -- This tutor is a combination of the transmit tutor and the QSO tutor. Call CQ
   and KochMorse will answer.
 * Different speed of characters and pauses. This allows to practice the reading in full speed,
   while having enough time to write the character down.
 * Different tones for dashes and dots.
 * Keying style -- Simulates straight key or bug-style dash/dot-length jitter.
 * Noise audio effect -- Increases the difficulty to read the morse code for practicing.
 * Fading audio effect -- Periodically fades the morse signal for practicing. In conjecture with
   the noise effect, it simulates the conditions on HF bands pretty well.
 * QRM effect -- Increases the difficulty to read the morse code by simulating parallel QSOs nearby.

## Releases 
 * 2018-09-25 - [**Version 3.4.0**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.4.0)
 * 2018-06-12 - [**Version 3.3.0**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.3.0)
 * 2015-06-30 - [**Version 3.2.2**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.2.2)
 * 2015-02-28 - [**Version 3.2.1**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.2.1)
 * 2015-02-22 - [**Version 3.2.0**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.2.0)
 * 2014-10-29 - [**Version 3.1.0**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.1.0)
 * 2014-10-25 - [**Version 3.0.0**](https://github.com/hmatuschek/kochmorse/releases/tag/v3.0.0) - First "official" release of the C++ reimplementation.


## History

This is a C++/Qt reimplementation of my "old" python morse-code tutor (also called "kochmorse",
https://kochmorse.googlecode.com). As the "old" implementation relied on ALSA for sound output,
it was restricted to Linux. This reimplementation now uses the Qt5 Multimedia module for the 
sound output and the GUI is implemented using Qt5, it is therefore more platform independent 
and runs under Linux, MacOS X and Windows.

 
