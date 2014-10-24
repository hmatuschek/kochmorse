#include "listwidget.hh"
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>

ListWidget::ListWidget(QWidget *parent) :
  QWidget(parent)
{
  _list = new QListWidget();
  QPushButton *selection = new QPushButton(tr("Select..."));
  QMenu *selectionMenu = new QMenu();
  selectionMenu->addAction("all", this, SLOT(onSelectAll()));
  selectionMenu->addAction("none", this, SLOT(onSelectNone()));
  selectionMenu->addAction("invert", this, SLOT(onInvertSelection()));
  selection->setMenu(selectionMenu);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(0); layout->setContentsMargins(0,0,0,0);
  layout->addWidget(_list);
  layout->addWidget(selection);
  this->setLayout(layout);
}

void
ListWidget::addItem(const QString &name, const QVariant &data, bool checked) {
  QListWidgetItem *item = new QListWidgetItem(name, _list);
  item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
  if (checked) { item->setCheckState(Qt::Checked); }
  else { item->setCheckState(Qt::Unchecked); }
  item->setData(Qt::UserRole, data);
}

int
ListWidget::numRows() const {
  return _list->count();
}

bool
ListWidget::isChecked(int row) const {
  return Qt::Checked == _list->item(row)->checkState();
}

QVariant
ListWidget::itemData(int row) const {
  return _list->item(row)->data(Qt::UserRole);
}

void
ListWidget::onSelectAll() {
  for (int i=0; i<_list->count(); i++) {
    _list->item(i)->setCheckState(Qt::Checked);
  }
}

void
ListWidget::onSelectNone() {
  for (int i=0; i<_list->count(); i++) {
    _list->item(i)->setCheckState(Qt::Unchecked);
  }
}

void
ListWidget::onInvertSelection() {
  for (int i=0; i<_list->count(); i++) {
    if (Qt::Checked == _list->item(i)->checkState()) {
      _list->item(i)->setCheckState(Qt::Unchecked);
    } else {
      _list->item(i)->setCheckState(Qt::Checked);
    }
  }
}

