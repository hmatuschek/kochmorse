#include "qsochat.hh"
#include <QRegExp>
#include "textgen.hh"
#include <QDebug>


/* ********************************************************************************************* *
 * Implementation of Token
 * ********************************************************************************************* */
Token::Token(Type type)
  : _type(type), _value()
{
  // pass...
}

Token::Token(Type type, const QString &value)
  : _type(type), _value(value)
{
  // pass...
}

/* ********************************************************************************************* *
 * Implementation of Lexer
 * ********************************************************************************************* */
Lexer::Lexer(const QString &text)
  : _offset(0), _text(text), _pattern()
{
  _pattern << QPair<QRegExp, Token::Type>(QRegExp("\\b([0-9a-z][a-z][0-9]+[a-z]+)\\b", Qt::CaseInsensitive), Token::T_CALL)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(de)\\b", Qt::CaseInsensitive), Token::T_DE)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(cq)\\b", Qt::CaseInsensitive), Token::T_CQ)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(rst)\\b", Qt::CaseInsensitive), Token::T_RST)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(name)\\b", Qt::CaseInsensitive), Token::T_NAME)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(op)\\b", Qt::CaseInsensitive), Token::T_NAME)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(qth)\\b", Qt::CaseInsensitive), Token::T_QTH)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(is)\\b", Qt::CaseInsensitive), Token::T_IS)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(hr)\\b", Qt::CaseInsensitive), Token::T_HERE)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b([k=])\\b", Qt::CaseInsensitive), Token::T_BREAK)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b([a-z]+)\\b", Qt::CaseInsensitive), Token::T_WORD)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b([0-9]+)\\b", Qt::CaseInsensitive), Token::T_NUMBER);
}

Token
Lexer::next() {
  if (_offset == _text.size())
    return Token(Token::T_EOS);

  int match = -1;
  int length = 0;
  Token token(Token::T_EOS);

  for (int i=0; i<_pattern.size(); i++) {
    int idx = _pattern[i].first.indexIn(_text, _offset);
    if (idx < 0)
      continue;
    if ((match<0) || (match>idx)) {
      match = idx;
      token = Token(_pattern[i].second, _pattern[i].first.cap(1));
      length = _pattern[i].first.matchedLength();
    }
  }


  if (match>=0) {
    qDebug() << "Got token @" << match << token.value() << token.type() << "l=" << length;
    _offset = match + length;
  } else
    _offset = _text.length();

  return token;
}


/* ********************************************************************************************* *
 * Implementation of Parser
 * ********************************************************************************************* */
Parser::Parser(QHash<QString, QString> &ctx)
  : _context(ctx), _state(S_START)
{
  // pass...
}

void Parser::parse(const QString &text) {
  Lexer lexer(text);

  Token tok = lexer.next();
  QString firstcall;

  while (true) {
    // Dispatch by state
    switch (_state) {
      // In Start condition
      case S_START:
        switch (tok.type()) {
          case Token::T_CQ:
            _state = S_CQ;
            break;
          case Token::T_CALL:
            _state = S_CALL;
            firstcall = tok.value();
            break;
          default:
            _state = S_START;
            break;
        }
        break;

      case S_RESPONSE:
        switch (tok.type()) {
          case Token::T_NAME:
            _state = S_NAME;
            break;
          case Token::T_RST:
            _state = S_RST;
            break;
          case Token::T_QTH:
            _state = S_QTH;
            break;
          default:
            _state = S_RESPONSE;
            break;
        }
        break;

      // Once a CQ has been seen
      case S_CQ:
        switch (tok.type()) {
          case Token::T_CALL:
            // CQ call is complete
            _state = S_CQ_CALL;
            _context["dxcall"] = tok.value();
            break;
          default:
            _state = S_CQ;
            break;
        }
        break;

      // Once a call-sign has been seen
      case S_CALL:
        switch (tok.type()) {
          case Token::T_DE:
            _state = S_CALL;
            break;
          case Token::T_CALL:
            _state = S_RESPONSE;
            _context["call"] = firstcall;
            _context["dxcall"] = tok.value();
            break;
          case Token::T_EOS:
          case Token::T_BREAK:
            _state = S_RESPONSE;
            _context["dxcall"] = firstcall;
            break;
          default:
            break;
        }
        break;

      case S_NAME:
        switch (tok.type()) {
          case Token::T_IS:
          case Token::T_HERE:
            break;
          case Token::T_WORD:
            _state = S_RESPONSE;
            _context["dxname"] = tok.value();
            break;
          default:
            _state = S_RESPONSE;
            break;
        }
        break;

      case S_RST:
        switch (tok.type()) {
          case Token::T_IS:
            break;
          case Token::T_NUMBER:
            _context["myrst"] = tok.value();
            _state = S_RESPONSE;
            break;
          default:
            _state = S_RESPONSE;
            break;
        }
        break;

      case S_QTH:
        switch (tok.type()) {
          case Token::T_IS:
          case Token::T_HERE:
            break;
          case Token::T_WORD:
            _context["dxqth"] = tok.value();
            _state = S_RESPONSE;
            break;
          default:
            _state = S_RESPONSE;
            break;
        }
        break;

      default:
        break;
    }

    // Check if EOS is reached
    if (Token::T_EOS == tok.type())
      break;
    else
      tok = lexer.next();
  }
}


/* ********************************************************************************************* *
 * Implementation of QSOChat
 * ********************************************************************************************* */
QSOChat::QSOChat(QObject *parent)
  : QObject(parent), _inQSO(false), _context()
{
	reset();
}


bool
QSOChat::handle(const QString &message, QTextStream &reply) {
	if (! isComplete(message))
		return false;

  QHash<QString, QString> result;
  Parser parser(result);
  parser.parse(message);

  if ((Parser::S_CQ_CALL == parser.state()) && result.contains("dxcall")) {
    TextGen generator(":/qso/chat-cq-response.xml");
    _context["dxcall"] = result["dxcall"];
    generator.generate(reply, _context);
  } else if (Parser::S_RESPONSE == parser.state()) {
    if (result.contains("dxname"))
      _context["dxname"] = result["dxname"];
    if (result.contains("dxqth"))
      _context["dxqth"] = result["dxqth"];
    TextGen generator(":/qso/chat-response.xml");
    generator.generate(reply, _context);
  }

  return true;
}


void
QSOChat::reset() {
  _inQSO = false;
  _context.clear();

  // Regenerate call, name, etc...
  TextGen generator(":/qso/chat-init.xml");
  QTextStream buffer;
  generator.generate(buffer, _context);
}


bool
QSOChat::isComplete(const QString &message) {
	return message.simplified().endsWith(" k",Qt::CaseInsensitive) ||
      message.simplified().endsWith(" +", Qt::CaseInsensitive);
}
