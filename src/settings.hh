#ifndef SETTINGSCTRL_HH
#define SETTINGSCTRL_HH

#include <QDialog>
#include <QComboBox>
#include <QTabWidget>
#include <QStackedWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QSettings>
#include <QDateTime>
#include "ringbuffer.hh"
#include <QLabel>
#include <QProgressBar>
#include "listwidget.hh"
#include <QGroupBox>
#include <QFontComboBox>
#include "morseencoder.hh"
#include "colorbutton.hh"
#include "audiosink.hh"


/** Represents the global persistent settings. */
class Settings: public QSettings
{
  Q_OBJECT
public:
  /** The possible morse tutors. */
  typedef enum {
    TUTOR_KOCH = 0,   ///< Koch method.
    TUTOR_RANDOM = 1, ///< Random chars.
    TUTOR_WORDSWORTH = 2, ///< Wordsworth tutor.
    TUTOR_TX  = 4,    ///< The TX tutor.
    TUTOR_CHAT  = 5,  ///< The Chat tutor.
    TUTOR_TEXTGEN = 6 ///< Generated text tuto.
  } Tutor;

public:
  /** Constructor. */
  explicit Settings(QObject *parent=nullptr);

  /** Retuns the current volume settings. */
  double volume() const;
  /** Sets the current volume. */
  void setVolume(double factor);

  bool checkForUpdates() const;
  void setCheckForUpdates(bool enable);
  QDateTime lastCheckForUpdates() const;
  void checkedForUpdates();

  int lastActiveTab() const;
  void setLastActiveTab(int index);

  /** Retunrs the current character speed. */
  int speed() const;
  /** Sets the character speed. */
  void setSpeed(int speed);

  /** Returns the inter-symbol pause-length factor. */
  double icPauseFactor() const;
  /** Sets the inter-symbol pause-length factor. */
  void setICPauseFactor(double factor);

  /** Returns the inter-word pause-length factor. */
  double iwPauseFactor() const;
  /** Sets the inter-word pause-length factor. */
  void setIWPauseFactor(double factor);

  /** Returns the current tone frequency. */
  int tone() const;
  /** Sets the tone frequency. */
  void setTone(int freq);

  /** Returns the current frequency shift for dashes. */
  int dashPitch() const;
  /** Sets the frequency shift for dashes. */
  void setDashPitch(int pitch);

  /** Returns the currently selected sound. */
  MorseEncoder::Sound sound() const;
  /** Sets the sound. */
  void setSound(MorseEncoder::Sound sound);

  /** Returns the currently selected jitter. */
  MorseEncoder::Jitter jitter() const;
  /** Sets the jitter. */
  void setJitter(MorseEncoder::Jitter jitter);

  double decoderLevel() const;
  void setDecoderLevel(double level);

  QAudioDeviceInfo outputDevice() const;
  void setOutputDevice(const QString &devicename);

  QAudioDeviceInfo inputDevice() const;
  void setInputDevice(const QString &devicename);

  /** Returns the current tutor. */
  Tutor tutor() const;
  /** Sets the tutor. */
  void setTutor(Tutor tutor);

  /** Koch tutor: Retunrs the current lesson. */
  int kochLesson() const;
  /** Koch tutor: Sets the lesson. */
  void setKochLesson(int n);

  /** Koch tutor: Returns true if "new" chars are more likely to be picked by the tutor. */
  bool kochPrefLastChars() const;
  /** Koch totor: Sets if "new" chars are more likely to be picked by the tutor. */
  void setKochPrefLastChars(bool pref);

  bool kochRepeatLastChar() const;
  void setKochRepeatLastChar(bool enable);

  int kochMinGroupSize() const;
  void setKochMinGroupSize(int size);
  int kochMaxGroupSize() const;
  void setKochMaxGroupSize(int size);

  bool kochInfiniteLineCount() const;
  void setKochInifiniteLineCount(bool enable);
  int kochLineCount() const;
  void setKochLineCount(int lines);
  bool kochSummary() const;
  void setKochSummary(bool show);
  int kochSuccessThreshold() const;
  void setKochSuccessThreshold(int thres);
  bool kochVerify() const;
  void setKochVerify(bool verify);
  bool kochHideOutput() const;
  void setKochHideOutput(bool hide);

  /** Random tutor: Retunrs the current character set. */
  QSet<QChar> randomChars() const;
  /** Random tutor: Sets the character set. */
  void setRandomChars(const QSet<QChar> &chars);

  int randomMinGroupSize() const;
  void setRandomMinGroupSize(int size);
  int randomMaxGroupSize() const;
  void setRandomMaxGroupSize(int size);

  bool randomInfiniteLineCount() const;
  void setRandomInifiniteLineCount(bool enable);
  int randomLineCount() const;
  void setRandomLineCount(int lines);
  bool randomVerify() const;
  void setRandomVerify(bool verify);
  bool randomHideOutput() const;
  void setRandomHideOutput(bool hide);
  bool randomSummary() const;
  void setRandomSummary(bool show);

  /** Wordworth tutor: Retunrs the current lesson. */
  int wordsworthLesson() const;
  /** Wordworth tutor: Sets the lesson. */
  void setWordsworthLesson(int n);

  /** Wordsworth tutor: Returns true if "new" words are more likely to be picked by the tutor. */
  bool wordsworthPrefLastWords() const;
  /** Wordsworth totor: Sets if "new" words are more likely to be picked by the tutor. */
  void setWordsworthPrefLastWords(bool pref);

  bool wordsworthRepeatLastWord() const;
  void setWordsworthRepeatLastWord(bool enable);

  bool wordsworthInfiniteLineCount() const;
  void setWordsworthInifiniteLineCount(bool enable);
  int wordsworthLineCount() const;
  void setWordsworthLineCount(int lines);
  bool wordsworthSummary() const;
  void setWordsworthSummary(bool show);
  int wordsworthSuccessThreshold() const;
  void setWordsworthSuccessThreshold(int thres);
  bool wordsworthVerify() const;
  void setWordsworthVerify(bool verify);
  bool wordsworthHideOutput() const;
  void setWordsworthHideOutput(bool hide);

  QString textGenFilename() const;
  void setTextGenFilename(const QString &filename);

  /** Noise effect: Enabled. */
  bool noiseEnabled() const;
  /** Noise effect: Enable/Disable noise effect. */
  void setNoiseEnabled(bool enabled);

  /** Noise effect: SNR (dB). */
  float noiseSNR() const;
  /** Noise effect: Set SNR (dB). */
  void setNoiseSNR(float snr);

  bool noiseFilterEnabled() const;
  void setNoiseFilterEnabled(bool enabled);

  float noiseFilterBw() const;
  void setNoiseFilterBw(float Bw);

  /** Fading effect: Enabled. */
  bool fadingEnabled() const;
  void setFadingEnabled(bool enabled);

  /** Fading effect: rate [1/min]. */
  float fadingRate() const;
  void setFadingRate(float rate);

  /** Fading effect: max damping [dB]. */
  float fadingMaxDamp() const;
  void setFadingMaxDamp(float damp);

  /** QRM effect: Enabled. */
  bool qrmEnabled() const;
  void setQRMEnabled(bool enabled);

  /** QRM effect: num. of QRM stations. */
  int qrmStations() const;
  void setQRMStations(int num);

  /** QRM effect: signal to "noise" ratio. */
  double qrmSNR() const;
  void setQRMSNR(double db);

  QFont textFont() const;
  void setTextFont(const QFont &font);

  QColor rxTextColor() const;
  void setRXTextColor(const QColor &color);
  QColor txTextColor() const;
  void setTXTextColor(const QColor &color);
  QColor summaryTextColor() const;
  void setSummaryTextColor(const QColor &color);

  bool sendHighScore() const;
  void setSendHighScore(bool send);

  QString hsID();

  QString hsCall() const;
  void setHSCall(const QString &call);
};


/** A configuration panel for the TX tutor. */
class TXTutorSettingsView: public QGroupBox
{
  Q_OBJECT

public:
  explicit TXTutorSettingsView(QWidget *parent=0);

  void save();
};


/** A configuration panel for the TX tutor. */
class ChatTutorSettingsView: public QGroupBox
{
  Q_OBJECT

public:
  explicit ChatTutorSettingsView(QWidget *parent=0);

  void save();
};


/** A configuration panel for the Koch tutor. */
class KochTutorSettingsView: public QGroupBox
{
  Q_OBJECT

public:
  explicit KochTutorSettingsView(QWidget *parent=0);

  /** Updates the persistent settings. */
  void save();

protected slots:
  void onMinSet(int value);
  void onMaxSet(int value);
  void onInfiniteToggled(bool enabled);
  void onShowSummaryToggled(bool enabled);
  void onVerifyToggled(bool enabled);

protected:
  QSpinBox *_lesson;
  QCheckBox *_prefLastChars;
  QCheckBox *_repLastChar;
  QSpinBox *_minGroupSize;
  QSpinBox *_maxGroupSize;
  QCheckBox *_infinite;
  QSpinBox *_lineCount;
  QCheckBox *_verify;
  QCheckBox *_hideOutput;
  QCheckBox *_summary;
  QSpinBox *_threshold;
};


/** A configuration panel for the Random tutor. */
class RandomTutorSettingsView: public QGroupBox
{
  Q_OBJECT

public:
  explicit RandomTutorSettingsView(QWidget *parent=0);

  /** Updates the persistent settings. */
  void save();

protected slots:
  void onMinSet(int value);
  void onMaxSet(int value);
  void onInfiniteToggled(bool enabled);
  void onVerifyToggled(bool enabled);

protected:
  ListWidget *_alpha;
  ListWidget *_num;
  ListWidget *_punct;
  ListWidget *_prosign;
  ListWidget *_special;
  QSpinBox *_minGroupSize;
  QSpinBox *_maxGroupSize;
  QCheckBox *_infinite;
  QSpinBox *_lineCount;
  QCheckBox *_verify;
  QCheckBox *_hideOutput;
  QCheckBox *_summary;
};


/** A configuration panel for the Koch tutor. */
class WordsworthTutorSettingsView: public QGroupBox
{
  Q_OBJECT

public:
  explicit WordsworthTutorSettingsView(QWidget *parent=0);

  /** Updates the persistent settings. */
  void save();

protected slots:
  void onInfiniteToggled(bool enabled);
  void onShowSummaryToggled(bool enabled);
  void onVerifyToggled(bool enabled);

protected:
  QSpinBox *_lesson;
  QCheckBox *_prefLastWords;
  QCheckBox *_repLastWord;
  QCheckBox *_infinite;
  QSpinBox *_lineCount;
  QCheckBox *_verify;
  QCheckBox *_hideOutput;
  QCheckBox *_summary;
  QSpinBox *_threshold;
};


/** A configuration panel for the TextGen tutor. */
class TextGenTutorSettingsView: public QGroupBox
{
  Q_OBJECT

public:
  explicit TextGenTutorSettingsView(QWidget *parent=0);

  void save();

protected slots:
  void onSelectFile();
  void onPreDefinedSelected(int idx);

protected:
  QComboBox *_defined;
  QLineEdit *_filename;
  QPushButton *_selectFile;
};


/** Tutor selection, also displays the config panel of the currently selected tutor. */
class TutorSettingsView: public QWidget
{
  Q_OBJECT
public:
  explicit TutorSettingsView(QWidget *parent=0);

  void save();

protected slots:
  void onTutorSelected(int idx);

protected:
  QComboBox *_tutor;
  QStackedWidget *_tutorSettings;
  KochTutorSettingsView *_kochSettings;
  RandomTutorSettingsView *_randSettings;
  WordsworthTutorSettingsView *_wordsworthSettings;
  TextGenTutorSettingsView *_textgetSettings;
  TXTutorSettingsView *_txSettings;
  ChatTutorSettingsView *_chatSettings;
};


/** Configuration panel for the Morse code generation. */
class CodeSettingsView: public QWidget
{
  Q_OBJECT

public:
  explicit CodeSettingsView(QWidget *parent=0);

  void save();

protected slots:
  void _onEffSpeedChanged();

protected:
  QSpinBox *_speed;
  QSpinBox *_icpFactor;
  QSpinBox *_iwpFactor;
  QLabel *_effSpeed;
  QLineEdit *_tone;
  QLineEdit *_daPitch;
  QComboBox *_sound;
  QComboBox *_jitter;
};


class EffectSettingsView: public QWidget
{
  Q_OBJECT

public:
  explicit EffectSettingsView(QWidget *parent=0);

  void save();

protected slots:
  void onNoiseToggled(bool enabled);
  void onNoiseFilterToggled(bool enabled);
  void onFadingToggled(bool enabled);
  void onQRMToggled(bool enabled);

protected:
  QCheckBox *_noiseEnabled;
  QSpinBox  *_noiseSNR;
  QCheckBox *_noiseFilter;
  QSpinBox  *_noiseBw;
  QCheckBox *_fadingEnabled;
  QSpinBox  *_fadingRate;
  QSpinBox  *_fadingMaxDamp;
  QCheckBox *_qrmEnabled;
  QSpinBox  *_qrmStations;
  QSpinBox  *_qrmSNR;
};


class DeviceSettingsView: public QWidget
{
  Q_OBJECT

public:
  explicit DeviceSettingsView(QWidget *parent=0);

public slots:
  void save();
  void populateDevices();

private slots:
  void onInputDeviceSelected(int index);
  void onAudioDataAvailable();

protected:
  Settings *_settings;
  QLabel *_loadingNote;
  QComboBox *_inputDevices;
  QComboBox *_outputDevices;
  QSpinBox  *_decoderLevel;

  RingBuffer _buffer;
  QAudioSource _source;
  double _level;

  QProgressBar *_levelBar;

protected:
  static constexpr double gamma = 0.998;
};


class AppearanceSettingsView: public QWidget
{
  Q_OBJECT

public:
  explicit AppearanceSettingsView(QWidget *parent=0);

  void save();

protected:
  QFontComboBox *_font;
  QSpinBox *_size;
  ColorButton *_rxColor;
  ColorButton *_txColor;
  ColorButton *_sumColor;
};

class HighScoreSettingsView: public QWidget
{
  Q_OBJECT

public:
  explicit HighScoreSettingsView(QWidget *parent=0);

  void save();

protected:
  QCheckBox *_enable;
  QLineEdit *_call;
};

/** The preferences dialog. */
class SettingsDialog : public QDialog
{
  Q_OBJECT
public:
  explicit SettingsDialog(QWidget *parent = 0);

public slots:
  virtual void accept();

protected slots:
  void showHelp();
  void onTabSelected(int index);

protected:
  Settings *_settings;
  QTabWidget *_tabs;
  TutorSettingsView *_tutor;
  CodeSettingsView *_code;
  EffectSettingsView *_effects;
  DeviceSettingsView *_devices;
  AppearanceSettingsView *_appearance;
  HighScoreSettingsView *_highscore;
};

#endif // SETTINGSCTRL_HH
