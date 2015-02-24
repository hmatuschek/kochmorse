/* A MegaHAL implementation using Qt.
 *
 * This implementation is more or less directly derived from the original one by
 * Jason Hutchens, http://megahal.sourceforge.net
 *
 * Copyright 2015, Hannes Matuschek <hmatuschek at gmail dot com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the license or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the Gnu Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA. */

#ifndef __QHAL_HH__
#define __QHAL_HH__

#include <QVector>
#include <QHash>
#include <QStringList>
#include <QIODevice>


/** A N-gram tree. */
class QHalTree
{
protected:
  /** Creates an empty tree for the given symbol. */
  explicit QHalTree(size_t symbol);

public:
  /** Creates an empty tree. */
  QHalTree();
  /** Reads a tree from the given device. */
  QHalTree(QIODevice &device);
  /** Destructor. */
  ~QHalTree();

  /** Returns @c true of the tree is empty. */
  bool empty() const;
  /** Returns the number of branches in the tree. */
  size_t size() const;
  /** Returns the usage of the symbol. */
  size_t usage() const;
  /** Retunrs the cummulative usage of the branches. */
  size_t count() const;
  /** Returns the symbol of the tree. */
  size_t symbol() const;

  /** Adds a symbol branch or increases its usage count if it already exists. */
  QHalTree *addSymbol(size_t symbol);
  /** Searches for a branch with the given symbol. Returns @c NULL if none can be found. */
  QHalTree *findSymbol(size_t symbol);
  /** Selects a branch at random. */
  QHalTree *random() const;
  /** Returns a branch by index. */
  QHalTree *branch(size_t idx) const;

  /** Serializes the tree into the given device. */
  void serialize(QIODevice &device);
  /** Reads a tree from the given device. */
  void read(QIODevice &device);
  /** Clears the tree. */
  void clear();

protected:
  /** Searches for a branch with the given symbol and returns it. If there is no branch with the
   * given symbol, a new one will be created and returned. */
  QHalTree *findSymbolAdd(size_t symbol);

protected:
  /** The symbol of this node. */
  size_t _symbol;
  /** The usage count of this symbol. */
  size_t _usage;
  /** The cummulative usage count child nodes. */
  size_t _count;
  /** Mapps the symbol to the branch index. */
  QHash<size_t, size_t> _symbols;
  /** The vector of sub-trees. */
  QVector<QHalTree *> _branches;
};


/** Bidirectional Symbol <-> Word dictionary. */
class QHalDict
{
public:
  /** Constructs an empty dictionary. */
  QHalDict();
  /** Reads a dictionary form the given device. */
  explicit QHalDict(QIODevice &device);
  /** Destructor. */
  ~QHalDict();

  /** Adds a word to the dictionary and returns its symbol. */
  size_t addWord(const QString &word);
  /** Returns @c true if the dictionary contains the given symbol. */
  bool contains(size_t symbol) const;
  /** Returns @c true if the dictionary contains the given word. */
  bool contains(const QString &word) const;
  /** Search for a word in the dictionary. */
  size_t search(const QString &word) const;
  /** Search for a symbol in the dictionary. */
  QString search(size_t symbol) const;
  /** Serializes the dictionary into the given device. */
  void serialize(QIODevice &device);
  /** Reads a dictionary form the given device. */
  void read(QIODevice &device);
  /** Clears the dictionary. */
  void clear();

protected:
  /** Mapping word->symbol. */
  QHash<QString, size_t> _table;
  /** Mapping symbol->word. */
  QVector<QString> _symbols;
};


/** The MegaHAL model. Use @c learn to train it some sentences and then @c reply to generate
 * replies. The model can be serialized into a @c QIODevice with the @c serialize method. */
class QHalModel
{
public:
  /** Loads a previously serialized model. */
  QHalModel(QIODevice &device);
  /** Creates an empty model. */
  QHalModel(uint8_t order=5);
  /** Destructor. */
  ~QHalModel();

  /** Learn something. */
  void learn(const QString &sentence);
  /** Learn from and reply to the given sentence.*/
  QString reply(const QString &sentence);

  /** Reads a model from the given device. */
  void read(QIODevice &device);
  /** Serializes the model into the given buffer. */
  void serialize(QIODevice &device);
  /** Clears the model. */
  void clear();

protected:
  /** Resets the current context. */
  void resetContext();
  /** Learns the tokenized sentence. */
  void learn(const QStringList &sentence);
  /** Update the current model with the given symbol. */
  void update(size_t symbol);
  /** Updates the current context. */
  void updateContext(size_t symbol);
  /** Assembles a response form the given tokenized sentence. */
  QString reply(const QStringList &words);
  /** Constructs a list of keywords from the given list of tokens. */
  QStringList makeKeywords(const QStringList &tokens);
  /** Assembles a tokenized response from the given keywords. */
  QStringList makeReply(const QStringList &keywords);
  /** Assembles a random response form the given keywords. */
  size_t seed(const QStringList &keywords);
  /** Assembles a response form the given keywords. */
  size_t babble(const QStringList &keywords, const QStringList &replies);
  /** Evaluate the entropy of the response. */
  float evaluateReply(const QStringList &keywords, const QStringList &replies);
  /** Constructs a string from the tokenized wordlist. */
  QString makeOutput(const QStringList &words);

protected:
  /** The order of the model (defaul 5). */
  uint8_t  _order;
  /** The forward n-gram tree. */
  QHalTree _forward;
  /** The backward n-gram tree. */
  QHalTree _backward;
  /** Current context. */
  QVector<QHalTree *> _context;
  /** The dictionary of symbols (words and non-words). */
  QHalDict _dictionary;
};


#endif
