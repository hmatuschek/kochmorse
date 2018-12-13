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

  // Special letters (language dependent)
  table[".-.-"] = QChar(0x00e4); // ä
  table["---."] = QChar(0x00f6); // ö
  table["..--"] = QChar(0x00fc); // ü
  table["----"] = QChar(0x03c7); // χ,CH
  table[".--.-"] = QChar(0x00e5); // å
  table["..-.."] = QChar(0x00e9); // é
  table["..--."] = QChar(0x00f0); // ð
  table[".--.."] = QChar(0x00de); // þ
  table["-.-.."] = QChar(0x0107); // ć
  table["--.-."] = QChar(0x011d); // ĝ
  table[".---."] = QChar(0x0125); // ĵ
  table["...-."] = QChar(0x015d); // ŝ
  table[".-..-"] = QChar(0x0142); // ł
  table["--.--"] = QChar(0x00f1); // ñ
  table["--..-"] = QChar(0x0107); // ż
  table["..-.-"] = QChar(0x00bf); // ¿
  table["--...-"] = QChar(0x00a1); // ¡
  table["...--.."] = QChar(0x00df); // ß
  table["...-..."] = QChar(0x0144); // ś

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

  // Special letters (language dependent)
  table[QChar(0x017a)] = "--"; // ź
  table[QChar(0x00e4)] = ".-.-"; // ä
  table[QChar(0x0105)] = ".-.-"; // ą
  table[QChar(0x00f6)] = "---."; // ö
  table[QChar(0x00f8)] = "---."; // ø
  table[QChar(0x00f3)] = "---."; // ó
  table[QChar(0x00fc)] = "..--"; // ü
  table[QChar(0x016d)] = "..--"; // ŭ
  table[QChar(0x03c7)] = "----"; // χ,CH
  table[QChar(0x0125)] = "----"; // ĥ
  table[QChar(0x00e0)] = ".--.-"; // à
  table[QChar(0x00e5)] = ".--.-"; // å
  table[QChar(0x00e8)] = "..-.."; // è
  table[QChar(0x00e9)] = "..-.."; // é
  table[QChar(0x0109)] = "..-.."; // ę
  table[QChar(0x00f0)] = "..--."; // ð
  table[QChar(0x00de)] = ".--.."; // þ
  table[QChar(0x0109)] = "-.-.."; // ĉ
  table[QChar(0x0107)] = "-.-.."; // ć
  table[QChar(0x011d)] = "--.-."; // ĝ
  table[QChar(0x0125)] = ".---."; // ĵ
  table[QChar(0x015d)] = "...-."; // ŝ
  table[QChar(0x0142)] = ".-..-"; // ł
  table[QChar(0x0144)] = "--.--"; // ń
  table[QChar(0x00f1)] = "--.--"; // ñ
  table[QChar(0x0107)] = "--..-"; // ż
  table[QChar(0x00bf)] = "..-.-"; // ¿
  table[QChar(0x00a1)] = "--...-"; // ¡
  table[QChar(0x00df)] = "...--.."; // ß
  table[QChar(0x0144)] = "...-..."; // ś

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
