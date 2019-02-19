#include "textgen.hh"
#include <random>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QDate>
#include <cmath>


/* ********************************************************************************************* *
 * Implementation of TextGenRule
 * ********************************************************************************************* */
TextGenRule::TextGenRule(QObject *parent)
  : QObject(parent)
{
	// pas...
}


/* ********************************************************************************************* *
 * Implementation of TextGenTextRule
 * ********************************************************************************************* */
TextGenTextRule::TextGenTextRule(const QString &text, QObject *parent)
  : TextGenRule(parent), _text(text)
{
	// pass...
}

void
TextGenTextRule::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
	buffer << _text;
}


/* ********************************************************************************************* *
 * Implementation of TextGenAnyLetterRule
 * ********************************************************************************************* */
TextGenAnyLetterRule::TextGenAnyLetterRule(QObject *parent)
  : TextGenRule(parent)
{
	// pass...
}

void
TextGenAnyLetterRule::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
    // Seed with a real random value, if available
    std::random_device r;
    std::default_random_engine entr(r());
    std::uniform_int_distribution<char> rnd(0, 25);
    char c = 'a' + rnd(entr);
    buffer << QChar(c);
}


/* ********************************************************************************************* *
 * Implementation of TextGenAnyNumberRule
 * ********************************************************************************************* */
TextGenAnyNumberRule::TextGenAnyNumberRule(QObject *parent)
  : TextGenRule(parent)
{
	// pass...
}

void
TextGenAnyNumberRule::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
    // Seed with a real random value, if available
    std::random_device r;
    std::default_random_engine entr(r());
    std::uniform_int_distribution<char> rnd(0, 9);
    char c = '0' + rnd(entr);
    buffer << QChar(c);
}


/* ********************************************************************************************* *
 * Implementation of TextGenOptRule
 * ********************************************************************************************* */
TextGenOptRule::TextGenOptRule(TextGenRule *rule, double p, QObject *parent)
  : TextGenRule(parent), _p(p), _rule(rule)
{
  // pass...
}

void
TextGenOptRule::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
  // Seed with a real random value, if available
  std::random_device r;
  // Choose a random mean between 1 and 6
  std::default_random_engine entr(r());
  std::bernoulli_distribution rnd(_p);
  if (rnd(entr))
    _rule->generate(buffer, ctx);
}


/* ********************************************************************************************* *
 * Implementation of TextGenOneOfRule
 * ********************************************************************************************* */
TextGenOneOfRule::TextGenOneOfRule(QObject *parent)
  : TextGenRule(parent), _rules()
{
	// pass...
}

void
TextGenOneOfRule::addRule(double weight, TextGenRule *rule) {
	if (0 == rule)
		return;
	rule->setParent(this);
	_weights.append(weight); _rules.append(rule);
}

void
TextGenOneOfRule::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
  // Seed with a real random value, if available
  std::random_device r;
  // Choose a random mean between 1 and 6
  std::default_random_engine entr(r());
  std::discrete_distribution<int> rnd(_weights.begin(), _weights.end());
  _rules[rnd(entr)]->generate(buffer, ctx);
}


/* ********************************************************************************************* *
 * Implementation of TextGenOneZipfOfRule
 * ********************************************************************************************* */
TextGenOneOfZipfRule::TextGenOneOfZipfRule(double exp, QObject *parent)
  : TextGenRule(parent), _exp(exp), _rules()
{
	// pass...
}

void
TextGenOneOfZipfRule::addRule(TextGenRule *rule) {
	if (0 == rule)
		return;
	rule->setParent(this);
  _rules.append(rule);
	_weights.append(1./std::pow(double(_rules.size()), _exp));
}

void
TextGenOneOfZipfRule::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
  // Seed with a real random value, if available
  std::random_device r;
  // Choose a random mean between 1 and 6
  std::default_random_engine entr(r());
  std::discrete_distribution<int> rnd(_weights.begin(), _weights.end());
  _rules[rnd(entr)]->generate(buffer, ctx);
}


/* ********************************************************************************************* *
 * Implementation of TextGenRepeatRule
 * ********************************************************************************************* */
TextGenRepeatRule::TextGenRepeatRule(size_t nmin, size_t nmax, TextGenRule *rule, QObject *parent)
  : TextGenRule(parent), _nmin(nmin), _nmax(nmax), _rule(rule)
{
  if (_rule)
    _rule->setParent(this);
}

void
TextGenRepeatRule::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
  if (0 == _rule)
    return;

  // Pick a random number in [min, max]
  // Seed with a real random value, if available
  std::random_device r;
  // Choose a random mean between 1 and 6
  std::default_random_engine entr(r());
  std::uniform_int_distribution<int> rnd(_nmin, _nmax);
  size_t n = rnd(entr);
  for (size_t i=0; i<n; i++) {
    _rule->generate(buffer, ctx);
  }
}


/* ********************************************************************************************* *
 * Implementation of TextGenVariable
 * ********************************************************************************************* */
TextGenVariable::TextGenVariable(const QString &id, TextGenRule *rule, QObject *parent)
  : TextGenRule(parent), _id(id), _rule(rule)
{
  // pass...
}

void
TextGenVariable::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
  QString tmp; QTextStream stream(&tmp);
  if (_rule)
    _rule->generate(stream, ctx);
  ctx[_id] = tmp;
  //qDebug() << "TextGen: set var" << _id << "to" << tmp;
}


/* ********************************************************************************************* *
 * Implementation of TextGenCondRule
 * ********************************************************************************************* */
TextGenCondRule::TextGenCondRule(const QString &var, const QString &value, TextGenRule *rule, QObject *parent)
  : TextGenRule(parent), _isDefined(false), _var(var), _value(value), _rule(rule)
{
  // pass...
}

TextGenCondRule::TextGenCondRule(const QString &var, TextGenRule *rule, QObject *parent)
  : TextGenRule(parent), _isDefined(true), _var(var), _value(), _rule(rule)
{
  // pass...
}

void
TextGenCondRule::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
  if (_rule && _isDefined && ctx.contains(_var))
    _rule->generate(buffer, ctx);
  if (_rule && (!_isDefined) && ctx.contains(_var) && (_value == ctx[_var]))
    _rule->generate(buffer, ctx);
}


/* ********************************************************************************************* *
 * Implementation of TextGenRefRule
 * ********************************************************************************************* */
TextGenRefRule::TextGenRefRule(const QString &id, QObject *parent)
  : TextGenRule(parent), _id(id)
{
  // pass...
}

void
TextGenRefRule::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
  if (ctx.contains(_id))
    buffer << ctx[_id];
}


/* ********************************************************************************************* *
 * Implementation of TextGenListRule
 * ********************************************************************************************* */
TextGenListRule::TextGenListRule(QObject *parent)
  : TextGenRule(parent), _rules()
{
  // pass...
}

TextGenListRule::TextGenListRule(const QList<TextGenRule *> &rules, QObject *parent)
  : TextGenRule(parent), _rules(rules)
{
  // pass...
}

void
TextGenListRule::generate(QTextStream &buffer, QHash<QString, QString> &ctx) {
  foreach(TextGenRule *rule, _rules) {
    if (rule)
      rule->generate(buffer, ctx);
  }
}


/* ********************************************************************************************* *
 * Implementation of TextGen::Context
 * ********************************************************************************************* */
TextGen::Context::Context()
  : QHash<QString, QString>()
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  switch (now.date().month()) {
    case 12:
    case 1:
    case 2:
      insert("ToY","winter");
      break;
    case 3:
    case 4:
    case 5:
      insert("ToY","spring");
      break;
    case 6:
    case 7:
    case 8:
      insert("ToY","summer");
      break;
    case 9:
    case 10:
    case 11:
      insert("ToY","fall");
      break;
    default: break;
  }
  if ((now.time().hour()>=6) && (now.time().hour()<12)) {
    insert("ToD","gm");
  } else if ((now.time().hour()>=12) && (now.time().hour()<15)) {
    insert("ToD","gd");
  } if ((now.time().hour()>=15) && (now.time().hour()<18)) {
    insert("ToD","ga");
  } if ((now.time().hour()>=18) && (now.time().hour()<23)) {
    insert("ToD","ge");
  } else {
    insert("ToD","gn");
  }
}

TextGen::Context::Context(const Context &other)
  : QHash<QString, QString>(other)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of TextGen
 * ********************************************************************************************* */
TextGen::TextGen(const QString &filename, QObject *parent)
  : TextGenListRule(parent), _pathStack()
{
	load(filename);
}

TextGen::TextGen(QXmlStreamReader &reader, QObject *parent)
  : TextGenListRule(parent), _pathStack()
{
	parse(reader);
}

bool
TextGen::load(QString filename) {
  QFileInfo info(filename);
  if ((! info.exists()) && (! _pathStack.isEmpty())) {
    filename = _pathStack.back() + "/" + filename;
    info.setFile(filename);
  }


  QFile file(filename);
  _pathStack.push_back(info.dir().absolutePath());
  if (! file.open(QIODevice::ReadOnly)) {
    qDebug() << "... cannot open file" << filename;
    _pathStack.pop_back();
    return false;
  }

  bool success = false;
  if ("txt" == info.suffix()) {
    _rules.append(new TextGenTextRule(QString::fromUtf8(file.readAll()), this));
    success = true;
  } else {
    QXmlStreamReader reader(&file);
    success = parse(reader);
    if (reader.hasError())
      qDebug() << "..." << reader.errorString();
  }
  _pathStack.pop_back();

  return success;
}

bool
TextGen::parse(QXmlStreamReader &reader) {
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isStartElement() && ("rules" == reader.name())) {
      // Handle document element
      this->parseRules(reader, _rules);
    } else if (reader.isStartElement()) {
      // Handle error
      reader.raiseError(tr("Unexpected element at line %1: %2")
                        .arg(reader.lineNumber()).arg(reader.name().toString()));
      break;
    } else if (reader.isEndElement()) {
      break;
    }
  }

  if (reader.hasError())
    return false;

  return true;
}

void
TextGen::parseRules(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isStartElement()) {
      if ("rule" == reader.name()) {
        this->parseRule(reader, rules);
      } else if ("load" == reader.name()) {
        this->parseLoad(reader, rules);
      } else if ("if" == reader.name()) {
        this->parseIf(reader, rules);
      } else if ("var" == reader.name()) {
        this->parseVar(reader, rules);
      } else if ("opt" == reader.name()) {
        this->parseOptRule(reader, rules);
      } else if ("one-of" == reader.name()) {
        this->parseOneOf(reader, rules);
      } else if ("one-of-zipf" == reader.name()) {
        this->parseOneOfZipf(reader, rules);
      } else if ("rep" == reader.name()) {
        this->parseRep(reader, rules);
      } else if ("any-letter" == reader.name()) {
        this->parseAnyLetter(reader, rules);
      } else if ("any-number" == reader.name()) {
        this->parseAnyNumber(reader, rules);
      } else if ("bt" == reader.name()) {
        this->parseBT(reader, rules);
      } else if ("bk" == reader.name()) {
        this->parseBK(reader, rules);
      } else if ("ar" == reader.name()) {
        this->parseAR(reader, rules);
      } else if ("sk" == reader.name()) {
        this->parseSK(reader, rules);
      } else if ("t" == reader.name()) {
        this->parseText(reader, rules);
      } else if ("p" == reader.name()) {
        this->parsePause(reader, rules);
      } else if ("stop" == reader.name()) {
        this->parseStop(reader, rules);
      } else if ("ref" == reader.name()) {
        this->parseRef(reader, rules);
      } else if ("apply" == reader.name()) {
        this->parseApply(reader, rules);
      } else {
        // Handle error
        reader.raiseError(tr("Unexpected element at line %1: %2")
                          .arg(reader.lineNumber()).arg(reader.name().toString()));
      }
    } else if (reader.isEndElement()) {
      return;
    }
  }
}

void
TextGen::parseLoad(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  if (! reader.attributes().hasAttribute("file")) {
    reader.raiseError(tr("'load' element is missing the 'file' attribute"));
  }
  // Load file
  load(reader.attributes().value("file").toString());
  // Check if element is empty...
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Handle error
      reader.raiseError(tr("Unexpected element '%2'").arg(reader.name().toString()));
      return;
    }
  }
}

void
TextGen::parseRule(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  if (! reader.attributes().hasAttribute("id")) {
    reader.raiseError(tr("<rule> has no 'id' attribute at line %1").arg(reader.lineNumber()));
    return;
  }
  // Get rule identifier
  QString id = reader.attributes().value("id").toString();
  // Parse rule
  QList<TextGenRule *> subrules;
  this->parseRules(reader, subrules);
  // Store rule in context for reference
  _context[id] = new TextGenListRule(subrules, this);
}

void
TextGen::parseIf(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  // Get variable name
  if (! reader.attributes().hasAttribute("var")) {
    reader.raiseError(tr("<if> has no 'var' attribute"));
    return;
  }
  QString var   = reader.attributes().value("var").toString();
  QString value;
  bool hasMatches = reader.attributes().hasAttribute("matches");
  if (hasMatches)
    value = reader.attributes().value("matches").toString();

  // Parse sub rules
  QList<TextGenRule *> subrules;
  this->parseRules(reader, subrules);

  // If there is a matches attribue ...
  if (hasMatches) {
    // Append condition
    rules.append(new TextGenCondRule(var, value, new TextGenListRule(subrules, this), this));
  } else {
    // Append condition
    rules.append(new TextGenCondRule(var, new TextGenListRule(subrules, this), this));
  }
}

void
TextGen::parseVar(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  // Check presence of 'id' attribute
  if (! reader.attributes().hasAttribute("id")) {
    reader.raiseError(tr("<var> has no 'id' attribute at line %1").arg(reader.lineNumber()));
    return;
  }

  // Get variable identifier
  QString id = reader.attributes().value("id").toString();
  // Parse variable definition
  QList<TextGenRule *> subrules;
  this->parseText(reader, subrules);
  rules.append(new TextGenVariable(id, new TextGenListRule(subrules, this), this));
}

void
TextGen::parseOptRule(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  double p = 0.5;
  if (reader.attributes().hasAttribute("p"))
    p = reader.attributes().value("p").toDouble();
  QList<TextGenRule *> subrules;
  this->parseText(reader, subrules);
  rules.push_back(new TextGenOptRule(new TextGenListRule(subrules, this), p, this));
}

void
TextGen::parseOneOf(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  TextGenOneOfRule *rule = new TextGenOneOfRule(this);
  rules.push_back(rule);

  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isStartElement() && ("i" == reader.name())) {
      parseOneOfItem(reader, rule);
    } else if (reader.isStartElement() && ("t" == reader.name())) {
      parseOneOfText(reader, rule);
    } else if (reader.isEndElement()) {
      return;
    } else if (reader.isStartElement()){
      // Handle error
      reader.raiseError(tr("Unexpected element at line %1: %2")
                        .arg(reader.lineNumber()).arg(reader.name().toString()));
    }
  }
}

void
TextGen::parseOneOfItem(QXmlStreamReader &reader, TextGenOneOfRule *rule) {
  double w = 1;
  if (reader.attributes().hasAttribute("w")) {
    bool ok;
    double tmp = reader.attributes().value("w").toDouble(&ok);
    if (ok)
      w = tmp;
  }

  QList<TextGenRule *> rules;
  parseRules(reader, rules);
  rule->addRule(w, new TextGenListRule(rules, this));
}

void
TextGen::parseOneOfText(QXmlStreamReader &reader, TextGenOneOfRule *rule) {
  double w = 1;
  if (reader.attributes().hasAttribute("w")) {
    bool ok;
    double tmp = reader.attributes().value("w").toDouble(&ok);
    if (ok)
      w = tmp;
  }

  QList<TextGenRule *> rules;
  parseText(reader, rules);
  rule->addRule(w, new TextGenListRule(rules));
}

void
TextGen::parseOneOfZipf(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  double exp = 1;
  if (reader.attributes().hasAttribute("exp")) {
    bool ok;
    double tmp = reader.attributes().value("exp").toDouble(&ok);
    if (ok)
      exp = tmp;
  }

  TextGenOneOfZipfRule *rule = new TextGenOneOfZipfRule(exp, this);
  rules.push_back(rule);

  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isStartElement() && ("i" == reader.name())) {
      parseOneOfZipfItem(reader, rule);
    } else if (reader.isStartElement() && ("t" == reader.name())) {
      parseOneOfZipfText(reader, rule);
    } else if (reader.isEndElement()) {
      return;
    } else if (reader.isStartElement()){
      // Handle error
      reader.raiseError(tr("Unexpected element at line %1: %2")
                        .arg(reader.lineNumber()).arg(reader.name().toString()));
    }
  }
}

void
TextGen::parseOneOfZipfItem(QXmlStreamReader &reader, TextGenOneOfZipfRule *rule) {
  QList<TextGenRule *> rules;
  parseRules(reader, rules);
  rule->addRule(new TextGenListRule(rules, this));
}

void
TextGen::parseOneOfZipfText(QXmlStreamReader &reader, TextGenOneOfZipfRule *rule) {
  QList<TextGenRule *> rules;
  parseText(reader, rules);
  rule->addRule(new TextGenListRule(rules));
}

void
TextGen::parseRep(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  // Get min
  if (! reader.attributes().hasAttribute("min")) {
    reader.raiseError(tr("<rep> has no 'min' attribute"));
    return;
  }
  size_t min = reader.attributes().value("min").toUInt();
  size_t max = min;
  if (reader.attributes().hasAttribute("max"))
    max = reader.attributes().value("max").toUInt();

  // Parse sub rules
  QList<TextGenRule *> subrules;
  this->parseRules(reader, subrules);

  // Append condition
  rules.append(new TextGenRepeatRule(min, max, new TextGenListRule(subrules, this), this));
}

void
TextGen::parseAnyNumber(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  rules.append(new TextGenAnyNumberRule(this));
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Handle error
      reader.raiseError(tr("Unexpected element '%2'").arg(reader.name().toString()));
      return;
    }
  }
}

void
TextGen::parseAnyLetter(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  rules.append(new TextGenAnyLetterRule(this));
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Handle error
      reader.raiseError(tr("Unexpected element '%2'").arg(reader.name().toString()));
      return;
    }
  }
}

void
TextGen::parseBT(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  rules.append(new TextGenTextRule("=", this));
  // Verify that element is empty:
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Handle error
      reader.raiseError(tr("Unexpected element '%2'").arg(reader.name().toString()));
      return;
    }
  }
}

void
TextGen::parseBK(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  rules.append(new TextGenTextRule(QChar(0x2417), this));
  // Verify that element is empty:
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Handle error
      reader.raiseError(tr("Unexpected element '%2'").arg(reader.name().toString()));
      return;
    }
  }
}

void
TextGen::parseAR(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  rules.append(new TextGenTextRule("+", this));
  // Verify that element is empty:
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Handle error
      reader.raiseError(tr("Unexpected element '%2'").arg(reader.name().toString()));
      return;
    }
  }
}

void
TextGen::parseSK(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  rules.append(new TextGenTextRule(QChar(0x2403), this));
  // Verify that element is empty:
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Handle error
      reader.raiseError(tr("Unexpected element '%2'").arg(reader.name().toString()));
      return;
    }
  }
}

void
TextGen::parsePause(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  rules.append(new TextGenTextRule("   ", this));
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Handle error
      reader.raiseError(tr("Unexpected element at line %1: %2")
                        .arg(reader.lineNumber()).arg(reader.name().toString()));
      return;
    }
  }
}


void
TextGen::parseStop(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  rules.append(new TextGenTextRule("   \n", this));
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Handle error
      reader.raiseError(tr("Unexpected element at line %1: %2")
                        .arg(reader.lineNumber()).arg(reader.name().toString()));
      return;
    }
  }
}


void
TextGen::parseText(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isStartElement()) {
      if ("one-of" == reader.name()) {
        this->parseOneOf(reader, rules);
      } else if ("if" == reader.name()) {
        this->parseIf(reader, rules);
      } else if ("any-letter" == reader.name()) {
        this->parseAnyLetter(reader, rules);
      } else if ("any-number" == reader.name()) {
        this->parseAnyNumber(reader, rules);
      } else if ("bt" == reader.name()) {
        this->parseBT(reader, rules);
      } else if ("bk" == reader.name()) {
        this->parseBK(reader, rules);
      } else if ("ar" == reader.name()) {
        this->parseAR(reader, rules);
      } else if ("sk" == reader.name()) {
        this->parseSK(reader, rules);
      } else if ("p" == reader.name()) {
        this->parsePause(reader, rules);
      } else if ("stop" == reader.name()) {
        this->parseStop(reader, rules);
      } else if ("ref" == reader.name()) {
        this->parseRef(reader, rules);
      } else if ("apply" == reader.name()) {
        this->parseApply(reader, rules);
      } else if ("opt" == reader.name()) {
        this->parseOptRule(reader, rules);
      } else {
        // Handle error
        reader.raiseError(tr("Unexpected element at line %1: %2")
                          .arg(reader.lineNumber()).arg(reader.name().toString()));
      }
    } else if (reader.isEndElement()) {
      return;
    } else if (reader.isCharacters()) {
      rules.append(new TextGenTextRule(reader.text().toString(), this));
    }
  }
}

void
TextGen::parseRef(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  if (! reader.attributes().hasAttribute("var")) {
    // Signal error
    reader.raiseError(tr("<ref> element needs 'var' attribue at line %1.")
                      .arg(reader.lineNumber()));
    return;
  }
  QString var = reader.attributes().value("var").toString();

  rules.append(new TextGenRefRule(var, this));
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Signal error
      reader.raiseError(tr("Unexpected element at line %1: %2")
                        .arg(reader.lineNumber()).arg(reader.name().toString()));
      return;
    }
  }
}

void
TextGen::parseApply(QXmlStreamReader &reader, QList<TextGenRule *> &rules) {
  if (! reader.attributes().hasAttribute("rule")) {
    // Signal error
    reader.raiseError(tr("<apply> element needs 'rule' attribue at line %1.")
                      .arg(reader.lineNumber()));
    return;
  }

  QString rule = reader.attributes().value("rule").toString();
  if (_context.contains(rule)) {
    rules.append(_context[rule]);
  } else {
    // Signal error
    reader.raiseError(tr("Unknown rule '%1' referenced at line %2.")
                      .arg(rule).arg(reader.lineNumber()));
    return;
  }

  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isEndElement())
      return;
    else if (reader.isStartElement()) {
      // Signal error
      reader.raiseError(tr("Unexpected element at line %1: %2")
                        .arg(reader.lineNumber()).arg(reader.name().toString()));
      return;
    }
  }
}

