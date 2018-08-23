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
  _pattern << QPair<QRegExp, Token::Type>(QRegExp("\\b([0-9a-z]?[a-z]+[0-9]+[a-z/1-9]+)\\b", Qt::CaseInsensitive), Token::T_CALL)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(de)\\b", Qt::CaseInsensitive), Token::T_DE)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(cq)\\b", Qt::CaseInsensitive), Token::T_CQ)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(rst)\\b", Qt::CaseInsensitive), Token::T_RST)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(name)\\b", Qt::CaseInsensitive), Token::T_NAME)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(op)\\b", Qt::CaseInsensitive), Token::T_NAME)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(qth)\\b", Qt::CaseInsensitive), Token::T_QTH)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(qrz)\\b", Qt::CaseInsensitive), Token::T_QRZ)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(nr)\\b", Qt::CaseInsensitive), Token::T_NEAR)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(rig)\\b", Qt::CaseInsensitive), Token::T_RIG)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(ant)\\b", Qt::CaseInsensitive), Token::T_ANT)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(pwr)\\b", Qt::CaseInsensitive), Token::T_PWR)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(wx)\\b", Qt::CaseInsensitive), Token::T_WX)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(is)\\b", Qt::CaseInsensitive), Token::T_IS)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(hr)\\b", Qt::CaseInsensitive), Token::T_HERE)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b([k=+])\\b", Qt::CaseInsensitive), Token::T_BREAK)
      << QPair<QRegExp, Token::Type>(QRegExp("\\b(73)\\b", Qt::CaseInsensitive), Token::T_73)
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
    //qDebug() << "Got token @" << match << token.value() << token.type() << "l=" << length;
    _offset = match + length;
  } else {
    _offset = _text.length();
  }

  return token;
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
  Lexer lexer(text);

  QString firstcall;

  for (Token tok = lexer.next(); Token::T_EOS != tok.type(); tok = lexer.next())
  {
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
          case Token::T_73:
            _state = S_CLOSING;
            break;
          case Token::T_QRZ:
            _state = S_QRZ;
            break;
          default:
            break;
        }
        break;

      case S_RESPONSE:
        switch (tok.type()) {
          case Token::T_CQ:
            _state = S_CQ;
            break;
          case Token::T_CALL:
            break;
          case Token::T_NAME:
            _state = S_NAME;
            break;
          case Token::T_RST:
            _state = S_RST;
            break;
          case Token::T_QTH:
            _state = S_QTH;
            break;
          case Token::T_RIG:
            _state = S_RIG;
            break;
          case Token::T_ANT:
            _state = S_ANT;
            break;
          case Token::T_PWR:
            _state = S_PWR;
            break;
          case Token::T_WX:
            _state = S_WX;
            break;
          case Token::T_73:
            _state = S_CLOSING;
            break;
          default:
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
            _context["call"] = firstcall;
            break;
          default:
            _state = S_RESPONSE;
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
          case Token::T_NEAR:
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

      case S_RIG:
        switch (tok.type()) {
          case Token::T_IS:
          case Token::T_HERE:
            break;
          case Token::T_WORD:
            _context["dxrig"] = tok.value();
            _state = S_RESPONSE;
            break;
          default:
            _state = S_RESPONSE;
            break;
        }
        break;

      case S_ANT:
        switch (tok.type()) {
          case Token::T_IS:
          case Token::T_HERE:
            break;
          case Token::T_WORD:
            _context["dxant"] = tok.value();
            _state = S_RESPONSE;
            break;
          default:
            _state = S_RESPONSE;
            break;
        }
        break;

      case S_PWR:
        switch (tok.type()) {
          case Token::T_IS:
          case Token::T_HERE:
            break;
          case Token::T_WORD:
          case Token::T_NUMBER:
            _context["dxpwr"] = tok.value();
            _state = S_RESPONSE;
            break;
          default:
            _state = S_RESPONSE;
            break;
        }
        break;

      case S_WX:
        switch (tok.type()) {
          case Token::T_IS:
          case Token::T_HERE:
            break;
          case Token::T_WORD:
          case Token::T_NUMBER:
            _context["dxwx"] = tok.value();
            break;
          default:
            _state = S_RESPONSE;
            break;
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
