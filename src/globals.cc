#include "globals.hh"
#include <QString>

const double Globals::sampleRate = 16e3;

/* Internal used function to initialize a static hash table. */
inline QHash<QString, QChar> _initCodeTable() {
  QHash<QString, QChar> table;
  table[".-"] = 'a';     table["-..."] = 'b';   table["-.-."] = 'c';   table["-.."] = 'd';
  table["."] = 'e';      table["..-."] = 'f';   table["--."] = 'g';    table["...."] = 'h';
  table[".."] = 'i';     table[".---"] = 'j';   table["-.-"] = 'k';    table[".-.."] = 'l';
  table["--"] = 'm';     table["-."] = 'n';     table["---"] = 'o';    table[".--."] = 'p';
  table["--.-"] = 'q';   table[".-."] = 'r';    table["..."] = 's';    table["-"] = 't';
  table["..-"] = 'u';    table["...-"] = 'v';   table[".--"] = 'w';    table["-..-"] = 'x';
  table["-.--"] = 'y';   table["--.."] = 'z';   table["-----"] = '0';  table[".----"] = '1';
  table["..---"] = '2';  table["...--"] = '3';  table["....-"] = '4';  table["....."] = '5';
  table["-...."] = '6';  table["--..."] = '7';  table["---.."] = '8';  table["----."] = '9';
  table[".-.-.-"] = '.'; table["--..--"] = ','; table["..--.."] = '?'; table["-..-."] = '/';
  table[".-..."] = '&';  table["---..."] = ':'; table["-.-.-."] = ';'; table["-...-"] = '=';
  table[".-.-."] = '+';  table["-.--."] = '(';  table["-.--.-"] = ')'; table["-....-"] = '-';
  table[".--.-."] = '@';
  // Some prosigns:
  table["-...-.-"] = QChar(0x2417);  // BK -> ETB
  table["-.-..-.."] = QChar(0x2404); // CL -> EOT
  table["...-.-"] = QChar(0x2403);   // SK -> ETX
  table["...-."] = QChar(0x2406);    // SN -> ACK
  table["-.--."] = QChar(0x2407);    // KN -> BEL
  // Other prosigns are repr. by their character representative:
  // AR -> '+'; AS -> '&'; BT -> '=';
  return table;
}

/* Init static morse-code table. */
QHash<QString, QChar> Globals::codeTable = _initCodeTable();


/* Internal used function to initialize a static hash table. */
inline QHash<QChar, QString> _initCharTable() {
  QHash<QChar, QString> table;
  table['a'] = ".-";     table['b'] = "-...";   table['c'] = "-.-.";   table['d'] = "-..";
  table['e'] = ".";      table['f'] = "..-.";   table['g'] = "--.";    table['h'] = "....";
  table['i'] = "..";     table['j'] = ".---";   table['k'] = "-.-";    table['l'] = ".-..";
  table['m'] = "--";     table['n'] = "-.";     table['o'] = "---";    table['p'] = ".--.";
  table['q'] = "--.-";   table['r'] = ".-.";    table['s'] = "...";    table['t'] = "-";
  table['u'] = "..-";    table['v'] = "...-";   table['w'] = ".--";    table['x'] = "-..-";
  table['y'] = "-.--";   table['z'] = "--..";   table['0'] = "-----";  table['1'] = ".----";
  table['2'] = "..---";  table['3'] = "...--";  table['4'] = "....-";  table['5'] = ".....";
  table['6'] = "-....";  table['7'] = "--...";  table['8'] = "---..";  table['9'] = "----.";
  table['.'] = ".-.-.-"; table[','] = "--..--"; table['?'] = "..--.."; table['/'] = "-..-.";
  table['&'] = ".-...";  table[':'] = "---..."; table[';'] = "-.-.-."; table['='] = "-...-";
  table['+'] = ".-.-.";  table['('] = "-.--.";  table[')'] = "-.--.-"; table['-'] = "-....-";
  table['@'] = ".--.-.";
  // Some prosigns:
  table[QChar(0x2417)] = "-...-.-";  // BK -> ETB
  table[QChar(0x2404)] = "-.-..-.."; // CL -> EOT
  table[QChar(0x2403)] = "...-.-";   // SK -> ETX
  table[QChar(0x2406)] = "...-.";    // SN -> ACK
  table[QChar(0x2407)] = "-.--.";    // KN -> BEL
  // Other prosigns are repr. by their character representative:
  // AR -> '+'; AS -> '&'; BT -> '=';
  return table;
}

/* Init static morse-code table. */
QHash<QChar, QString> Globals::charTable = _initCharTable();


inline QHash<QChar, QString> _initProsignTable() {
  QHash<QChar, QString> table;
  table[QChar(0x2417)] = "BK"; // BK -> ETB
  table[QChar(0x2404)] = "CL"; // CL -> EOT
  table[QChar(0x2403)] = "SK"; // SK -> ETX
  table[QChar(0x2406)] = "SN"; // SN -> ACK
  table[QChar(0x2407)] = "KN"; // KN -> BEL
  return table;
}

/* Init static morse-code table. */
QHash<QChar, QString> Globals::prosignTable = _initProsignTable();


QString
Globals::mapProsign(QChar ch) {
  if (! prosignTable.contains(ch))
    return ch;
  return prosignTable[ch];
}
