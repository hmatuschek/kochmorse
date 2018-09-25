#include "aboutdialog.hh"
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QDesktopServices>


AboutDialog::AboutDialog(QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle(tr("About KochMorse"));

  QTextBrowser *about = new QTextBrowser();
  about->setSource(QUrl("qrc:/text/about.html"));
  about->setMinimumSize(450, 400);
  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close | QDialogButtonBox::Help);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(about);
  layout->addWidget(buttons);
  this->setLayout(layout);

  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(buttons, SIGNAL(helpRequested()), this, SLOT(help()));
}

void
AboutDialog::help() {
  QDesktopServices::openUrl(tr("https://github.com/hmatuschek/kochmorse/wiki"));
}
