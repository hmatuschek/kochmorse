#ifndef QSOCHAT_HH
#define QSOCHAT_HH

#include <QObject>
#include <QTextStream>
#include <QRegExp>
#include <QVector>
#include <QPair>


class Token
{
public:
  typedef enum {
    T_CALL, T_DE, T_CQ, T_RST, T_NAME, T_IS, T_HERE, T_QTH, T_WORD, T_NUMBER, T_BREAK, T_73, T_EOS
  } Type;

public:
  Token(Type type);
  Token(Type type, const QString &value);

  inline Type type() const { return _type; }
  inline const QString &value() const { return _value; }

protected:
  Type _type;
  QString _value;
};


class Lexer
{
public:
  Lexer(const QString &text);

  Token next();

protected:
  int _offset;
  const QString &_text;
  QVector< QPair<QRegExp, Token::Type> > _pattern;
};


class Parser
{
public:
  typedef enum {
    S_START, S_RESPONSE, S_CLOSING,
    S_CQ, S_CQ_CALL, S_CALL, S_NAME, S_RST, S_QTH
  } State;

public:
  Parser(QHash<QString, QString> &ctx);

  void parse(const QString &text);
  inline State state() const { return _state; }

protected:
  QHash<QString, QString> &_context;
  State _state;
};


class QSOChat: public QObject
{
	Q_OBJECT

public:
  explicit QSOChat(QObject *parent=0);

	inline QHash<QString, QString> &context() { return _context; }
	inline const QHash<QString, QString> &context() const { return _context; }

	bool handle(const QString &message, QTextStream &reply);

  void reset();

public:
	static bool isComplete(const QString &message);

protected:
  bool _inQSO;
	QHash<QString, QString> _context;
};

#endif // QSOPARSER_HH
