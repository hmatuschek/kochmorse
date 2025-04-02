#ifndef TEXTGEN_HH
#define TEXTGEN_HH

#include <QObject>
#include <QTextStream>
#include <QHash>
#include <QVector>
#include <QXmlStreamReader>


/** Base class for all text generation rules. */
class TextGenRule: public QObject
{
	Q_OBJECT

protected:
  /** Hidden constructor. */
	explicit TextGenRule(QObject *parent=nullptr);

public:
  /** Needs to be implemented by all rules to generate the actual content. */
	virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx) = 0;
};


/** A simple rule that replicates some fixed text passed to the constructor. */
class TextGenTextRule: public TextGenRule
{
	Q_OBJECT

public:
  /** Constructs a simple rule that replicates given text. */
	TextGenTextRule(const QString &text, QObject *parent=nullptr);

	virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);

protected:
  /** Holds the text to replicate. */
	QString _text;
};


/** A simple rule that generates a random letter. */
class TextGenAnyLetterRule: public TextGenRule
{
  Q_OBJECT

public:
  /** Constructs the random letter rule. */
  explicit TextGenAnyLetterRule(QObject *parent=nullptr);

  virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);
};


/** A simple rule that generates a random number. */
class TextGenAnyNumberRule: public TextGenRule
{
  Q_OBJECT

public:
  /** Constructs the random number rule. */
  explicit TextGenAnyNumberRule(QObject *parent=nullptr);

  virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);
};


/** A simple rule that may generate some optional text. */
class TextGenOptRule: public TextGenRule
{
  Q_OBJECT

public:
  /** Constructs a new optional text rule.
   * @param rule Specifies the text rule.
   * @param p Specifies the probability to generate text from  @c rule.
   * @param parent Specifies the @c QObject parent. */
  TextGenOptRule(TextGenRule *rule, double p, QObject *parent=nullptr);

  virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);

protected:
  /** Holds the probability to generate text from rule @c _rule. */
  double _p;
  /** The text rule. */
  TextGenRule *_rule;
};


/** A rule that selects one sub-rule according to some weights associated with the sub-rules. */
class TextGenOneOfRule: public TextGenRule
{
	Q_OBJECT

public:
  /** Constructs an empty "one-of" rule. Use @c addRule to add rules. */
	TextGenOneOfRule(QObject *parent=nullptr);

  /** Adds the specified rule with the associated weight. */
	void addRule(double weight, TextGenRule *rule);

	virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);

protected:
  /** The vector of rule-weights. */
	QVector<double>       _weights;
  /** The vector of rules. */
	QVector<TextGenRule*> _rules;
};


/** A rule that selects one sub-rule according to weights associated with the sub-rules by means
 * of Zipf's Law. */
class TextGenOneOfZipfRule: public TextGenRule
{
	Q_OBJECT

public:
  /** Constructs an empty "one-of" rule. Use @c addRule to add rules. */
	TextGenOneOfZipfRule(double exp=1.0, QObject *parent=nullptr);

  /** Adds the specified rule with the associated weight. */
	void addRule(TextGenRule *rule);

	virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);

protected:
  double _exp;
  /** The vector of rule-weights. */
	QVector<double>       _weights;
  /** The vector of rules. */
	QVector<TextGenRule*> _rules;
};


/** This rule randomly repeates some other text rule. */
class TextGenRepeatRule: public TextGenRule
{
  Q_OBJECT

public:
  /** Constructs a repetition rule.
   * @param nmin Specifies the minimum number of repetitions.
   * @param nmax Specifies the maximum number of repetitions.
   * @param rule Specifies the text rule to repeat.
   * @param parent Specifies the @c QObject parent. */
  TextGenRepeatRule(size_t nmin, size_t nmax, TextGenRule *rule, QObject *parent=nullptr);

  virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);

protected:
  /** The minimum number of repetitions. */
  size_t _nmin;
  /** The maximum number of repetitions. */
  size_t _nmax;
  /** The text rule to repeat. */
  TextGenRule *_rule;
};


/** A variable definition "rule".
 * This rule does not generate any text itself but applies the associated sub-rule and assigns its
 * result to a variable. */
class TextGenVariable: public TextGenRule
{
  Q_OBJECT

public:
  /** Constructs the variable definition rule for the specified variable name (@c id). */
  TextGenVariable(const QString &id, TextGenRule *rule, QObject *parent=nullptr);

  virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);

protected:
  /** Holds the name of the variable. */
  QString _id;
  /** Holds the rule that generates the value, that gets assigned to the variable upon generation. */
  TextGenRule *_rule;
};


/** Implements a simple condition rule.
 * This rule generates some content using the associated sub-rule if the specified variable has
 * a certain value. */
class TextGenCondRule: public TextGenRule
{
  Q_OBJECT

public:
  /** Constructs the condition that applies if the specified @c var has the specified @c value. */
  TextGenCondRule(const QString &var, const QString &value, TextGenRule *rule, QObject *parent=nullptr);
  /** Constructs the condition that applies if the specified @c var is defined (irrespective of the actual value). */
  TextGenCondRule(const QString &var, TextGenRule *rule, QObject *parent=nullptr);

  virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);

protected:
  /** If true, the condition only checks if var is defined. */
  bool _isDefined;
  /** Holds the variable name. */
  QString _var;
  /** Holds the value, the variable must contains for a match. */
  QString _value;
  /** Holds a reference to the rule that gets applied of the variable matches. */
  TextGenRule *_rule;
};


/** This rule references a variable.
 * That is, it generates the content of the specified variable. */
class TextGenRefRule: public TextGenRule
{
  Q_OBJECT

public:
  /** Constructs a variable reference rule. */
  TextGenRefRule(const QString &id, QObject *parent=nullptr);

  virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);

protected:
  /** Holds the name of the variable. */
  QString _id;
};


/** A simple list of rules rule.
 * This rule generates text by concatinating the content generated by all sub-rules. */
class TextGenListRule: public TextGenRule
{
Q_OBJECT

protected:
  /** Constructs an empty list-rule. */
  explicit TextGenListRule(QObject *parent=nullptr);

public:
  /** Constructs a list-rule from the passed rules. */
  TextGenListRule(const QList<TextGenRule *> &rules, QObject *parent=nullptr);

  virtual void generate(QTextStream &buffer, QHash<QString, QString> &ctx);

protected:
  /** Weak references to the sub-rules. */
  QList<TextGenRule *> _rules;
};



/** This class parses a XML description of rules that form the text generators. */
class TextGen: public TextGenListRule
{
  Q_OBJECT

public:
  class Context: public QHash<QString, QString>
  {
  public:
    Context();
    Context(const Context &other);
  };

public:
  /** Constructs a text generator from the rules specified as XML in @c filename. */
  explicit TextGen(const QString &filename, QObject *parent = nullptr);
  /** Constructs a text generator from the rules specified as XML read by @c reader. */
  explicit TextGen(QXmlStreamReader &reader, QObject *parent = nullptr);

  /** Loads a XML files with rules and add them to the generator. */
  bool load(QString filename);
  /** Parses the XML description of generator rules read by @c reader. */
  bool parse(QXmlStreamReader &reader);

protected:
  /** Parses a list of rules. */
  void parseRules(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "load" instruction. */
  void parseLoad(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a single named-rule definition. */
  void parseRule(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "if"-condition rule. */
  void parseIf(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a variable definition rule. */
  void parseVar(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "opt" rule. */
  void parseOptRule(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "one-of" rule. */
  void parseOneOf(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "one-of" rule item. */
  void parseOneOfItem(QXmlStreamReader &reader, TextGenOneOfRule *rule);
  /** Parses a "one-of" rule text-item. */
  void parseOneOfText(QXmlStreamReader &reader, TextGenOneOfRule *rule);
  /** Parses a "one-of-zipf" rule. */
  void parseOneOfZipf(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "one-of-zipf" rule item. */
  void parseOneOfZipfItem(QXmlStreamReader &reader, TextGenOneOfZipfRule *rule);
  /** Parses a "one-of-zipf" rule text-item. */
  void parseOneOfZipfText(QXmlStreamReader &reader, TextGenOneOfZipfRule *rule);
  /** Parses a "rep" rule item. */
  void parseRep(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "any-letter" rule. */
  void parseAnyLetter(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "any-number" rule. */
  void parseAnyNumber(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "BT" prosign rule. */
  void parseBT(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "BK" prosign rule. */
  void parseBK(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "AR" prosign rule. */
  void parseAR(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "SK" prosign rule. */
  void parseSK(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "pause" rule. */
  void parsePause(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "stop" rule. */
  void parseStop(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "text" rule. */
  void parseText(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a variable reference rule. */
  void parseRef(QXmlStreamReader &reader, QList<TextGenRule *> &rules);
  /** Parses a "apply" rule. */
  void parseApply(QXmlStreamReader &reader, QList<TextGenRule *> &rules);

protected:
  /** Holds the named rules table. */
  QHash<QString, TextGenRule *> _context;
  /** Path stack for nested load instructions. */
  QList<QString> _pathStack;
};


#endif // TEXTGEN_HH
