#include "morseencoder.hh"
#include "audiosink.hh"


int main(int argc, char *argv[])
{
  AudioSink::init();

  AudioSink sink;
  MorseEncoder morse(&sink, 750, 750, 20, 15);

  morse.send("Hallo World.");

  AudioSink::finalize();

  return 0;
}
