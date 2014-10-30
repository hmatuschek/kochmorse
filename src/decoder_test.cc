#include <iostream>
#include "morseencoder.hh"
#include "morsedecoder.hh"

int main(int argc, char *argv[]) {
  MorseDecoder decoder(20, 700);
  MorseEncoder encoder(&decoder, 700, 700, 20, 20, MorseEncoder::SOUND_SHARP, false);

  encoder.send("abc def xyz");

  return 0;
}
