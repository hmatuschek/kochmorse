#ifndef TEXTCOMPARE_HH
#define TEXTCOMPARE_HH

#include <QString>
#include <QVector>
int wordCompare(const QStringRef &a, const QStringRef &b, QVector<int> &mistakes);
int textCompare(const QString &a, const QString &b, QVector<int> &mistakes);


#endif // TEXTCOMPARE_HH
