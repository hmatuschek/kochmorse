#include "aboutdialog.hh"
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QVBoxLayout>


AboutDialog::AboutDialog(QWidget *parent)
  : QDialog(parent)
{
  QTextBrowser *about = new QTextBrowser();
  about->setSource(QUrl("qrc:/text/about.html"));
  about->setMinimumSize(450, 400);
  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(about);
  layout->addWidget(buttons);
  this->setLayout(layout);

  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}
