#ifndef __KOCHMORSE_MAINWINDOW_HH__
#define __KOCHMORSE_MAINWINDOW_HH__

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
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

protected:
  Application &_app;
  QPlainTextEdit *_text;
  QPushButton *_play;
  QLabel *_time;
  QSpinBox *_speed;
  QSpinBox *_effSpeed;
};

#endif // __KOCHMORSE_MAINWINDOW_HH__
