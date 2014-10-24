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
#include <QListWidget>
#include <QGroupBox>

/** Represents global persistent settings. */
class Settings: public QSettings
{
  Q_OBJECT
public:
  /** The possible morse tutors. */
  typedef enum {
    TUTOR_KOCH = 0,   ///< Koch method.
    TUTOR_RANDOM = 1  ///< Random chars.
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

  /** Random tutor: Retunrs the current character set. */
  QSet<QChar> randomChars() const;
  /** Random tutor: Sets the character set. */
  void setRandomChars(const QSet<QChar> &chars);

  /** Noise effect: Enabled. */
  bool noiseEnabled() const;
  /** Noise effect: Enable/Disable noise effect. */
  void setNoiseEnabled(bool enabled);

  /** Noise effect: SNR (dB). */
  float noiseSNR() const;
  /** Noise effect: Set SNR (dB). */
  void setNoiseSNR(float snr);
};


/** A configuration panel for the Koch tutor. */
class KochTutorSettingsView: public QGroupBox
{
  Q_OBJECT

public:
  explicit KochTutorSettingsView(QWidget *parent=0);

  /** Updates the persistent settings. */
  void save();

protected:
  QSpinBox *_lesson;
  QCheckBox *_prefLastChars;
};


/** A configuration panel for the Random tutor. */
class RandomTutorSettingsView: public QGroupBox
{
  Q_OBJECT

public:
  explicit RandomTutorSettingsView(QWidget *parent=0);

  /** Updates the persistent settings. */
  void save();

protected:
  QListWidget *_alpha;
  QListWidget *_num;
  QListWidget *_punct;
  QListWidget *_prosign;
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
