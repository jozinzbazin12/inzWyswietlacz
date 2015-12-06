Rêczna kompilacja:
{1}- Pliki nag³ówkowe kompilatora

g++ -std=c++0x "-I{1}" "-Ilib/include" -O3 -Wall -c -fmessage-length=0 -o "src/main.o" "src/main.cpp" 
g++ "-Llib/lib" -static-libgcc -static-libstdc++ -o Wyswietlacz.exe "src/main.o" -lglew32s -lfreeglut -lopengl32 -lglu32 -lwinmm -lmingw32 -lSDL2main -lSDL2 -lSDL2_image 
