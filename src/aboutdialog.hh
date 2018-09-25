#ifndef ABOUTDIALOG_HH
#define ABOUTDIALOG_HH

#include <QDialog>

/** Just the "About" dialog. */
class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit AboutDialog(QWidget *parent = nullptr);

protected slots:
  void help();
};

#endif // ABOUTDIALOG_HH
