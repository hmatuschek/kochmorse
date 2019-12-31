#ifndef COLORBUTTON_HH
#define COLORBUTTON_HH

#include <QToolButton>
#include <QColorDialog>
#include <QColor>


class ColorButton : public QToolButton
{
	Q_OBJECT

public:
	explicit ColorButton(const QColor &color, QWidget *parent=nullptr);

	const QColor &color() const;
	void setColor(const QColor &color);

protected slots:
	void _onSelectColor();

protected:
	QColor _color;
};

#endif // COLORBUTTON_HH
