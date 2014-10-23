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


/** Represents global persistent settings. */
class Settings: public QSettings
{
  Q_OBJECT
public:
  typedef enum {
    TUTOR_KOCH = 0
  } Tutor;

public:
  explicit Settings();

  double volume() const;
  void setVolume(double factor);

  int speed() const;
  void setSpeed(int speed);

  int effSpeed() const;
  void setEffSpeed(int speed);

  int tone() const;
  void setTone(int freq);

  int dashPitch() const;
  void setDashPitch(int pitch);

  Tutor tutor() const;
  void setTutor(Tutor tutor);

  int kochLesson() const;
  void setKochLesson(int n);

  bool kochPrefLastChars() const;
  void setKochPrefLastChars(bool pref);
};


class KochTutorSettingsView: public QWidget
{
  Q_OBJECT

public:
  explicit KochTutorSettingsView(QWidget *parent=0);

  void save();

protected:
  QSpinBox *_lesson;
  QCheckBox *_prefLastChars;
};


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
};


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
};

#endif // SETTINGSCTRL_HH
