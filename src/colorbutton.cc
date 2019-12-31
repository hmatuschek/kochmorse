#include "colorbutton.hh"

ColorButton::ColorButton(const QColor &color, QWidget *parent)
    : QToolButton(parent), _color(color)
{
  setColor(color);
  connect(this, SIGNAL(clicked()), this, SLOT(_onSelectColor()));
}

const QColor &
ColorButton::color() const {
	return _color;
}

void
ColorButton::setColor(const QColor &color) {
	_color = color;
	QPixmap pixmap(64,64);
	pixmap.fill(_color);
	setIcon(QIcon(pixmap));
}

void
ColorButton::_onSelectColor() {
  QColorDialog dialog(_color);
  if (QDialog::Accepted != dialog.exec())
    return;
  setColor(dialog.currentColor());
}

