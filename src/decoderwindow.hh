#ifndef __KOCHMORSE_DECODERWINDOW_HH__
#define __KOCHMORSE_DECODERWINDOW_HH__

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QSlider>

#include "morsedecoder.hh"
#include "audiosink.hh"


class DecoderWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit DecoderWindow(QWidget *parent = 0);
  virtual ~DecoderWindow();

protected slots:
  void onCharReceived(QChar ch);
  void onUnknownCharReceived(QString pattern);

  void onFequencySet(QString val);
  void onSpeedSet(int val);
  void onThresholdSet(int val);

protected:
  void closeEvent(QCloseEvent *evt);

protected:
  MorseDecoder _decoder;
  PortAudioSource _audio_src;
  QChar _lastChar;

  QPlainTextEdit *_textView;

  QLineEdit *_freq;
  QSpinBox *_speed;
  QSlider  *_threshold;
};

#endif // __KOCHMORSE_DECODERWINDOW_HH__
