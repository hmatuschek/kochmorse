#ifndef LISTWIDGET_HH
#define LISTWIDGET_HH

#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QFont>

/** A simple widget to show a list of chekable items with additional options to select all/none or
 * to invert the selection. */
class ListWidget : public QWidget
{
  Q_OBJECT
public:
  /** Constructor. */
  explicit ListWidget(QWidget *parent = nullptr);
  /** Add an item to the list. */
  void addItem(const QString &name, const QVariant &data, bool checked=false);
  /** The number of rows in the list. */
  int numRows() const;
  /** Returns @c true of the item in row @c row is checked. */
  bool isChecked(int row) const;
  /** Returns the data associated with the item in row @c row. */
  QVariant itemData(int row) const;

protected slots:
  /** Selects all items. */
  void onSelectAll();
  /** Unselects all items. */
  void onSelectNone();
  /** Inverts the item selection. */
  void onInvertSelection();

protected:
  /** The list of items. */
  QListWidget *_list;
  /** The menu button. */
  QPushButton *_selection;
  /** The selection menu. */
  QMenu *_selectionMenu;
};

#endif // LISTWIDGET_HH
