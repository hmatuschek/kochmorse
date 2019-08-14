#include "textcompare.hh"
#include <QStringList>

int countMismatchingChars(const QStringRef &a, const QStringRef &b, int ia, int ib) {
  int err = 0;
  while ( (ia<a.size()) || (ib<b.size())) {
    if (ia>=a.size())
      err++;
    else if (ib>=b.size())
      err++;
    else if (a.at(ia) != b.at(ib))
      err++;
    ia++; ib++;
  }
  return err;
}

int countMismatchingWords(const QVector<QStringRef> &a, const QVector<QStringRef> &b, int ia, int ib) {
  int err = 0;
  QVector<int> wordMistakes;
  while ( (ia<a.size()) || (ib<b.size())) {
    if (ia>=a.size())
      err += b.at(ib).size();
    else if (ib>=b.size())
      err += a.at(ia).size();
    else
      err += wordCompare(a.at(ia), b.at(ib), wordMistakes);
    ia++; ib++;
  }
  return err;
}

void _add_missing_word(const QStringRef &word, QVector<int> &mistakes) {
  int offset = word.position();
  for (int i=0; i<word.size(); i++, offset++) {
    mistakes.push_back(offset);
  }
}


int wordCompare(const QStringRef &a, const QStringRef &b, QVector<int> &mistakes)
{
  mistakes.clear();
  mistakes.reserve(a.size());
	int ia = 0, ib = 0;

  for (; ia<a.size(); ia++, ib++) {
    if (ib >= b.size()) {
      mistakes.push_back(a.position()+ia);
      continue;
    }
    if (a.at(ia) == b.at(ib))
      continue;

    mistakes.push_back(a.position()+ia);

    int eref = countMismatchingChars(a, b, ia, ib);
    int etest = countMismatchingChars(a, b, ia+1, ib);
    if (etest < eref)
      ia++;
  }

  return mistakes.size();
}


int textCompare(const QString &a, const QString &b, QVector<int> &mistakes)
{
  mistakes.clear();
  mistakes.reserve(a.size());
  QVector<int> wordMistakes;

  QVector<QStringRef> awords = a.splitRef(' ', QString::SkipEmptyParts);
  QVector<QStringRef> bwords = b.splitRef(' ', QString::SkipEmptyParts);
  int ia = 0, ib = 0, e = 0;
  for (; ia<awords.size(); ia++, ib++) {
    if (ib >= bwords.size()) {
      e += awords.at(ia).size();
      _add_missing_word(awords.at(ia), mistakes);
      continue;
    }
    int cmp = wordCompare(awords.at(ia), bwords.at(ib), wordMistakes);
    if (0 == cmp)
      continue;
    int eref = countMismatchingWords(awords, bwords, ia, ib);
    int etest = countMismatchingWords(awords, bwords, ia+1, ib);
    if (etest < eref) {
      e += awords.at(ia).size();
      _add_missing_word(awords.at(ia), mistakes);
      ia++;
    } else {
      e += cmp;
      mistakes.append(wordMistakes);
    }
  }

  return e;
}
