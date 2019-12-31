#include "aboutdialog.hh"
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QTabWidget>


AboutDialog::AboutDialog(QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle(tr("About KochMorse"));

  QTextBrowser *about = new QTextBrowser();
  about->setSource(QUrl("qrc:/text/about.html"));
  about->setMinimumSize(450, 400);
  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close | QDialogButtonBox::Help);

  QTextBrowser *contr = new QTextBrowser();
  contr->setSource(QUrl("qrc:/text/contributors.html"));
  about->setMinimumSize(450, 400);

  QTabWidget *tabs = new QTabWidget();
  tabs->addTab(about, tr("About KochMorse"));
  tabs->addTab(contr, tr("Contributors"));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(tabs);
  layout->addWidget(buttons);
  this->setLayout(layout);

  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(buttons, SIGNAL(helpRequested()), this, SLOT(help()));
}

void
AboutDialog::help() {
  QDesktopServices::openUrl(tr("https://github.com/hmatuschek/kochmorse/wiki"));
}
