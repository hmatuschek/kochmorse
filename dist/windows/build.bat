SET PATH=C:/MinGW/bin;C:/Qt/Qt5.3.2/5.3/mingw482_32/bin;%PATH%

cmake ../.. -G "MinGW Makefiles" -DKOCHMORSE_LIBS="-lportaudio" -DCMAKE_CXX_FLAGS="-IC:/Qt/Qt5.3.2/Tools/mingw482_32/i686-w64-mingw32/include"
mingw32-make
