#ifndef QSOCHAT_HH
#define QSOCHAT_HH

#include <QObject>
#include <QTextStream>
#include <QRegularExpression>
#include <QVector>
#include <QPair>
#include <QXmlStreamReader>


class Token
{
public:
  Token();
  Token(const QString &id, const QString &value=0);
  Token(const Token &other);

  Token &operator=(const Token &other);

  const QString &id() const;
  bool isNull() const;

  inline const QString &value() const { return _value; }

protected:
  QString _id;
  QString _value;
};


class Lexer
{
public:
  Lexer();

  void addPattern(const QRegularExpression &regex, const QString &id);
  void addPattern(const QString &regex, const QString &id);

  void start(const QString &text);
  Token next();

protected:
  int _offset;
  QString _text;
  QList< QPair<QRegularExpression, QString> > _pattern;
};


class Parser
{
public:
  typedef enum {
    S_START, S_RESPONSE, S_CLOSING,
    S_CQ, S_QRZ, S_CQ_CALL, S_CALL, S_NAME, S_RST, S_QTH, S_RIG, S_ANT, S_PWR, S_WX
  } State;

public:
  Parser(QHash<QString, QString> &ctx, State init=S_START);

  void parse(const QString &text);
  inline State state() const { return _state; }

protected:
  QHash<QString, QString> &_context;
  State _state;
};

class ChatMachine;
class ChatMachineState;

class ChatMachineCode: public QObject
{
  Q_OBJECT

protected:
  explicit ChatMachineCode(QObject *parent);

public:
  virtual bool exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream) = 0;

  void addCommand(ChatMachineCode *cmd);

protected:
  QVector<ChatMachineCode *> _commands;
};

class ChatMachineClear: public ChatMachineCode
{
  Q_OBJECT

public:
  explicit ChatMachineClear(ChatMachineCode *parent);

  bool exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream);
};

class ChatMachineSend: public ChatMachineCode
{
  Q_OBJECT

public:
  ChatMachineSend(bool send, const QString &filename, ChatMachineCode *parent);

  bool exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream);

protected:
  bool _send;
  QString _filename;
};

class ChatMachineOn: public ChatMachineCode
{
  Q_OBJECT

public:
  ChatMachineOn(const QString &token, ChatMachineCode *parent);

  bool exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream);

protected:
  QString _token;
};

class ChatMachineGoto: public ChatMachineCode
{
  Q_OBJECT

public:
  ChatMachineGoto(const QString &state, ChatMachineCode *parent);

  bool exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream);

protected:
  QString _state;
};

class ChatMachineStoreToken: public ChatMachineCode
{
  Q_OBJECT

public:
  ChatMachineStoreToken(const QString &var, ChatMachineCode *parent);

  bool exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream);

protected:
  QString _var;
};


class ChatMachineState: public ChatMachineCode
{
  Q_OBJECT

public:
  explicit ChatMachineState(ChatMachine *parent);

  bool exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream);
};


class ChatMachine: public QObject
{
  Q_OBJECT

public:
  explicit ChatMachine(QObject *parent=nullptr);

  bool load(const QString &file);
  bool process(const QString &text, QTextStream &response);

  QHash<QString, QString> &context();
  const QHash<QString, QString> &context() const;
  void clear();

  ChatMachineState *state();
  void setState(const QString &state);
  void setState(ChatMachineState *state);

protected:
  bool preParse(QXmlStreamReader &reader);
  bool parse(QXmlStreamReader &reader);
  void parseToken(QXmlStreamReader &reader);
  void parseCode(ChatMachineCode *code, QXmlStreamReader &reader);

protected:
  Lexer _lexer;
  ChatMachineState *_start;
  ChatMachineState *_state;
  QMap<QString, ChatMachineState *> _states;
  QHash<QString, QString> _context;
};


class QSOChat: public QObject
{
	Q_OBJECT

public:
  typedef enum {
    S_INITIAL, S_CQ, S_BASIC_XCHANGE, S_RAG_CHEW
  } State;

public:
  explicit QSOChat(QObject *parent=nullptr);

	inline QHash<QString, QString> &context() { return _context; }
	inline const QHash<QString, QString> &context() const { return _context; }

	bool handle(const QString &message, QTextStream &reply);

  void reset();

public:
	static bool isComplete(const QString &message);

protected:
  State _state;
	QHash<QString, QString> _context;
};

#endif // QSOPARSER_HH
