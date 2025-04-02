#include "qsochat.hh"
#include <QRegularExpression>
#include "textgen.hh"
#include <QDebug>


/* ********************************************************************************************* *
 * Implementation of Token
 * ********************************************************************************************* */
Token::Token()
  : _id(), _value()
{
  // pass...
}

Token::Token(const QString &id, const QString &value)
  : _id(id), _value(value)
{
  // pass...
}

const QString &
Token::id() const {
  return _id;
}

bool
Token::isNull() const {
  return _id.isEmpty();
}

Token::Token(const Token &other)
  : _id(other._id), _value(other._value)
{
  // pass...
}

Token &
Token::operator=(const Token &other) {
  _id = other._id;
  _value = other._value;
  return *this;
}


/* ********************************************************************************************* *
 * Implementation of Lexer
 * ********************************************************************************************* */
Lexer::Lexer()
  : _offset(0), _text(), _pattern()
{
  addPattern("\\b([0-9a-z]?[a-z]+[0-9]+[a-z/1-9]+)\\b", "call");
  addPattern("\\b(de)\\b", "de");
  addPattern("\\b(cq)\\b", "cq");
  addPattern("\\b(rst)\\b", "rst");
  addPattern("\\b(name)\\b", "name");
  addPattern("\\b(op)\\b", "name");
  addPattern("\\b(qth)\\b", "qth");
  addPattern("\\b(qrz)\\b", "qrz");
  addPattern("\\b(nr)\\b", "nr");
  addPattern("\\b(rig)\\b", "rig");
  addPattern("\\b(ant)\\b", "ant");
  addPattern("\\b(pwr)\\b", "pwr");
  addPattern("\\b(power)\\b", "power");
  addPattern("\\b(wx)\\b", "wx");
  addPattern("\\b(is)\\b", "is");
  addPattern("\\b(hr)\\b", "hr");
  addPattern("\\b(k=+)\\b", "break");
  addPattern("\\b(73)\\b", "73");
  addPattern("\\b([a-z]+)\\b", "word");
  addPattern("\\b([0-9]+)\\b", "number");
}

void
Lexer::addPattern(const QRegularExpression &regex, const QString &id) {
  _pattern.append(QPair<QRegularExpression, QString>(regex, id));
}

void
Lexer::addPattern(const QString &regex, const QString &id) {
  _pattern.append(QPair<QRegularExpression, QString>(QRegularExpression(regex, QRegularExpression::CaseInsensitiveOption), id));
}

void
Lexer::start(const QString &text) {
  _text = text;
  _offset = 0;
}

Token
Lexer::next() {
  if (_offset == _text.size())
    return Token();

  int match = -1;
  int length = 0;
  Token token;

  for (int i=0; i<_pattern.size(); i++) {
    QRegularExpressionMatch matchResult = _pattern[i].first.match(_text, _offset);
    if (!matchResult.hasMatch()) {
      // No match for this pattern, continue to next
      continue;
    }

    int idx = matchResult.capturedStart(1); // Start of the captured group
    if ((match<0) || (match>idx)) {
      match = idx;
      token = Token(_pattern[i].second, matchResult.captured(1));
      length = matchResult.capturedLength(1); // Length of the matched string
    }
  }


  if (match>=0) {
    //qDebug() << "Got token @" << match << token.value() << token.type() << "l=" << length;
    _offset = match + length;
  } else {
    _offset = _text.length();
  }

  return token;
}

/* ********************************************************************************************* *
 * Implementation of ChatMachine
 * ********************************************************************************************* */
ChatMachine::ChatMachine(QObject *parent)
  : QObject(parent), _lexer(), _start(nullptr), _state(nullptr), _states(), _context()
{
  // pass...
}

bool
ChatMachine::preParse(QXmlStreamReader &reader)
{
  _lexer = Lexer();
  _state = _start = nullptr;
  foreach (auto state, _states.values())
    state->deleteLater();
  _states.clear();

  /* First pass: Define states und token. */
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isStartElement() && ("chat" == reader.name())) {
      // Handle document element
      while (! reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
          if ("token" == reader.name()) {
            // pass
          } else if ("start" == reader.name()) {
            if (! reader.attributes().hasAttribute("id")) {
              reader.raiseError(tr("<start> element at line %1 needs 'id' attribute.")
                                .arg(reader.lineNumber()));
              break;
            }
            if (_start) {
              reader.raiseError(tr("<start> element at line %1: Multiple <start> elements in one document!")
                                .arg(reader.lineNumber()));
              break;
            }
            QString id = reader.attributes().value("id").toString();
            if (_states.contains(id)) {
              reader.raiseError(tr("<start> element at line %1: Redefinition of state '%2'!")
                                .arg(reader.lineNumber()).arg(id));
              break;
            }
            _state = _start = new ChatMachineState(this);
            _states.insert(id, _start);
          } else if ("state" == reader.name()) {
            if (! reader.attributes().hasAttribute("id")) {
              reader.raiseError(tr("<state> element at line %1 needs 'id' attribute.")
                                .arg(reader.lineNumber()));
              break;
            }
            QString id = reader.attributes().value("id").toString();
            if (_states.contains(id)) {
              reader.raiseError(tr("<state> element at line %1: Redefinition of state '%2'!")
                                .arg(reader.lineNumber()).arg(id));
              break;
            }
            ChatMachineState *state = new ChatMachineState(this);
            _states.insert(id, state);
          } else {
            // Handle error
            reader.raiseError(tr("Unexpected element at line %1: %2")
                              .arg(reader.lineNumber()).arg(reader.name().toString()));
            return false;
          }
        }
      }
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


bool
ChatMachine::parse(QXmlStreamReader &reader)
{
  /* Second pass: read and link token and states. */
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isStartElement() && ("chat" == reader.name())) {
      // Handle document element
      while (! reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
          if ("token" == reader.name()) {
            this->parseToken(reader);
          } else if (("start" == reader.name()) || ("state" == reader.name())) {
            if (! reader.attributes().hasAttribute("id")) {
              reader.raiseError(tr("<state> or <start> element at line %1 needs 'id' attribute.")
                                .arg(reader.lineNumber()));
              break;
            }
            QString id=reader.attributes().value("id").toString();
            ChatMachineState *state = _states[id];
            this->parseCode(state, reader);
          }
        }
      }
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
ChatMachine::parseToken(QXmlStreamReader &reader) {
  if (! reader.attributes().hasAttribute("id")) {
    reader.raiseError(tr("<token> element at line %1 needs 'id' attribute.")
                      .arg(reader.lineNumber()));
    return;
  }
  QString id=reader.attributes().value("id").toString();

  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isCDATA() || reader.isCharacters()) {
      qDebug() << "Found token " << id << reader.text().toString();
      _lexer.addPattern(QRegularExpression(reader.text().toString(), QRegularExpression::CaseInsensitiveOption), id);
    } else if (reader.isStartElement()) {
      reader.raiseError(tr("Unexpected element at line %1: %2")
                        .arg(reader.lineNumber()).arg(reader.name().toString()));
      break;
    } else if (reader.isEndElement()) {
      break;
    }
  }
}


void
ChatMachine::parseCode(ChatMachineCode *code, QXmlStreamReader &reader) {
  while (! reader.atEnd()) {
    reader.readNext();
    if (reader.isStartElement()) {
      if ("clear" == reader.name()) {
        code->addCommand(new ChatMachineClear(code));
      } else if (("load" == reader.name()) || ("send" == reader.name())) {
        if (!reader.attributes().hasAttribute("file")) {
          reader.raiseError(tr("<load> or <send> element line %1: 'file' attribute needed.")
                            .arg(reader.lineNumber()));
          break;
        }
        code->addCommand(new ChatMachineSend(
                           "send" == reader.name(), reader.attributes().value("file").toString(), code));
      } else if ("on" == reader.name()) {
        if (! reader.attributes().hasAttribute("token")) {
          reader.raiseError(tr("<on> element line %1: 'token' attribute needed.")
                            .arg(reader.lineNumber()));
          break;
        }
        code->addCommand(new ChatMachineOn(reader.attributes().value("token").toString(), code));
      } else if ("goto" == reader.name()) {
        if (! reader.attributes().hasAttribute("state")) {
          reader.raiseError(tr("<goto> element line %1: 'state' attribute needed.")
                            .arg(reader.lineNumber()));
          break;
        }
        code->addCommand(new ChatMachineGoto(reader.attributes().value("state").toString(), code));
      } else if ("store-token" == reader.name()) {
        if (! reader.attributes().hasAttribute("store-token")) {
          reader.raiseError(tr("<store-token> element line %1: 'var' attribute needed.")
                            .arg(reader.lineNumber()));
          break;
        }
        code->addCommand(new ChatMachineStoreToken(reader.attributes().value("var").toString(), code));
      } else {
        reader.raiseError(tr("Unexpected element at line %1: %2")
                          .arg(reader.lineNumber()).arg(reader.name().toString()));
        break;
      }
    }
  }
}

void
ChatMachine::clear() {
  _context.clear();
}

QHash<QString, QString> &
ChatMachine::context() {
  return _context;
}

const QHash<QString, QString> &
ChatMachine::context() const {
  return _context;
}

ChatMachineState *
ChatMachine::state() {
  return _state;
}

void
ChatMachine::setState(const QString &state) {
  if (! _states.contains(state)) {
    qDebug() << "ChatMachine: Cannot set state to" << state << ": Unknown state.";
    return;
  }
  setState(_states[state]);
}

void
ChatMachine::setState(ChatMachineState *state) {
  _state = state;
}


/* ********************************************************************************************* *
 * Implementation of ChatMachineCode
 * ********************************************************************************************* */
ChatMachineCode::ChatMachineCode(QObject *parent)
  : QObject(parent), _commands()
{
  // pass...
}

void
ChatMachineCode::addCommand(ChatMachineCode *cmd) {
  _commands.append(cmd);
}


/* ********************************************************************************************* *
 * Implementation of ChatMachineClear
 * ********************************************************************************************* */
ChatMachineClear::ChatMachineClear(ChatMachineCode *parent)
  : ChatMachineCode(parent)
{
  // pass...
}

bool
ChatMachineClear::exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream) {
  Q_UNUSED(token); Q_UNUSED(value); Q_UNUSED(stream);
  machine->clear();
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ChatMachineSend
 * ********************************************************************************************* */
ChatMachineSend::ChatMachineSend(bool send, const QString &filename, ChatMachineCode *parent)
  : ChatMachineCode(parent), _send(send), _filename(filename)
{
  // pass...
}

bool
ChatMachineSend::exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream) {
  Q_UNUSED(token); Q_UNUSED(value);

  TextGen generator(_filename);
  generator.generate(stream, machine->context());

  return true;
}

/* ********************************************************************************************* *
 * Implementation of ChatMachineOn
 * ********************************************************************************************* */
ChatMachineOn::ChatMachineOn(const QString &token, ChatMachineCode *parent)
  : ChatMachineCode(parent), _token(token)
{
  // pass...
}

bool
ChatMachineOn::exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream)
{
  if (_token != token)
    return true;

  for(auto instr=_commands.begin(); instr!=_commands.end(); instr++) {
    if (! (*instr)->exec(token, value, machine, stream))
      return false;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ChatMachineGoto
 * ********************************************************************************************* */
ChatMachineGoto::ChatMachineGoto(const QString &state, ChatMachineCode *parent)
  : ChatMachineCode(parent), _state(state)
{
  // pass...
}

bool
ChatMachineGoto::exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream) {
  Q_UNUSED(token); Q_UNUSED(value); Q_UNUSED(stream);
  machine->setState(_state);
  return false;
}

/* ********************************************************************************************* *
 * Implementation of ChatMachineStoreToken
 * ********************************************************************************************* */
ChatMachineStoreToken::ChatMachineStoreToken(const QString &var, ChatMachineCode *parent)
  : ChatMachineCode(parent), _var(var)
{
  // pass...
}

bool
ChatMachineStoreToken::exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream)
{
  Q_UNUSED(token); Q_UNUSED(stream);
  machine->context().insert(_var, value);
  return true;
}

/* ********************************************************************************************* *
 * Implementation of ChatMachineState
 * ********************************************************************************************* */
ChatMachineState::ChatMachineState(ChatMachine *parent)
  : ChatMachineCode(parent)
{
  // pass...
}

bool
ChatMachineState::exec(const QString &token, const QString &value, ChatMachine *machine, QTextStream &stream)
{
  for(auto instr=_commands.begin(); instr!=_commands.end(); instr++) {
    if (! (*instr)->exec(token, value, machine, stream))
      return false;
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of Parser
 * ********************************************************************************************* */
Parser::Parser(QHash<QString, QString> &ctx, State init)
  : _context(ctx), _state(init)
{
  // pass...
}

void Parser::parse(const QString &text) {
  Lexer lexer;
  lexer.start(text);

  QString firstcall;

  for (Token tok = lexer.next(); !tok.isNull(); tok = lexer.next())
  {
    // Dispatch by state
    switch (_state) {
      // In Start condition
      case S_START:
        if ("cq" == tok.id()) {
            _state = S_CQ;
        } else if ("call" == tok.id()) {
          _state = S_CALL;
          firstcall = tok.value();
        } else if ("73" == tok.id()) {
          _state = S_CLOSING;
        } else if ("qrz" == tok.id()) {
          _state = S_QRZ;
        }
        break;

      case S_RESPONSE:
        if ("cq" == tok.id()) {
          _state = S_CQ;
        } else if ("name" == tok.id()) {
          _state = S_NAME;
        } else if ("rst" == tok.id()) {
          _state = S_RST;
        } else if ("qth" == tok.id()) {
          _state = S_QTH;
        } else if ("rig" == tok.id()) {
          _state = S_RIG;
        } else if ("ant" == tok.id()) {
          _state = S_ANT;
        } else if ("pwr" == tok.id()) {
          _state = S_PWR;
        } else if ("wx" == tok.id()) {
          _state = S_WX;
        } else if ("73" == tok.id()) {
          _state = S_CLOSING;
        }
        break;

      // Once a CQ has been seen
      case S_CQ:
        if ("call" == tok.id()) {
          // CQ call is complete
          _state = S_CQ_CALL;
          _context["dxcall"] = tok.value();
        }
        break;

      // Once a call-sign has been seen
      case S_CALL:
        if ("de" == tok.id()) {
          _context["call"] = firstcall;
        } else {
          _state = S_RESPONSE;
        }
        break;

      case S_NAME:
        if (("is" == tok.id()) || ("hr" == tok.id())) {
          // pass...
        } else if ("word" == tok.id()) {
          _state = S_RESPONSE;
          _context["dxname"] = tok.value();
        } else {
          _state = S_RESPONSE;
        }
        break;

      case S_RST:
        if ("is" == tok.id()) {
          // pass...
        } else if ("number" == tok.id()) {
          _context["myrst"] = tok.value();
          _state = S_RESPONSE;
        } else {
          _state = S_RESPONSE;
        }
        break;

      case S_QTH:
        if (("is"==tok.id()) || ("hr"==tok.id()) || ("nr"==tok.id())) {
          // pass...
        } else if ("word" == tok.id()) {
          _context["dxqth"] = tok.value();
          _state = S_RESPONSE;
        } else {
          _state = S_RESPONSE;
        }
        break;

      case S_RIG:
        if (("is"==tok.id()) || ("hr"==tok.id())) {
          // pass...
        } else if ("word" == tok.id()) {
          _context["dxrig"] = tok.value();
          _state = S_RESPONSE;
        } else {
          _state = S_RESPONSE;
        }
        break;

      case S_ANT:
        if (("is"==tok.id()) || ("hr"==tok.id())) {
          // pass...
        } else if ("word" == tok.id()) {
          _context["dxant"] = tok.value();
          _state = S_RESPONSE;
        } else {
          _state = S_RESPONSE;
        }
        break;

      case S_PWR:
        if (("is"==tok.id()) || ("hr"==tok.id())) {
          // pass...
        } else if (("word" == tok.id()) || ("number"==tok.id())) {
          _context["dxpwr"] = tok.value();
          _state = S_RESPONSE;
        } else {
          _state = S_RESPONSE;
        }
        break;

      case S_WX:
        if (("is"==tok.id()) || ("hr"==tok.id())) {
          // pass...
        } else if (("word" == tok.id()) || ("number"==tok.id())) {
          _context["dxwx"] = tok.value();
          _state = S_RESPONSE;
        } else {
          _state = S_RESPONSE;
        }
        break;

      case S_CLOSING:
      case S_QRZ:
        break;

      default:
        break;
    }
  }
}


/* ********************************************************************************************* *
 * Implementation of QSOChat
 * ********************************************************************************************* */
QSOChat::QSOChat(QObject *parent)
  : QObject(parent), _state(S_INITIAL), _context()
{
	// pass...
}


bool
QSOChat::handle(const QString &message, QTextStream &reply) {
	if (! isComplete(message))
		return false;

  QHash<QString, QString> result;
  Parser::State parserInit = Parser::S_START;
  if (S_BASIC_XCHANGE == _state)
    parserInit = Parser::S_RESPONSE;
  Parser parser(result, parserInit);
  parser.parse(message);

  if ((Parser::S_CQ_CALL == parser.state()) && result.contains("dxcall")) {
    reset();
    TextGen generator(":/qso/chat-cq-response.xml");
    _context["dxcall"] = result["dxcall"];
    generator.generate(reply, _context);
    _state = S_CQ;
  } else if ((S_CQ == _state) && (Parser::S_RESPONSE == parser.state())) {
    if (result.contains("call") && (result["call"] != _context["myfullcall"]))
      return true;
    if (result.contains("dxname"))
      _context["dxname"] = result["dxname"];
    if (result.contains("dxqth"))
      _context["dxqth"] = result["dxqth"];
    TextGen generator(":/qso/chat-response.xml");
    generator.generate(reply, _context);
    _state = S_BASIC_XCHANGE;
  } else if ((S_CQ == _state) && (Parser::S_QRZ == parser.state())) {
    TextGen generator(":/qso/chat-cq-response.xml");
    generator.generate(reply, _context);
  } else if ((S_BASIC_XCHANGE == _state) && (Parser::S_RESPONSE == parser.state())) {
    TextGen generator(":/qso/chat-ragchew.xml");
    generator.generate(reply, _context);
    _state = S_RAG_CHEW;
  } else if (((S_BASIC_XCHANGE == _state) || (S_RAG_CHEW == _state)) && (Parser::S_CLOSING == parser.state())) {
    TextGen generator(":/qso/chat-closing.xml");
    generator.generate(reply, _context);
  }

  return true;
}


void
QSOChat::reset() {
  _state = S_INITIAL;
  _context.clear();

  // Regenerate call, name, etc...
  TextGen generator(":/qso/chat-init.xml");
  QTextStream buffer;
  generator.generate(buffer, _context);
}


bool
QSOChat::isComplete(const QString &message) {
	return message.simplified().endsWith(" k", Qt::CaseInsensitive) ||
      message.simplified().endsWith(" +") || message.simplified().endsWith("?");
}
