#ifndef SETTINGSCTRL_HH
#define SETTINGSCTRL_HH

#include <QDialog>
#include <QComboBox>
#include <QTabWidget>
#include <QStackedWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QSettings>
#include "listwidget.hh"
#include <QGroupBox>
#include "morseencoder.hh"


/** Represents the global persistent settings. */
class Settings: public QSettings
{
  Q_OBJECT
public:
  /** The possible morse tutors. */
  typedef enum {
    TUTOR_KOCH = 0,   ///< Koch method.
    TUTOR_RANDOM = 1, ///< Random chars.
    TUTOR_QSO = 2,    ///< QSO Tutor.
    TUTOR_TX  = 3,    ///< The TX tutor.
    TUTOR_CHAT  = 4   ///< The Chat tutor.
  } Tutor;

public:
  /** Constructor. */
  explicit Settings();

  /** Retuns the current volume settings. */
  double volume() const;
  /** Sets the current volume. */
  void setVolume(double factor);

  /** Retunrs the current character speed. */
  int speed() const;
  /** Sets the character speed. */
  void setSpeed(int speed);

  /** Retunrs the current effective (pause) speed. */
  int effSpeed() const;
  /** Sets the effective (pause) speed. */
  void setEffSpeed(int speed);

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

  /** Noise effect: Enabled. */
  bool noiseEnabled() const;
  /** Noise effect: Enable/Disable noise effect. */
  void setNoiseEnabled(bool enabled);

  /** Noise effect: SNR (dB). */
  float noiseSNR() const;
  /** Noise effect: Set SNR (dB). */
  void setNoiseSNR(float snr);

  /** Fading effect: Enabled. */
  bool fadingEnabled() const;
  void setFadingEnabled(bool enabled);

  /** Fading effect: rate [1/min]. */
  float fadingRate() const;
  void setFadingRate(float rate);

  /** Fading effect: max damping [dB]. */
  float fadingMaxDamp() const;
  void setFadingMaxDamp(float damp);
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


/** A configuration panel for the QSO tutor. */
class QSOTutorSettingsView: public QGroupBox
{
  Q_OBJECT

public:
  explicit QSOTutorSettingsView(QWidget *parent=0);

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

protected:
  QSpinBox *_lesson;
  QCheckBox *_prefLastChars;
  QCheckBox *_repLastChar;
  QSpinBox *_minGroupSize;
  QSpinBox *_maxGroupSize;
  QCheckBox *_infinite;
  QSpinBox *_lineCount;
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

protected:
  ListWidget *_alpha;
  ListWidget *_num;
  ListWidget *_punct;
  ListWidget *_prosign;
  QSpinBox *_minGroupSize;
  QSpinBox *_maxGroupSize;
  QCheckBox *_infinite;
  QSpinBox *_lineCount;
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
  QSOTutorSettingsView *_qsoSettings;
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

protected:
  QSpinBox *_speed;
  QSpinBox *_effSpeed;
  QLineEdit *_tone;
  QLineEdit *_daPitch;
  QComboBox *_sound;
};


class EffectSettingsView: public QWidget
{
  Q_OBJECT

public:
  explicit EffectSettingsView(QWidget *parent=0);

  void save();

protected:
  QCheckBox *_noiseEnabled;
  QSpinBox  *_noiseSNR;
  QCheckBox *_fadingEnabled;
  QSpinBox  *_fadingRate;
  QSpinBox  *_fadingMaxDamp;
};


/** The preferences dialog. */
class SettingsDialog : public QDialog
{
  Q_OBJECT
public:
  explicit SettingsDialog(QWidget *parent = 0);

public slots:
  virtual void accept();

protected:
  QTabWidget *_tabs;
  TutorSettingsView *_tutor;
  CodeSettingsView *_code;
  EffectSettingsView *_effects;
};

#endif // SETTINGSCTRL_HH
