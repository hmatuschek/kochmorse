#include "qhal.hh"
#include <QListIterator>
#include <QString>
#include <cmath>
#include <QDebug>
#include <QBuffer>
#include <QIODevice>


/* ******************************************************************************************** *
 * Implementation some helper functions
 * ******************************************************************************************** */
void
toWords(const QString text, QStringList &words) {
  if (0 == text.size()) { return; }

  words = text.split(QRegExp("\\b"));
  if (words.last() != ".") { words.append("."); }
  return;
}



/* ******************************************************************************************** *
 * Implementation of QHalTree
 * ******************************************************************************************** */
QHalTree::QHalTree()
  : _symbol(0), _usage(0), _count(0), _symbols(), _branches()
{
  // pass...
}

QHalTree::QHalTree(size_t symbol)
  : _symbol(symbol), _usage(0), _count(0), _symbols(), _branches()
{
  // pass...
}

QHalTree::QHalTree(QIODevice &device)
  : _symbol(0), _usage(0), _count(0), _symbols(), _branches()
{
  read(device);
}

QHalTree::~QHalTree() {
  clear();
}

QHalTree *
QHalTree::addSymbol(size_t symbol) {
  QHalTree *node = findSymbolAdd(symbol);
  if (node->_count < std::numeric_limits<size_t>::max()) {
    node->_count++; _usage++;
  }
  return node;
}

QHalTree *
QHalTree::findSymbolAdd(size_t symbol) {
  QHash<size_t, size_t>::iterator item = _symbols.find(symbol);
  if (item != _symbols.end()) { return _branches[item.value()]; }
  QHalTree *node = new QHalTree(symbol);
  size_t idx = _branches.size();
  _branches.append(node);
  _symbols[symbol] = idx;
  return node;
}

QHalTree *
QHalTree::findSymbol(size_t symbol) {
  QHash<size_t, size_t>::iterator item = _symbols.find(symbol);
  if (item != _symbols.end()) { return _branches[item.value()]; }
  return 0;
}

bool
QHalTree::empty() const {
  return _branches.empty();
}

size_t
QHalTree::size() const {
  return _branches.size();
}

size_t
QHalTree::count() const {
  return _count;
}

size_t
QHalTree::usage() const {
  return _usage;
}

size_t
QHalTree::symbol() const {
  return _symbol;
}

QHalTree *
QHalTree::random() const {
  return _branches[rand()%_branches.size()];
}

QHalTree *
QHalTree::branch(size_t idx) const {
  return _branches[idx];
}

void
QHalTree::read(QIODevice &device) {
  // First, clear tree
  clear();

  // Read symbol
  device.read((char *)&_symbol, sizeof(size_t));
  // Read usage
  device.read((char *)&_usage, sizeof(size_t));
  // Read count
  device.read((char *)&_count, sizeof(size_t));

  // Read branches
  size_t n=0; device.read((char *)&n, sizeof(size_t));
  for (int i=0; i<n; i++) {
    QHalTree *node = new QHalTree(device);
    _branches.push_back(node);
    _symbols[node->symbol()] = i;
  }
}

void
QHalTree::serialize(QIODevice &device) {
  // Serialize symbol
  device.write((char *)&_symbol, sizeof(size_t));
  // Serialize usage
  device.write((char *)&_usage, sizeof(size_t));
  // Serialize count
  device.write((char *)&_count, sizeof(size_t));

  // serialize branch count
  size_t n = _branches.size();
  device.write((char *)&n, sizeof(size_t));
  // serialize branches
  for (int i=0; i<n; i++) {
    _branches[i]->serialize(device);
  }
}

void
QHalTree::clear() {
  // Free all branches
  for (int i=0; i<_branches.size(); i++) {
    delete _branches[i];
  }
  // clear data structures
  _branches.clear(); _symbols.clear();
  _symbol = 0; _usage = 0; _count = 0;
}



/* ******************************************************************************************** *
 * Implementation of QHalDict
 * ******************************************************************************************** */
QHalDict::QHalDict()
  : _table(), _symbols()
{
  _symbols.append("<ERROR>");
  _symbols.append("<FIN>");
}

QHalDict::QHalDict(QIODevice &device)
  : _table(), _symbols()
{
  read(device);
}

QHalDict::~QHalDict() {
  // pass...
}

size_t
QHalDict::addWord(const QString &word) {
  QHash<QString, size_t>::iterator item = _table.find(word);
  if (item != _table.end()) { return item.value(); }
  size_t idx = _symbols.size();
  _table[word] = idx;
  _symbols.append(word);
  return idx;
}

bool
QHalDict::contains(const QString &word) const {
  return _table.contains(word);
}

bool
QHalDict::contains(size_t symbol) const {
  return _symbols.size() < symbol;
}

size_t
QHalDict::search(const QString &word) const {
  return _table[word];
}

QString
QHalDict::search(size_t symbol) const {
  return _symbols[symbol];
}

void
QHalDict::read(QIODevice &device) {
  // Clear table.
  clear();
  // Read number of elements
  size_t n=0; device.read((char *)&n, sizeof(size_t));
  // Read elements
  for (int symbol=0; symbol<n; symbol++) {
    // Read string size
    size_t s=0; device.read((char *)&s, sizeof(size_t));
    // Read encoded string
    QByteArray enc = device.read(s);
    // Get string
    QString str = QString::fromLocal8Bit(enc);
    // store
    _table.insert(str, symbol);
    _symbols.push_back(str);
  }
}

void
QHalDict::serialize(QIODevice &device) {
  // serialize number of elements
  size_t n = _symbols.size();
  device.write((char *)&n, sizeof(size_t));
  // serialize strings
  for (size_t i=0; i<_symbols.size(); i++) {
    // Encode string in local 8bit form (i.e. UTF-8)
    QByteArray tmp = _symbols[i].toLocal8Bit();
    // Get size
    n = tmp.size();
    // Store size
    device.write((char *)&n, sizeof(size_t));
    // Store encoded string
    device.write(tmp.data(), n);
  }
}

void
QHalDict::clear() {
  _table.clear();
  _symbols.clear();
}


/* ******************************************************************************************** *
 * Implementation of QHalModel
 * ******************************************************************************************** */
QHalModel::QHalModel(uint8_t order)
  : _order(order), _forward(), _backward(), _context(_order+2, 0)
{
  // pass...
}

QHalModel::QHalModel(QIODevice &device)
  : _order(0), _forward(), _backward(), _context()
{
  read(device);
}

QHalModel::~QHalModel() {
  // pass...
}


void
QHalModel::learn(const QString &sentence) {
  // Turn into lower-case
  QString text = sentence.toLower();
  // Split into words
  QStringList words;
  toWords(text, words);
  // Learn
  learn(words);
}


QString
QHalModel::reply(const QString &sentence) {
  // Turn into lower-case
  QString text = sentence.toLower();
  // Split into words
  QStringList words;
  toWords(text, words);
  // Learn
  learn(words);
  // Create response
  return reply(words);
}


void
QHalModel::resetContext() {
  for (int i=0; i<=_order; i++) { _context[i] = 0; }
}


void
QHalModel::learn(const QStringList &sentence) {
  /* We only learn from inputs which are long enough */
  if (sentence.size()<=_order)
    return;

  /* Train the model in the forwards direction.  Start by initializing
   * the context of the model. */
  resetContext();
  _context[0] = &_forward;
  QListIterator<QString> word(sentence); word.toFront();
  while (word.hasNext()) {
    /* Add the symbol to the model's dictionary if necessary, and then
     * update the forward model accordingly. */
    size_t symbol = _dictionary.addWord(word.next());
    update(symbol);
  }
  /* Add the sentence-terminating symbol. */
  update(1);

  /* Train the model in the backwards direction.  Start by initializing
   * the context of the model. */
  resetContext();
  _context[0] = &_backward;
  word = sentence; word.toBack();
  while (word.hasPrevious()) {
    /* Find the symbol in the model's dictionary, and then update
     * the backward model accordingly. */
    size_t symbol = _dictionary.search(word.previous());
    update(symbol);
  }
  /* Add the sentence-terminating symbol. */
  update(1);
  // done.
  return;
}


QString
QHalModel::reply(const QStringList &words) {
  QStringList replywords;
  QStringList keywords;
  float surprise;
  QString output;
  QString outputNone = "I don't know enough to answer you yet!";

  /* Make sure some sort of reply exists */
  output=outputNone;
  replywords = makeReply(keywords);
  if(words != replywords)
    output = makeOutput(replywords);

  /* Create a list of keywords from the words in the user's input */
  keywords = makeKeywords(words);

  // Try it several times.
  // Basically a trivial Monte-Carlo optimization
  float max_surprise = (float)-1.0;
  for (int count=0; count<100; count++) {
    replywords = makeReply(keywords);
    surprise = evaluateReply(keywords, replywords);
    if ( (surprise>max_surprise) && (words != replywords)) {
      max_surprise = surprise;
      output = makeOutput(replywords);
    }
  }
  /* Return the best answer we generated so far. */
  return(output);
}


QStringList
QHalModel::makeKeywords(const QStringList &tokens) {
  QStringList keys;
  // Extract all words in dictionary (skipping non-words).
  foreach (QString word, tokens) {
    if (word.isEmpty()) { continue; }
    if (! word[0].isLetter()) { continue; }
    if (_dictionary.contains(word)) {
      keys.push_back(word);
    }
  }
  return keys;
}


QStringList
QHalModel::makeReply(const QStringList &keywords) {
  QStringList replies;
  int symbol;
  bool start=true;

  /* Start off by making sure that the model's context is empty. */
  resetContext();
  _context[0] = &_forward;

  /* Generate the reply in the forward direction. */
  while (true) {
    /* Get a random symbol from the current context. */
    if (start)
      symbol = seed(keywords);
    else
      symbol = babble(keywords, replies);
    if ( (symbol==0)||(symbol==1) )
      break;
    start=false;

    /* Append the symbol to the reply dictionary. */
    replies.append(_dictionary.search(symbol));

    /* Extend the current context of the model with the current symbol. */
    updateContext(symbol);
  }

  /* Start off by making sure that the model's context is empty. */
  resetContext();
  _context[0] = &_backward;

  /* Re-create the context of the model from the current reply
   * dictionary so that we can generate backwards to reach the
   * beginning of the string. */
  if(replies.size()) {
    for(int i=std::min(replies.size()-1, int(_order)); i>=0; --i) {
      symbol = _dictionary.search(replies[i]);
      updateContext(symbol);
    }
  }

  /* Generate the reply in the backward direction. */
  while (true) {
    /* Get a random symbol from the current context. */
    symbol = babble(keywords, replies);
    if ( (symbol==0) || (symbol==1) ) break;

    /* Prepend the symbol to the reply dictionary. */
    replies.prepend(_dictionary.search(symbol));

    /* Extend the current context of the model with the current symbol. */
    updateContext(symbol);
  }

  return(replies);
}


size_t
QHalModel::seed(const QStringList &keywords) {
  size_t symbol;
  unsigned int stop;

  /* Fix, thanks to Mark Tarrabain */
  if (_context[0]->empty()) {
    symbol = 0;
  } else {
    symbol = _context[0]->random()->symbol();
  }

  if (keywords.size()) {
    int i = rand()%keywords.size();
    stop=i;
    while (true) {
      if( _dictionary.contains(keywords[i]) ) {
        symbol = _dictionary.search(keywords[i]);
        return(symbol);
      }
      ++i;
      if(i==keywords.size()) i=0;
      if(i==stop) return(symbol);
    }
  }

  return symbol;
}


size_t
QHalModel::babble(const QStringList &keywords, const QStringList &replies) {
  QHalTree *node = 0;
  size_t symbol = 0;

  /* Select the longest available context. */
  for(int i=0; i<=_order; ++i) {
    if(_context[i]!=NULL)
      node = _context[i];
  }
  // If empty -> abort
  if (node->empty()) return(0);

  /* Choose a symbol at random from this context. */
  int i = rand()%node->size();
  int count = rand()%node->usage();

  while (count>=0) {
    /* If the symbol occurs as a keyword, then use it. */
    symbol = node->branch(i)->symbol();
    if ( keywords.contains(_dictionary.search(symbol)) &&
         !replies.contains(_dictionary.search(symbol)) ) {
      break;
    }
    count -= node->branch(i)->count();
    i = (i>=(node->size()-1)) ? 0 : (i+1);
  }

  return(symbol);
}


float
QHalModel::evaluateReply(const QStringList &keys, const QStringList &words) {
  int symbol;
  float probability;
  int count;
  float entropy = 0.0;
  QHalTree *node;
  int num=0;

  if (words.size()<=0) return 0.0 ;

  resetContext();
  _context[0] = &_forward;
  for (int i=0; i<words.size(); ++i) {
    symbol = _dictionary.search(words[i]);

    if (keys.contains(words[i])) {
      probability=(float)0.0;
      count=0;
      ++num;
      for (int j=0; j<_order; ++j) {
        if (_context[j]) {
          node = _context[j]->findSymbol(symbol);
          probability += float(node->count()) / float(_context[j]->usage());
          ++count;
        }
      }

      if (count > 0) {
        entropy -= std::log(probability/float(count));
      }
    }
    updateContext(symbol);
  }

  resetContext();
  _context[0] = &_backward;
  for (int k=(words.size()-1); k>=0; --k) {
    symbol = _dictionary.search(words[k]);
    if(keys.contains(words[k])) {
      probability = 0.0; count = 0; ++num;
      for (int j=0; j<_order; ++j){
        if(_context[j] != NULL) {
          node = _context[j]->findSymbol(symbol);
          probability += float(node->count()) / float(_context[j]->usage());
          ++count;
        }
      }
      if (count>0.0) {
        entropy-=(float)log(probability/(float)count);
      }
    }

    updateContext(symbol);
  }

  if (num>=8)
    entropy /= (float)std::sqrt(float(num-1));
  if (num>=16)
    entropy /= (float)num;

  return entropy;
}


QString
QHalModel::makeOutput(const QStringList &words) {
  return words.join("");
}


void
QHalModel::update(size_t symbol) {
  /* Update all of the models in the current context with the specified
   * symbol. */
  for (int i=(_order+1); i>0; --i) {
    if (_context[i-1]) {
      _context[i] = _context[i-1]->addSymbol(symbol);
    }
  }
  // Done...
  return;
}


void
QHalModel::updateContext(size_t symbol) {
  /* Update all of the models in the current context with the specified
   * symbol. */
  for (int i=(_order+1); i>0; --i) {
    if (_context[i-1]) {
      _context[i] = _context[i-1]->findSymbol(symbol);
    }
  }
  // Done...
  return;
}


void
QHalModel::read(QIODevice &device) {
  // Clear model
  clear();

  // Read order
  device.read((char *)&_order, sizeof(uint8_t));

  // Allocate context
  _context.resize(_order+2);
  resetContext();

  // read forward tree
  _forward.read(device);

  // read backward tree
  _backward.read(device);

  // read dictionary
  _dictionary.read(device);
}


void
QHalModel::serialize(QIODevice &device) {
  // Serialize order
  device.write((char *)&_order, sizeof(uint8_t));
  // serialize forward tree.
  _forward.serialize(device);
  // serialize backward tree.
  _backward.serialize(device);
  // serialize dictionary.
  _dictionary.serialize(device);
}


void
QHalModel::clear() {
  resetContext();
  _forward.clear();
  _backward.clear();
  _dictionary.clear();
}
