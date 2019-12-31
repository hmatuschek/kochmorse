#include "textcompare.hh"
#include <QStringList>
#include <QDebug>
#include <QRegularExpression>

void _add_missing_word(const QStringRef &word, QVector<int> &mistakes) {
  int offset = word.position();
  for (int i=0; i<word.size(); i++, offset++) {
    mistakes.push_back(offset);
  }
}

int wordCompare(const QStringRef &a, const QStringRef &b, QVector<int> &mistakes)
{
  mistakes.clear();
  mistakes.reserve(a.length());
	int ia = 0, ib = 0;

  for (; ia<a.size(); ia++, ib++) {
    // If word B ends earlier -> assume rest of word A is missing
    if (ib >= b.size()) {
      mistakes.push_back(a.position()+ia);
      continue;
    }
    // If chars in word A & B match -> ok
    if (a.at(ia) == b.at(ib))
      continue;

    // Otherwise there must be a mistake
    mistakes.push_back(a.position()+ia);

    if ((ia+1) >= a.size())
      continue;

    // Check if it is more likely that a char is missing or if the current char is simply wrong,
    // by comparing the number of mistakes for the remaining words
    // with the number of mismatches if the current char in A is omitted
    QVector<int> mwrong, mskip;
    int ewrong = wordCompare(a.mid(ia+1), b.mid(ib+1), mwrong);
    int eskip  = wordCompare(a.mid(ia+1), b.mid(ib), mskip);
    // if dropping a char in
    if (eskip < ewrong) {
      mistakes.append(mskip);
      return mistakes.size();
    } else {
      mistakes.append(mwrong);
      return mistakes.size();
    }
  }

  return mistakes.size();
}

int textCompare(const QVector<QStringRef> &awords, const QVector<QStringRef> &bwords,
                QVector<int> &mistakes, int ia, int ib, int misses=5)
{
  mistakes.clear();
  QVector<int> wordMistakes, mmiss, mwrong;

  while (ia < awords.size())
  {
    // If there are words missing at the end of B
    if (ib >= bwords.size()) {
      _add_missing_word(awords.at(ia), mistakes);
      ia++;
      continue;
    }

    // Compare current words
    int cmp = wordCompare(awords.at(ia), bwords.at(ib), wordMistakes);
    // If fine, continue with next words
    if (0 == cmp) {
      ia++, ib++;
      continue;
    }

    // Compute mistakes under the assumption that the current word just contains some mistakes.
    int ewrong = textCompare(awords, bwords, mwrong, ia+1, ib+1, misses);

    // Limit branching: This algorithm is O(2^n)! Where n is the number of words to compare.
    // Missing complete words, however, is rare. Hence, we may limit the number of complete words
    // being missed to a small number. This turns the complexity to O(n), as the number of branches
    // per word is limited to a fixed (small) number.
    if (misses > 0) {
      // Compute number of mistakes under the assumption
      int emiss  = textCompare(awords, bwords, mmiss, ia+1, ib, misses-1);
      if ((awords.at(ia).size()+emiss) < (cmp+ewrong)) {
        _add_missing_word(awords.at(ia), mistakes);
        mistakes.append(mmiss);
        return mistakes.size();
      }
    }

    mistakes.append(wordMistakes);
    mistakes.append(mwrong);
    return mistakes.size();
  }

  return mistakes.size();
}

int textCompare(const QString &a, const QString &b, QVector<int> &mistakes)
{
  mistakes.clear();
  mistakes.reserve(a.size());

  QVector<QStringRef> alines = a.splitRef(QRegularExpression("\\R"), QString::SkipEmptyParts);
  QVector<QStringRef> blines = b.splitRef(QRegularExpression("\\R"), QString::SkipEmptyParts);
  for (int l=0; l<alines.size(); l++) {
    // Split line into words
    QVector<QStringRef> awords = alines[l].split(' ', QString::SkipEmptyParts);
    // If there is a line left in b
    if (l < blines.size()) {
      // -> compare lines
      QVector<int> lineMistakes;
      QVector<QStringRef> bwords = blines[l].split(' ', QString::SkipEmptyParts);
      textCompare(awords, bwords, lineMistakes, 0, 0);
      mistakes.append(lineMistakes);
    } else {
      // otherwise add entire line to misses
      for (int i=0; i<awords.size(); i++) {
        _add_missing_word(awords[i], mistakes);
      }
    }
  }
  return mistakes.size();
}

