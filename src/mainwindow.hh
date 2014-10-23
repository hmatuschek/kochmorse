#ifndef __KOCHMORSE_MAINWINDOW_HH__
#define __KOCHMORSE_MAINWINDOW_HH__

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QAction>
#include <QSlider>
#include <QLabel>

#include "application.hh"


class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(Application &app, QWidget *parent = 0);

protected slots:
  void onSessionFinished();
  void onCharSend(QChar ch);

  void onPlayToggled(bool play);
  void onPrefClicked();
  void onAboutClicked();
  void onVolumeChanged(int value);
  void onQuit();

protected:
  Application &_app;
  QPlainTextEdit *_text;
  QAction *_play;
  QAction *_pref;
  QAction *_info;
  QSlider *_volume;
  QLabel *_volumeLabel;
  QAction *_quit;
};

#endif // __KOCHMORSE_MAINWINDOW_HH__
