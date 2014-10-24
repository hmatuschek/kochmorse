#ifndef LISTWIDGET_HH
#define LISTWIDGET_HH

#include <QWidget>
#include <QPushButton>
#include <QListWidget>

class ListWidget : public QWidget
{
  Q_OBJECT
public:
  explicit ListWidget(QWidget *parent = 0);

  void addItem(const QString &name, const QVariant &data, bool checked=false);
  int numRows() const;
  bool isChecked(int row) const;
  QVariant itemData(int row) const;

protected slots:
  void onSelectAll();
  void onSelectNone();
  void onInvertSelection();

protected:
  QListWidget *_list;
  QPushButton *_selection;
  QMenu *_selectionMenu;
};

#endif // LISTWIDGET_HH
