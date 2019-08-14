#ifndef __KOCHMORSE_MAINWINDOW_HH__
#define __KOCHMORSE_MAINWINDOW_HH__

#include <QMainWindow>
#include <QTextEdit>
#include <QAction>
#include <QSlider>
#include <QLabel>
#include <QShortcut>
#include <QPushButton>

#include "application.hh"

/** Implements the main window of the application. */
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit MainWindow(Application &app, QWidget *parent = nullptr);

protected slots:
  /** Gets called, once a tutor session is finished. */
  void onSessionFinished();
  /** Gets called, once a char was send. */
  void onCharSend(QString ch);
  /** Gets called, once a char was received. */
  void onCharReceived(QString ch);
  /** Gets called on play-button press. */
  void onPlayToggled(bool play);
  /** Gets called on preferences-button press. */
  void onPrefClicked();
  /** Gets called on info-button press. */
  void onAboutClicked();
  /** Gets called on volume change. */
  void onVolumeChanged(int value);
  /** Gets called on quit-button press. */
  void onQuit();
  void onCheck();
  void onTutorChanged();

protected:
  /** Holds a weak reference to the application instance. */
  Application &_app;
  /** Holds the output text view. */
  QTextEdit *_text;
  /** Holds the input text widget. */
  QTextEdit *_input;
  /** The check button. */
  QPushButton *_check;
  /** QAction for the play/stop toggle button. */
  QAction *_play;
  /** QAction for the "preferences" button. */
  QAction *_pref;
  /** QAction for the "about" button. */
  QAction *_info;
  /** A slider to control the volume. */
  QSlider *_volume;
  /** The label displaying the current volume value. */
  QLabel *_volumeLabel;
  /** The action for the "quit" button. */
  QAction *_quit;
  /** The shortcut for the play button*/
  QShortcut *playShortcut;
};

#endif // __KOCHMORSE_MAINWINDOW_HH__
