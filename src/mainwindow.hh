#ifndef __KOCHMORSE_MAINWINDOW_HH__
#define __KOCHMORSE_MAINWINDOW_HH__

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QAction>
#include <QSlider>
#include <QLabel>

#include "application.hh"

/** Implements the main window of the application. */
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit MainWindow(Application &app, QWidget *parent = 0);

protected slots:
  /** Gets called, once a tutor session is finished. */
  void onSessionFinished();
  /** Gets called, once a char was send. */
  void onCharSend(QChar ch);
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

protected:
  /** Holds a weak reference to the application instance. */
  Application &_app;
  /** Holds the text view. */
  QPlainTextEdit *_text;
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
};

#endif // __KOCHMORSE_MAINWINDOW_HH__
