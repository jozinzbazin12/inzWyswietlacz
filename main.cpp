#define GLEW_STATIC
#define coileklatek 10
#define ilekrokowwanimacji 30
#define ile 200
#define ilemtl 150
#define iletxt 300
#define ileobj 100
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <limits>
#include <process.h>
#include <windows.h>
#include <vector>
using namespace std;
////////////////////////////////////////////
void DrawString(double x, double y, double z, string string);
string otworz(string nazwa, string koniec);
string utnij(string dupa);
void zapisz();
long long unsigned sprawdz_rozmiar(string nazwa);
class animacja;
class obiekt;
class podobiekt;
class obiekt_final;
class material;
class material_lib;
class obcinanie;
class tekstura;
////////////////////////////////////////////
obcinanie *ciach;
obiekt_final * obiekty_f[1000];
obiekt_final ** obiekty_posortowane;
obiekt_final * obiekty_animowane[100];
obiekt_final * wybrany;
material_lib *materialy[ileobj];
tekstura *tekstury[iletxt];
obiekt *obiekty[ileobj];
tagPOINT *mysz_pozycja;
float modelview[16];
bool czywsp = true;
bool wcisnietyprawy = false;
bool wcisnietylewy = false;
GLfloat cx = 0;
GLfloat cy = 0;
GLfloat cx2 = 0, cy2 = 0, cz2 = 0;
int myk = 0;
int myk2 = 0;
int kamera = 5;
double predkosc = 10;
int licznik = 0;
int ramki = 0;
bool obracamy = false;
GLfloat px = -5, py = 5, pz = 11;
int ileobiektow = 0;
int ileobiektow2 = 0;
int ilematerialow = 0;
int iletekstur = 0;
int ilebuforow = 0;
int ileanimacji = 0;
int wys = 700, szer = 1300;
long long unsigned ilee = 0;
int ktoreswiatlo = 0;
int ktorapos = 0;
GLfloat kutas[] = { 0.4f, 0.4f, 0.4f, 0.4f };
int ktorykutas = -1;
int ktorykutas2 = 0;
GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 0.1f };
GLfloat light_diffuse[] = { 0.5f, 0.5f, 0.5f, 0.5f };
GLfloat light_specular[] = { 0.8f, 0.8f, 0.8f, 0.8f };
GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat high_shininess[] = { 100.0f };

HANDLE hThread;
HANDLE hThread2;
HANDLE hThread3;
struct informacja {
	string x1, y1, z1, fps, speed, amb, diff, spec, pos, poss, ob, ob2, ileob, ileob2, licznikob;
};
informacja info;

class Logger {
private:
	static ofstream outfile;
	static Logger* logger;
	ostringstream buffer;

	string constructTime(time_t time) {
		struct tm * now = localtime(&time);
		ostringstream ss;
		ss << '[';
		if (now->tm_mday >= 10) {
			ss << now->tm_mday;
		} else {
			ss << 0 << now->tm_mday;
		}
		ss << '-';
		if (now->tm_mon + 1 >= 10) {
			ss << now->tm_mon + 1;
		} else {
			ss << 0 << now->tm_mon + 1;
		}
		ss << '-' << (now->tm_year + 1900) << ' ';
		if (now->tm_hour >= 10) {
			ss << now->tm_hour;
		} else {
			ss << 0 << now->tm_hour;
		}
		ss << ':';
		if (now->tm_min >= 10) {
			ss << now->tm_min;
		} else {
			ss << 0 << now->tm_min;
		}
		ss << ':';
		if (now->tm_sec >= 10) {
			ss << now->tm_sec;
		} else {
			ss << 0 << now->tm_sec;
		}
		ss << "] - ";
		return ss.str();
	}

public:
	static string LINE;
	static string ERR;
	static void log(string text, bool newline = true, bool showTime = true) {
		if (!logger) {
			logger = new Logger();
		}
		string output = text;
		if (showTime) {
			output = logger->constructTime((long long unsigned) time(0)) + output;
		}
		if (newline) {
			output+="\n";
		}
		outfile << output;
		outfile.flush();
	}

	Logger(){
	}
	~Logger(){
		outfile.close();
	}
};
Logger* Logger::logger=NULL;
ofstream Logger::outfile("log.txt", ios::out);
string Logger::LINE = "=========================";
string Logger::ERR="Stwierdzam b³¹d, bo ";

class tekstura {
public:
	SDL_Surface *txt;
	GLenum format;
	GLenum internalformat;
	string roz;
	static GLuint txtid[iletxt];
	string nazwatxt;
	bool przezroczysta;
	void czyprzezroczysta() {
		unsigned wys;
		przezroczysta = false;
		if (format == GL_RGBA || format == GL_BGRA) {
			for (int i = 0; i < txt->h; i += 16)
				for (int j = 0; j < txt->w; j += 16) {
					wys = ((unsigned int*) txt->pixels)[i * (txt->pitch / sizeof(unsigned int)) + j];
					wys &= 0xFF000000;
					wys >>= 24;
					if (wys < 255) {
						Logger::log("Przezroczysta       " + nazwatxt);
						przezroczysta = true;
						return;
					}
				}
		}
	}

	static int czyjuzjesttekstura(string nazwa) {
		for (int i = 0; i < iletekstur; i++)
			if (tekstury[i]->nazwatxt == nazwa)
				return i;
		return -1;
	}

	GLenum jakiformat() {
		if (roz == ".png")
			switch (txt->format->BytesPerPixel) {
			case 4:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
					return GL_BGRA;
				else
					return GL_RGBA;
			case 3:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
					return GL_BGR;
				else
					return GL_RGB;
			}
		if (roz == ".tif")
			return GL_RGBA;
		if (roz == ".tga")
			switch (txt->format->BytesPerPixel) {
			case 4:
				return GL_BGRA;
			case 3:
				return GL_BGR;
			}

		if (roz == ".bmp")
			return GL_BGR;
		if (txt->format->BytesPerPixel == 1)
			return GL_INTENSITY;
		return GL_RGB;

	}

	string dej_rozszerzenie(string nazwa) {
		int pozycja = 0;
		for (unsigned i = 0; i < nazwa.length(); i++)
			if (nazwa[i] == '.')
				pozycja = i;
		string format = "";
		for (unsigned i = pozycja; i < nazwa.length(); i++)
			format += nazwa[i];
		for (unsigned i = 0; i < format.length(); i++)
			if (format[i] <= 'Z' && format[i] >= 'A')
				format[i] += 32;
		return format;
	}

	tekstura(string nazwa, string tex) {
		Logger::log("--Tekstura: " + nazwa);
		nazwatxt = nazwa;
		roz = dej_rozszerzenie(nazwa);
		txt = IMG_Load(nazwa.c_str());
		if (roz == ".jpg" || roz == ".jpeg" || roz == ".jpe" || roz == ".jif" || roz == ".jfif" || roz == ".jfi")
			roz = ".jpg";
		if (roz == ".bmp" || roz == ".dib")
			roz = ".bmp";
		if (roz == ".tif" || roz == ".tiff")
			roz = "tif";
		if (roz == ".tga" || roz == ".tpic")
			roz = ".tga";

		if (txt == NULL) {
			ostringstream ss;
			Logger::log(Logger::ERR + IMG_GetError());
			exit(0);
		}
		format = jakiformat();
		czyprzezroczysta();
		if (format == GL_RGBA || format == GL_BGRA) {
			if (przezroczysta)
				internalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			else
				internalformat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		}
		if (format == GL_RGB || format == GL_BGR)
			internalformat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		if (format == GL_INTENSITY)
			internalformat = GL_COMPRESSED_INTENSITY;

		glGenTextures(1, &txtid[iletekstur]);
		glBindTexture(GL_TEXTURE_2D, txtid[iletekstur]);
		//glTexStorage2D(GL_TEXTURE_2D, 16 ,txt->format->BytesPerPixel,txt->w,txt->h);
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, txt->w, txt->h, 0, format, GL_UNSIGNED_BYTE, txt->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (tex == "map_Kd") {
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);   //Interpolate RGB with RGB
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
			glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, kutas);

			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
		}

		glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, -2);
		GLclampf a = 1.0f;
		glPrioritizeTextures(1, &txtid[iletekstur], &a);
		SDL_FreeSurface(txt);

	}

};
GLuint tekstura::txtid[iletxt];

class material {
public:
	GLenum s;
	float kat[4];
	float kdt[4];
	float kst[4];
	int tkat;
	int tkdt;
	int tkst;
	float nst;
	string imie;

	void dajd(float d) {
		this->kat[3] = d;
		this->kdt[3] = d;
		this->kst[3] = d;
	}
	void dajnst(float nst) {
		this->nst = nst;
	}

	void dajambient(float kat[3]) {
		this->kat[0] = kat[0];
		this->kat[1] = kat[1];
		this->kat[2] = kat[2];
	}

	void dajdiffuse(float kdt[3]) {
		this->kdt[0] = kdt[0];
		this->kdt[1] = kdt[1];
		this->kdt[2] = kdt[2];
	}

	void dajspecular(float kst[3]) {
		this->kst[0] = kst[0];
		this->kst[1] = kst[1];
		this->kst[2] = kst[2];
	}

	void dajmap_kd(int tkdt) {
		this->tkdt = tkdt;
	}

	material(string imie) {
		tkat = -1;
		tkdt = -1;
		tkst = -1;
		s = GL_SMOOTH;
		this->imie = imie;
	}
};

class material_lib {
public:
	material **mtl;
	int ktorymaterial;
	string sciezka;

	int jakimaterial(string nazwa) {
		for (int i = 0; i <= this->ktorymaterial; i++)
			if (mtl[i]->imie == nazwa)
				return i;
		exit(0);
	}

	void wczytaj(string nazwa) {
		float k[3];
		float n;
		string imie;
		string napis;
		ifstream wczytywacz;
		int pom;

		wczytywacz.open(nazwa.c_str());
		if (!wczytywacz.is_open()) {
			Logger::log(Logger::ERR + "brak mtllib");
			exit(0);
		}

		while (!wczytywacz.eof()) {
			wczytywacz >> napis;

			if (napis == "newmtl") {
				wczytywacz >> napis;
				mtl[++this->ktorymaterial] = new material(napis);
			}
			if (napis == "Ns") {
				wczytywacz >> napis;
				n = atof(napis.c_str()) * 128.0 / 1000.0;
				mtl[this->ktorymaterial]->dajnst(n + 1);
			}

			if (napis == "Ka") {
				wczytywacz >> napis;
				k[0] = atof(napis.c_str());
				wczytywacz >> napis;
				k[1] = atof(napis.c_str());
				wczytywacz >> napis;
				k[2] = atof(napis.c_str());
				mtl[this->ktorymaterial]->dajambient(k);
			}

			if (napis == "Kd") {
				wczytywacz >> napis;
				k[0] = atof(napis.c_str());
				wczytywacz >> napis;
				k[1] = atof(napis.c_str());
				wczytywacz >> napis;
				k[2] = atof(napis.c_str());
				mtl[this->ktorymaterial]->dajdiffuse(k);
			}

			if (napis == "Ks") {
				wczytywacz >> napis;
				k[0] = atof(napis.c_str());
				wczytywacz >> napis;
				k[1] = atof(napis.c_str());
				wczytywacz >> napis;
				k[2] = atof(napis.c_str());
				mtl[this->ktorymaterial]->dajspecular(k);
			}

			if (napis == "d") {
				wczytywacz >> napis;
				n = atof(napis.c_str());
				mtl[this->ktorymaterial]->dajd(n);
			}

			if (napis == "map_Kd") {
				wczytywacz >> napis;
				string kutas = utnij(sciezka) + "/";
				pom = tekstura::czyjuzjesttekstura(kutas + napis);
				if (pom == -1) {
					tekstury[iletekstur++] = new tekstura(kutas + napis, "map_Kd");
					mtl[this->ktorymaterial]->dajmap_kd(iletekstur - 1);
				} else
					mtl[this->ktorymaterial]->dajmap_kd(pom);

			}
		}
		ostringstream stream;
		stream << "Utworzono " << ktorymaterial + 1 << " materialow";
		Logger::log(stream.str());
	}

	material_lib(string nazwa) {
		Logger::log("-MTL Lib: " + nazwa);
		sciezka = nazwa;
		ktorymaterial = -1;
		mtl = new material*[ilemtl];
		wczytaj(nazwa);
	}
	~material_lib() {
		for (int i = 0; i < this->ktorymaterial; i++)
			delete mtl;
	}

};

class podobiekt {
public:
	long long unsigned ilewierzcholkow;
	material *mtl;
	int ktorybuff[3];

	podobiekt(int ilewierzcholkow, int ktorymtl, material_lib* mtl, int ktorybuff) {
		this->ilewierzcholkow = ilewierzcholkow;
		this->mtl = mtl->mtl[ktorymtl];
		this->ktorybuff[0] = ktorybuff;
		this->ktorybuff[1] = ktorybuff + 1;
		this->ktorybuff[2] = ktorybuff + 2;
	}
};

class obiekt {
public:
	podobiekt **podobiekty;
	int ileobiektow;
	material_lib *mtl;
	static GLuint buff[ile * ileobj * 3 + 1];
	static int ilebuforow;
	static GLuint numerkowybuforXD;
	string nazwa;
	GLfloat min[3][3];
	GLfloat max[3][3];
	int licznik;
	int tmpmtl;

	int zwroc1(string a) {
		int x;
		string tmp = "";
		for (unsigned i = 0; i < a.size(); i++) {
			tmp += a[i];
			if (a[i] == '/')
				break;
		}
		x = atoi(tmp.c_str());
		if (x < 0)
			x *= -1;
		return x;
	}

	int zwroc2(string a) {
		int x = 0;
		unsigned i = 0;
		string tmp = "";
		while (x < 1)
			if (a[i++] == '/')
				x++;

		for (; i < a.size(); i++) {
			tmp += a[i];
			if (a[i] == '/')
				break;
		}
		x = atoi(tmp.c_str());
		if (x < 0)
			x *= -1;
		return x;
	}

	int zwroc3(string a) {
		int x = 0;
		unsigned i = 0;
		string tmp = "";
		while (x < 2)
			if (a[i++] == '/')
				x++;

		for (; i < a.size(); i++) {
			tmp += a[i];
			if (a[i] == '/')
				break;
		}
		x = atoi(tmp.c_str());
		if (x < 0)
			x *= -1;
		return x;
	}

	void takietamwczytywanie(string nazwa, bool tag) {
		Logger::log("Obiekt: " + nazwa);
		long long unsigned rozmiarpliku;
		long long unsigned ktorywierzcholek = 0;
		long long unsigned ktorynormalny = 0;
		long long unsigned ktoratekstura = 0;
		long long unsigned ktorywierzcholek2 = 0;
		long long unsigned ktorynormalny2 = 0;
		long long unsigned ktoratekstura2 = 0;
		long long unsigned ktoryfejs = 0;
		string napis;
		bool silnik_od_tostera = true;
		ifstream wczytywacz;
		wczytywacz.open(nazwa.c_str(), ios::binary);
		if (!wczytywacz.is_open()) {
			Logger::log(Logger::ERR + "brak .obj");
			exit(0);
		}
		wczytywacz.seekg(0, ios::end);
		rozmiarpliku = wczytywacz.tellg();
		wczytywacz.seekg(0, ios::beg);
		ostringstream stream;
		stream << "Rozmiar pliku " << rozmiarpliku << "B";
		Logger::log(stream.str());
		if (tag)
			rozmiarpliku = rozmiarpliku / 60 * 5;   //  /60
		else
			rozmiarpliku = rozmiarpliku / 60 * 3;
		float *w = new float[rozmiarpliku];
		float *n = new float[rozmiarpliku];
		float *t = new float[rozmiarpliku];
		int **f = new int*[rozmiarpliku];
		for (long long unsigned i = 0; i < rozmiarpliku; i++)
			f[i] = new int[3];

		while (!wczytywacz.eof() && napis != "mtllib") {
			wczytywacz >> napis;
		}
		wczytywacz >> napis;
		//xd << this->nazwa << endl;
		materialy[ilematerialow] = new material_lib(utnij(this->nazwa) + "/" + napis);
		mtl = materialy[ilematerialow++];

		while (!wczytywacz.eof()) {
			wczytywacz >> napis;
			if (napis == "o") {
				if (this->ileobiektow != -1)
					dorup(w, n, t, f, ktoryfejs, ktorywierzcholek2, ktorynormalny2, ktoratekstura2);
				this->ileobiektow++;
				wczytywacz >> napis;
				silnik_od_tostera = true;
			}

			if (napis == "usemtl") {
				wczytywacz >> napis;
				if (!silnik_od_tostera) {
					if (this->ileobiektow != -1)
						dorup(w, n, t, f, ktoryfejs, ktorywierzcholek2, ktorynormalny2, ktoratekstura2);
					this->ileobiektow++;
				}
				silnik_od_tostera = false;
				tmpmtl = mtl->jakimaterial(napis);

			}

			if (napis == "s") {
				wczytywacz >> napis;
				if (napis == "1")
					mtl->mtl[mtl->ktorymaterial]->s = GL_SMOOTH;
				else
					mtl->mtl[mtl->ktorymaterial]->s = GL_FLAT;
			}

			if (napis == "v") {

				wczytywacz >> napis;
				w[ktorywierzcholek++] = atof(napis.c_str());
				wczytywacz >> napis;
				w[ktorywierzcholek++] = atof(napis.c_str());
				wczytywacz >> napis;
				w[ktorywierzcholek++] = atof(napis.c_str());
				napis = "";
			}

			if (napis == "vt") {
				wczytywacz >> napis;
				t[ktoratekstura++] = atof(napis.c_str());
				wczytywacz >> napis;
				t[ktoratekstura++] = 1 - atof(napis.c_str());
				napis = "";
			}

			if (napis == "vn") {
				wczytywacz >> napis;
				n[ktorynormalny++] = atof(napis.c_str());
				wczytywacz >> napis;
				n[ktorynormalny++] = atof(napis.c_str());
				wczytywacz >> napis;
				n[ktorynormalny++] = atof(napis.c_str());
				napis = "";
			}

			if (napis == "f") {
				int tmp;
				for (int i = 0; i < 3; i++) {
					wczytywacz >> napis;
					tmp = zwroc1(napis) - 1;
					f[ktoryfejs][0] = tmp;
					tmp = zwroc2(napis) - 1;
					f[ktoryfejs][1] = tmp;
					tmp = zwroc3(napis) - 1;
					f[ktoryfejs++][2] = tmp;
				}
			}
		}
		int lol = 0;
		dorup(w, n, t, f, ktoryfejs, ktorywierzcholek2, ktorynormalny2, ktoratekstura2);
		for (int i = 0; i <= this->ileobiektow; i++)
			lol += podobiekty[i]->ilewierzcholkow;
		lol /= 3;
		ilee += lol;
		minimax(w, ktorywierzcholek);
		for (unsigned i = 0; i < rozmiarpliku; i++)
			delete[] f[i];
		delete[] f;
		delete[] w;
		delete[] n;
		delete[] t;
		stream.str("");
		Logger::log(Logger::LINE);
		stream << "Wczytano " << this->ileobiektow + 1 << " podobiektow";
		Logger::log(stream.str());
		stream.str("");
		stream << "Utworzono " << lol << " trojkatow\n\n";
		Logger::log(stream.str());
	}

	void dorup(float *w, float *n, float *t, int **f, long long unsigned &ktoryfejs,
			long long unsigned &ktorywierzcholek2, long long unsigned &ktorynormalny2,
			long long unsigned &ktoratekstura2) {
		GLfloat *normalne = new GLfloat[ktoryfejs * 3];
		GLfloat *wierzcholki = new GLfloat[ktoryfejs * 3];
		GLfloat *tekstury = new GLfloat[ktoryfejs * 2];
		for (long long unsigned i = 0; i < ktoryfejs; i++) {
			wierzcholki[ktorywierzcholek2++] = w[f[i][0] * 3];
			wierzcholki[ktorywierzcholek2++] = w[f[i][0] * 3 + 1];
			wierzcholki[ktorywierzcholek2++] = w[f[i][0] * 3 + 2];

			normalne[ktorynormalny2++] = n[f[i][2] * 3];
			normalne[ktorynormalny2++] = n[f[i][2] * 3 + 1];
			normalne[ktorynormalny2++] = n[f[i][2] * 3 + 2];

			tekstury[ktoratekstura2++] = t[f[i][1] * 2];
			tekstury[ktoratekstura2++] = t[f[i][1] * 2 + 1];
		}

		amore_bufore(normalne, wierzcholki, tekstury, ktoryfejs);
		delete[] wierzcholki;
		delete[] normalne;
		delete[] tekstury;
		podobiekty[this->ileobiektow] = new podobiekt(ktoryfejs, tmpmtl, mtl, ilebuforow);
		ilebuforow += 3;
		ktorynormalny2 = 0;
		ktoratekstura2 = 0;
		ktorywierzcholek2 = 0;
		ktoryfejs = 0;
	}

	void amore_bufore(GLfloat *normalne, GLfloat *wierzcholki, GLfloat *tekstury, int ktoryfejs) {
		//ilebuforow chujowe
		glGenBuffers(3, &buff[ilebuforow]);
		glBindBuffer(GL_ARRAY_BUFFER, buff[ilebuforow]);
		glBufferData(GL_ARRAY_BUFFER, ktoryfejs * 3 * sizeof(GLfloat), wierzcholki, GL_STATIC_READ);
		glBindBuffer(GL_ARRAY_BUFFER, buff[ilebuforow + 1]);
		glBufferData(GL_ARRAY_BUFFER, ktoryfejs * 3 * sizeof(GLfloat), normalne, GL_STATIC_READ);
		glBindBuffer(GL_ARRAY_BUFFER, buff[ilebuforow + 2]);
		glBufferData(GL_ARRAY_BUFFER, ktoryfejs * 2 * sizeof(GLfloat), tekstury, GL_STATIC_READ);
	}

	void ustaw(GLfloat t[3], GLfloat *w, int a) {
		t[0] = w[a];
		t[1] = w[a + 1];
		t[2] = w[a + 2];
	}

	void minimax(GLfloat *w, int ktorywierzcholek) {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				min[i][j] = numeric_limits<GLfloat>::infinity();
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				max[i][j] = -numeric_limits<GLfloat>::infinity();
		for (int i = 0; i < ktorywierzcholek; i += 3) {
			if (w[i] > max[0][0])
				ustaw(max[0], w, i);
			if (w[i + 1] > max[1][1])
				ustaw(max[1], w, i);
			if (w[i + 2] > max[2][2])
				ustaw(max[2], w, i);
			if (w[i] < min[0][0])
				ustaw(min[0], w, i);
			if (w[i + 1] < min[1][1])
				ustaw(min[1], w, i);
			if (w[i + 2] < min[2][2])
				ustaw(min[2], w, i);
		}
	}

	obiekt(string nazwa, bool tag = false) {
		podobiekty = new podobiekt*[ile];
		this->ileobiektow = -1;
		this->nazwa = otworz(nazwa, ".obj");
		tmpmtl = 0;
		takietamwczytywanie(this->nazwa, tag);
		licznik = 0;
		this->ileobiektow++;
	}
	~obiekt() {
		for (int i = 0; i < this->ileobiektow; i++)
			delete podobiekty[i];
		delete[] podobiekty;
		delete mtl;
	}
};

class obiekt_final {
public:
	obiekt *ob;
	obiekt_final *ociec;
	GLfloat px, py, pz;
	GLfloat sx, sy, sz;
	GLfloat rx, ry, rz;
	GLfloat min[3][3], max[3][3];
	GLfloat pomin[3][3], pomax[3][3];
	animacja *anim;
	bool zawsze;
	static unsigned int granica1, granica2;

	void zrup() {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++) {
				pomin[i][j] = min[i][j];
				pomax[i][j] = max[i][j];
			}
		update_obrot();
		update_skale();
		update_przesuniecie();
	}

	void update_obrot() {
		const float a = 0.01745329251;
		if (ry)
			for (int i = 0; i < 3; i++) {
				pomin[i][2] = pomin[i][0] * cos(a * ry) - pomin[i][2] * sin(a * ry);
				pomin[i][0] = pomin[i][0] * sin(a * ry) + pomin[i][2] * sin(a * ry);
				pomax[i][2] = pomax[i][0] * cos(a * ry) - pomax[i][2] * sin(a * ry);
				pomax[i][0] = pomax[i][0] * sin(a * ry) + pomin[i][2] * sin(a * ry);
			}
		if (rx)
			for (int i = 0; i < 3; i++) {
				pomin[i][1] = pomin[i][1] * cos(a * rx) - pomin[i][2] * sin(a * rx);
				pomin[i][2] = pomin[i][1] * sin(a * rx) + pomin[i][2] * sin(a * rx);
				pomax[i][1] = pomax[i][1] * cos(a * rx) - pomax[i][2] * sin(a * rx);
				pomax[i][2] = pomax[i][1] * sin(a * rx) + pomin[i][2] * sin(a * rx);
			}
		if (rz)
			for (int i = 0; i < 3; i++) {
				pomin[i][0] = pomin[i][0] * cos(a * rz) - pomin[i][1] * sin(a * rz);
				pomin[i][1] = pomin[i][0] * sin(a * rz) + pomin[i][1] * sin(a * rz);
				pomax[i][0] = pomax[i][0] * cos(a * rz) - pomax[i][1] * sin(a * rz);
				pomax[i][1] = pomax[i][0] * sin(a * rz) + pomax[i][1] * sin(a * rz);
			}

	}

	void update_przesuniecie() {
		for (int i = 0; i < 3; i++) {
			pomin[i][0] += px;
			pomax[i][0] += px;
			pomin[i][1] += py;
			pomax[i][1] += py;
			pomin[i][2] += pz;
			pomax[i][2] += pz;
		}
	}

	void update_skale() {
		for (int i = 0; i < 3; i++) {
			pomin[i][0] *= sx;
			pomax[i][0] *= sx;
			pomin[i][1] *= sy;
			pomax[i][1] *= sy;
			pomin[i][2] *= sz;
			pomax[i][2] *= sz;
		}
	}
	void dodaj_przesuniecie(GLfloat px, GLfloat py, GLfloat pz) {
		this->px = px;
		this->py = py;
		this->pz = pz;
	}

	void dodaj_skale(GLfloat sx, GLfloat sy, GLfloat sz) {
		this->sx = sx;
		this->sy = sy;
		this->sz = sz;

	}

	void dodaj_obrot(GLfloat rx, GLfloat ry, GLfloat rz) {
		this->rx = rx;
		this->ry = ry;
		this->rz = rz;
	}

	obiekt_final(obiekt *ktoryobiekt) {
		zawsze = false;
		anim = NULL;
		ociec = NULL;
		ob = ktoryobiekt;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				this->min[i][j] = ob->min[i][j];
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				this->max[i][j] = ob->max[i][j];
		this->px = 0;
		this->py = 0;
		this->pz = 0;
		this->sx = 1;
		this->sy = 1;
		this->sz = 1;
		this->rx = 0;
		this->ry = 0;
		this->rz = 0;
		ob->licznik++;
		Logger::log("Tworze obiekt " + this->ob->nazwa, true);
	}

	~obiekt_final() {
		Logger::log("Usuwam obiekt " + this->ob->nazwa, true);
		for (int i = 0; i < ileobiektow; i++)
			if (obiekty_f[i]->ociec == this)
				obiekty_f[i]->ociec = ociec;
		for (unsigned i = 0; i < obiekt_final::granica2; i++)
			if (obiekty_posortowane[i] == this)
				obiekty_posortowane[i] = NULL;
		ob->licznik--;
		ileobiektow--;
	}
};

GLuint obiekt::buff[ile * ileobj * 3 + 1];
int obiekt::ilebuforow = 0;
GLuint obiekt::numerkowybuforXD;
unsigned int obiekt_final::granica1 = 0;
unsigned int obiekt_final::granica2 = 0;

class animacja {
public:
	GLfloat startpx, startpy, startpz;
	GLfloat startsx, startsy, startsz;
	GLfloat startrx, startry, startrz;
	float p[ilekrokowwanimacji][3];
	float s[ilekrokowwanimacji][3];
	float r[ilekrokowwanimacji][3];
	bool petla;
	int ktorykrok;
	int ilekrokow;
	float speed[ilekrokowwanimacji];
	int licznik;
	string nazwa;
	void dodaj_p(float a, float b, float c) {
		p[ilekrokow][0] = a;
		p[ilekrokow][1] = b;
		p[ilekrokow][2] = c;
	}

	void dodaj_s(float a, float b, float c) {
		s[ilekrokow][0] = a;
		s[ilekrokow][1] = b;
		s[ilekrokow][2] = c;
	}

	void dodaj_r(float a, float b, float c) {
		r[ilekrokow][0] = a;
		r[ilekrokow][1] = b;
		r[ilekrokow][2] = c;
	}

	void animuj(obiekt_final *ob) {
		if (ktorykrok != -1) {
			ob->px += p[ktorykrok][0] * speed[ktorykrok];
			ob->py += p[ktorykrok][1] * speed[ktorykrok];
			ob->pz += p[ktorykrok][2] * speed[ktorykrok];
			ob->sx += s[ktorykrok][0] * speed[ktorykrok];
			ob->sy += s[ktorykrok][1] * speed[ktorykrok];
			ob->sz += s[ktorykrok][2] * speed[ktorykrok];
			ob->rx += r[ktorykrok][0] * speed[ktorykrok];
			ob->ry += r[ktorykrok][1] * speed[ktorykrok];
			ob->rz += r[ktorykrok][2] * speed[ktorykrok];
			if (ob->rx > 360)
				ob->rx -= 360;
			if (ob->ry > 360)
				ob->ry -= 360;
			if (ob->rz > 360)
				ob->rz -= 360;
			licznik--;
			if (licznik == 0) {
				ktorykrok++;
				if (ktorykrok >= ilekrokow) {
					if (petla) {
						ktorykrok = 0;
						licznik = 1 / speed[ktorykrok];
					} else
						ktorykrok = -1;
				} else
					licznik = 1 / speed[ktorykrok];
			}
		}

	}

	animacja(string sciezka, string nazwa, obiekt_final *ob) {
		for (int i = 0; i < ilekrokowwanimacji; i++)
			for (int j = 0; j < 3; j++) {
				p[i][j] = 0;
				s[i][j] = 0;
				r[i][j] = 0;
			}
		startpx = ob->px;
		startpy = ob->py;
		startpz = ob->pz;
		startsx = ob->sx;
		startsy = ob->sy;
		startsz = ob->sz;
		startrx = ob->rx;
		startry = ob->ry;
		startrz = ob->rz;
		ktorykrok = 0;
		ilekrokow = 0;
		petla = false;
		fstream otwieracz;
		float a, b, c;
		this->nazwa = nazwa;
		sciezka = "animacje/" + nazwa + ".txt";
		otwieracz.open(sciezka.c_str());
		if (!otwieracz.is_open()) {
			Logger::log(Logger::ERR + "brak pliku z animacja");
			exit(0);
		}

		string kutacz;
		while (!otwieracz.eof()) {
			otwieracz >> kutacz;
			if (kutacz == "speed") {
				otwieracz >> a;
				speed[ilekrokow] = a;
			}

			if (kutacz == "p") {
				otwieracz >> a >> b >> c;
				dodaj_p(a, b, c);
			}

			if (kutacz == "s") {
				otwieracz >> a >> b >> c;
				dodaj_s(a, b, c);
			}

			if (kutacz == "r") {
				otwieracz >> a >> b >> c;
				dodaj_r(a, b, c);
			}

			if (kutacz == "petla")
				petla = true;
			if (kutacz == "#")
				ilekrokow++;

		}
		ilekrokow++;
		licznik = 1 / speed[0];
	}

};

class mapa {
public:
	static int **wysokosc;
	static int map_x, map_z;
	static long double stosunekx;
	static long double stosuneky;
	static long double stosunekz;
	int wymx, wymz;
	long long unsigned rozmiarmapy;

	int DeleteDirectory(const string &refcstrRootDirectory, bool bDeleteSubdirectories = true) {
		bool bSubdirectory = false;       // Flag, indicating whether
										  // subdirectories have been found
		HANDLE hFile;                       // Handle to directory
		string strFilePath;                 // Filepath
		string strPattern;                  // Pattern
		WIN32_FIND_DATA FileInformation;             // File information

		strPattern = refcstrRootDirectory + "\\*.*";
		hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
		if (hFile != INVALID_HANDLE_VALUE) {
			do {
				if (FileInformation.cFileName[0] != '.') {
					strFilePath.erase();
					strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

					if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						if (bDeleteSubdirectories) {
							// Delete subdirectory
							int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
							if (iRC)
								return iRC;
						} else
							bSubdirectory = true;
					} else {
						// Set file attributes
						if (::SetFileAttributes(strFilePath.c_str(),
						FILE_ATTRIBUTE_NORMAL) == FALSE)
							return ::GetLastError();

						// Delete file
						if (::DeleteFile(strFilePath.c_str()) == FALSE)
							return ::GetLastError();
					}
				}
			} while (::FindNextFile(hFile, &FileInformation) == TRUE);

			// Close handle
			::FindClose(hFile);

			DWORD dwError = ::GetLastError();
			if (dwError != ERROR_NO_MORE_FILES)
				return dwError;
			else {
				if (!bSubdirectory) {
					// Set directory attributes
					if (::SetFileAttributes(refcstrRootDirectory.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();
				}
			}
		}

		return 0;
	}

	static float oblicz_wysokosc(float a2, float b2, float c2) {
		int aa, cc;
		long double a, c;
		float suma = 0, suma2 = 0;
		a = (long double) ((a2) / stosunekx) + (long double) (map_x / 2);	//-1
		c = (long double) ((c2) / stosunekz) + (long double) (map_z / 2);		//0
		aa = (int) a;
		cc = (int) c;
		if (aa < 0 || a >= map_x || cc < 0 || cc >= map_z)
			return 0;
		if (a == aa && c == cc)
			return wysokosc[map_x - aa][cc] * stosuneky + b2;

		if (a == aa) {
			suma = (wysokosc[map_x - aa][cc] - wysokosc[map_x - aa][cc + 1]) * (c - cc);
			return (wysokosc[map_x - aa][cc] - suma) * stosuneky + b2;
		}

		if (c == cc) {
			suma = (wysokosc[map_x - aa][cc] - wysokosc[map_x - aa + 1][cc]) * (a - aa);
			return (wysokosc[map_x - aa][cc] - suma) * stosuneky + b2;
		}

		if (c - cc < 0.5 && a - aa < 0.5) {
			suma = (wysokosc[map_x - aa][cc] - wysokosc[map_x - aa][cc + 1]) * (c - cc);
			suma2 = (wysokosc[map_x - aa][cc] - wysokosc[map_x - aa + 1][cc]) * (a - aa);
			return (wysokosc[map_x - aa][cc] - suma - suma2) * stosuneky + b2;
		}

		else {
			suma = (wysokosc[map_x - aa + 1][cc + 1] - wysokosc[map_x - aa + 1][cc]) * (1 - a + aa);
			suma2 = (wysokosc[map_x - aa + 1][cc + 1] - wysokosc[map_x - aa][cc + 1]) * (1 - c + cc);
			return (wysokosc[map_x - aa + 1][cc + 1] - suma - suma2) * stosuneky + b2;
		}

		//return 1000;
		return 0;
	}

	void sprawdz_co() {
		fstream wczytywacz;
		wczytywacz.open("ustawienia/mapa.txt");

		string nazwa;
		string tex;
		string mtl;
		wczytywacz >> nazwa;
		wczytywacz >> tex;
		wczytywacz >> mtl;
		wczytywacz >> wymx;
		wczytywacz >> wymz;
		wczytywacz.close();
		wczytajmape(nazwa, tex, mtl);
	}

	void normalizuj(float *t1) {
		float d = sqrt(t1[0] * t1[0] + t1[1] * t1[1] + t1[2] * t1[2]);
		if (d != 0) {
			t1[0] /= d;
			t1[1] /= d;
			t1[2] /= d;
		}
	}

	void kopiuj(string mtl, string tekstura, string dupaa) {
		ifstream src(mtl.c_str(), ios::binary);
		ofstream dst("modele/0/0.mtl", ios::binary);
		ifstream src2(tekstura.c_str(), ios::binary);
		ofstream dst2(dupaa.c_str(), ios::binary);
		dst << src.rdbuf();
		dst2 << src2.rdbuf();
		src.close();
		dst.close();
		src2.close();
		dst2.close();
	}

	void wczytaj_wysokosci() {
		wysokosc = new int*[map_x];
		for (int i = 0; i < map_x; i++)
			wysokosc[i] = new int[map_z];
		fstream wczytywacz;
		wczytywacz.open("mapy/wysokosc.txt");
		if (!wczytywacz.is_open()) {
			Logger::log(Logger::ERR + "nie ma wysokosci");
			exit(0);
		}

		for (int i = 0; i < map_x; i++)
			for (int j = 0; j < map_z; j++)
				wczytywacz >> wysokosc[i][j];
		wczytywacz.close();
	}

	bool sprawdz(string nazwa) {
		fstream sprawdzacz, sprawdzacz2;
		bool tag = false;
		sprawdzacz.open("modele/0/0.obj");
		if (!sprawdzacz.is_open())
			Logger::log("Nie ma mapy, probuje utworzyc");
		else {
			sprawdzacz2.open("mapy/ostatnia.txt");
			if (sprawdzacz2.is_open()) {
				ifstream sprawdzacz3;
				long long unsigned a;
				string b;
				sprawdzacz2 >> b;
				sprawdzacz2 >> a;
				sprawdzacz2 >> map_x;
				sprawdzacz2 >> map_z;
				rozmiarmapy = sprawdz_rozmiar(nazwa);
				if (a == rozmiarmapy && nazwa == b) {
					Logger::log("Jest zrobiona mapa, wczytuje...");
					tag = true;
					wczytaj_wysokosci();
					stosunekx = (float) wymx / (float) map_x;
					stosunekz = (float) wymz / (float) map_z;
					stosuneky = 2;
					obiekty[ileobiektow2++] = new obiekt("0", true);
				}

			}
		}
		sprawdzacz.close();
		sprawdzacz2.close();
		return tag;
	}

	float* zrup_normalny(float t1[3], float t2[3], float t3[3]) {
		float temp1[3], temp2[3];
		float *vec = new float[3];
		for (int k = 0; k < 3; k++) {
			temp1[k] = t2[k] - t1[k];
			temp2[k] = t3[k] - t1[k];
		}

		vec[0] = temp1[1] * temp2[2] - temp1[2] * temp2[1];
		vec[1] = temp1[2] * temp2[0] - temp1[0] * temp2[2];
		vec[2] = temp1[0] * temp2[1] - temp1[1] * temp2[0];
		return vec;
	}

	void wczytajmape(string nazwa2, string tekstura2, string mtl2) {
		string nazwa = "mapy/mapy/" + nazwa2;
		string tekstura = "mapy/tekstury/" + tekstura2;
		string mtl = "mapy/mtl/" + mtl2;
		string dupaa = "modele/0/tex.";
		dupaa += tekstura2[tekstura2.size() - 3];
		dupaa += tekstura2[tekstura2.size() - 2];
		dupaa += tekstura2[tekstura2.size() - 1];
		bool tag = sprawdz(nazwa);
		if (tag)
			return;
		DeleteDirectory("modele/0/");
		kopiuj(mtl, tekstura, dupaa);
		Logger::log("Nie ma mapy, probuje utworzyc\n");
		SDL_Surface *txt = IMG_Load(nazwa.c_str());
		if (txt == NULL) {
			Logger::log(Logger::ERR + "chujowy obrazek");
			exit(0);
		}
		stosunekx = (float) wymx / (float) txt->w;
		stosunekz = (float) wymz / (float) txt->h;
		stosuneky = 2;
		map_x = txt->w;
		map_z = txt->h;
		unsigned pixel, r, g, b;
		long double wys2;
		fstream zapisywacz, zapisywacz2;
		zapisywacz.open("modele/0/0.obj", ios::out);
		zapisywacz2.open("mapy/wysokosc.txt", ios::out);
		zapisywacz << "mtllib 0.mtl\no kutas" << endl;
		int bpp = txt->format->BytesPerPixel;

		wysokosc = new int*[txt->h];
		for (int i = 0; i < txt->h; i++)
			wysokosc[i] = new int[txt->w];
		float **vec = new float*[(txt->w - 1) * (txt->h - 1) * 2];
		int v = 0;

		for (int i = 0; i < txt->h; i++)
			for (int j = 0; j < txt->w; j++) {
				pixel = ((Uint32*) txt->pixels)[i * (txt->pitch / sizeof(Uint32)) + j * bpp / 4];
				r = pixel & 0x000000FF;
				g = pixel & 0x0000FF00;
				b = pixel & 0x00FF0000;
				g >>= 8;
				b >>= 16;
				wys2 = (r + b + g) / 3;
				zapisywacz << "v " << txt->h / 2 - i << " " << wys2 - 128 << " " << j - txt->w / 2 << endl; //moze sie zjebac!
				zapisywacz2 << wys2 - 128 << " ";
				wysokosc[i][j] = wys2 - 128;
			}
		zapisywacz2.close();
		float t1[3], t2[3], t3[3];
		for (int i = 0; i < txt->h - 1; i++)
			for (int j = 0; j < txt->w - 1; j++) {
				t1[0] = txt->h / 2 - i;
				t1[1] = wysokosc[i][j];  //i,j
				t1[2] = j - txt->w / 2;

				t2[0] = txt->h / 2 - i;
				t2[1] = wysokosc[i][j + 1]; //i,j+1
				t2[2] = j + 1 - txt->w / 2;

				t3[0] = txt->h / 2 - i - 1;
				t3[1] = wysokosc[i + 1][j]; //i+1,j
				t3[2] = j - txt->w / 2;

				vec[v++] = zrup_normalny(t1, t3, t2);

				t1[0] = txt->h / 2 - i - 1;
				t1[1] = wysokosc[i + 1][j];  //i+1,j
				t1[2] = j - txt->w / 2;

				t2[0] = txt->h / 2 - i;
				t2[1] = wysokosc[i][j + 1]; //i,j+1
				t2[2] = j + 1 - txt->w / 2;

				t3[0] = txt->h / 2 - i - 1;
				t3[1] = wysokosc[i + 1][j + 1]; //i+1,j+1
				t3[2] = j + 1 - txt->w / 2;

				vec[v++] = zrup_normalny(t3, t2, t1);
			}
		zapisywacz << "vt 0 0" << endl;
		zapisywacz << "vt 0 2" << endl;
		zapisywacz << "vt 2 0" << endl;
		zapisywacz << "vt 2 2" << endl;
		zapisywacz << "usemtl cipa" << endl;
		zapisywacz << "s 1" << endl;
		int przes, przes2;
		float lol[3];
		for (int i = 0; i < txt->h; i++)
			for (int j = 0; j < txt->w; j++) {
				przes = i * (txt->w - 1) * 2 + j * 2;
				przes2 = (i - 1) * (txt->w - 1) * 2 + j * 2;
				lol[0] = 0;
				lol[1] = 0;
				lol[2] = 0;
				if (j != txt->w - 1 && i != txt->h - 1) {
					lol[0] += vec[przes][0];
					lol[1] += vec[przes][1];
					lol[2] += vec[przes][2];
				}

				if (j != 0 && i != txt->h - 1) {
					lol[0] += vec[przes - 2][0];
					lol[1] += vec[przes - 2][1];
					lol[2] += vec[przes - 2][2];

					lol[0] += vec[przes - 1][0];
					lol[1] += vec[przes - 1][1];
					lol[2] += vec[przes - 1][2];
				}

				if (j != 0 && i != 0) {
					lol[0] += vec[przes2 - 1][0];
					lol[1] += vec[przes2 - 1][1];
					lol[2] += vec[przes2 - 1][2];
				}

				if (j != txt->w - 1 && i != 0) {
					lol[0] += vec[przes2][0];
					lol[1] += vec[przes2][1];
					lol[2] += vec[przes2][2];

					lol[0] += vec[przes2 + 1][0];
					lol[1] += vec[przes2 + 1][1];
					lol[2] += vec[przes2 + 1][2];
				}
				normalizuj(lol);
				zapisywacz << "vn " << lol[0] << " " << lol[1] << " " << lol[2] << endl;
			}

		for (int i = 0; i < txt->h - 1; i++)
			for (int j = 1; j < txt->w - 1; j++) {
				przes = i * txt->w + j;
				przes2 = (i + 1) * txt->w + j;
				zapisywacz << "f " << przes << "/1/" << przes << " " << przes + 1 << "/2/" << przes + 1 << " " << przes2
						<< "/3/" << przes2 << endl;
				zapisywacz << "f " << przes + 1 << "/2/" << przes + 1 << " " << przes2 << "/3/" << przes2 << " "
						<< przes2 + 1 << "/4/" << przes2 + 1 << endl;
			}
		Logger::log("Utworzono mape");
		fstream sprawdzacz2;
		sprawdzacz2.open("mapy/ostatnia.txt", ios::out);
		sprawdzacz2 << nazwa << endl;
		sprawdzacz2 << rozmiarmapy << endl;
		sprawdzacz2 << txt->w << endl;
		sprawdzacz2 << txt->h;
		sprawdzacz2.close();
		SDL_FreeSurface(txt);
		for (int i = 0; i < v; i++)
			delete[] vec[i];
		delete[] vec;
		obiekty[ileobiektow2++] = new obiekt("0", true);
		zapisywacz.close();
	}

	mapa() {
		sprawdz_co();
	}
};
int** mapa::wysokosc;
int mapa::map_x = 0;
int mapa::map_z = 0;
long double mapa::stosunekx;
long double mapa::stosuneky;
long double mapa::stosunekz;

class obcinanie {
public:
	float fardist;
	float neardist;
	float bottom, top;
	float ar;
	float tan;
	float pcx, pcx2, pcy, pcy2, pcz, pcz2;
	float h, h2;
	GLfloat px2, py2, pz2;

	void licz(float tab[3], float tab2[3], float &p1, float &p2, float md1, float md2, float md3) {
		float v[3], v2[3];
		v[0] = tab[0] - px2;
		v[1] = tab[1] - py2;
		v[2] = tab[2] - pz2;
		p1 = -md1 * v[0] - md2 * v[1] - md3 * v[2];
		v2[0] = tab2[0] - px2;
		v2[1] = tab2[1] - py2;
		v2[2] = tab2[2] - pz2;
		p2 = -md1 * v2[0] - md2 * v2[1] - md3 * v2[2];
	}

	bool sprawdzX(float tab[3], float tab2[3]) {
		h = pcz * tan;
		h2 = pcz2 * tan;
		float w = h * 2.0 * ar / (top - bottom);
		float w2 = h2 * 2.0 * ar / (top - bottom);
		licz(tab, tab2, pcx, pcx2, modelview[0], modelview[4], modelview[8]);
		if ((pcx > -w && pcx < w) || (pcx2 > -w2 && pcx2 < w2))
			return true;
		return false;
	}

	bool sprawdzY(float tab[3], float tab2[3]) {
		h = pcz * tan;
		h2 = pcz2 * tan;
		licz(tab, tab2, pcy, pcy2, modelview[1], modelview[5], modelview[9]);
		if ((pcy > -h && pcy < h) || (pcy2 > -h2 && pcy2 < h2))
			return true;
		return false;
	}

	bool sprawdzZ(float tab[3], float tab2[3]) {
		licz(tab, tab2, pcz, pcz2, modelview[2], modelview[6], modelview[10]);
		if ((pcz < fardist && pcz > neardist) || (pcz2 < fardist && pcz2 > neardist))
			return true;
		return false;
	}

	bool nalezy(int i) {
		if (obiekty_f[i]->zawsze)
			return true;
		if (ktorykutas == -1) {
			px2 = px;
			py2 = py;
			pz2 = pz;
		} else {
			pz2 = kamera;
			const double a = 0.01745329251;
			py2 = -pz2 * sin(-a * cx);
			pz2 = pz2 * cos(-a * cx);
			px2 = -pz2 * sin(a * cy);
			pz2 = pz2 * cos(a * cy);

			px2 += wybrany->px;
			py2 += wybrany->py + 5;
			pz2 += wybrany->pz;

		}
		obiekty_f[i]->zrup();
		if (sprawdzZ(obiekty_f[i]->pomin[2], obiekty_f[i]->pomax[2])
				&& sprawdzY(obiekty_f[i]->pomin[1], obiekty_f[i]->pomax[1])
				&& sprawdzX(obiekty_f[i]->pomin[0], obiekty_f[i]->pomax[0]))
			return true;
		return false;
	}

	obcinanie() {
		neardist = 1.0;
		fardist = 100000.0;
		bottom = -0.5;
		top = 0.5;
		tan = (top - bottom) / neardist;
	}
};

void resize(int width, int height) {
	const float ar = (float) width / (float) height / 2;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, ciach->bottom, ciach->top, ciach->neardist, ciach->fardist);
	ciach->ar = ar;
	glMatrixMode(GL_MODELVIEW);
	wys = height;
	szer = width;
}

void rysuj(obiekt_final *ob) {
	GLfloat p1 = ob->px;
	GLfloat p2 = ob->py;
	GLfloat p3 = ob->pz;
	obiekt_final *kutas = ob->ociec;
	while (kutas) {
		//xd<<kutas<<endl;
		p1 += kutas->px;
		p2 += kutas->py;
		p3 += kutas->pz;
		kutas = kutas->ociec;
	}
	glPushMatrix();
	glTranslatef(p1, p2, p3);
	glRotatef(ob->rx, 1, 0, 0);
	glRotatef(ob->ry, 0, 1, 0);
	glRotatef(ob->rz, 0, 0, 1);
	glScalef(ob->sx, ob->sy, ob->sz);

	for (int j = 0; j < ob->ob->ileobiektow; j++) {
		glShadeModel(ob->ob->podobiekty[j]->mtl->s);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ob->ob->podobiekty[j]->mtl->kat);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ob->ob->podobiekty[j]->mtl->kdt);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ob->ob->podobiekty[j]->mtl->kst);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, ob->ob->podobiekty[j]->mtl->nst);
		/*
		 if(ob->ob->mtl->td[ob->ob->ktorymtl[j]])
		 {
		 glEnable(GL_TEXTURE1);
		 glActiveTexture(GL_TEXTURE1);
		 glClientActiveTexture( GL_TEXTURE1 );
		 glEnable(GL_TEXTURE_2D);
		 glBindBuffer(GL_ARRAY_BUFFER, ob->ob->ktorybuff[j][2]);
		 glTexCoordPointer(2, GL_FLOAT, 0, 0);
		 glBindTexture(GL_TEXTURE_2D, tekstura::txtid[ob->ob->mtl->tdt[ob->ob->ktorymtl[j]]]);
		 glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		 }
		 */
		if (ob->ob->podobiekty[j]->mtl->tkdt != -1) {
			//	glActiveTexture(GL_TEXTURE0);
			//	glClientActiveTexture( GL_TEXTURE0 );
			glEnable(GL_TEXTURE_2D);
			glBindBuffer(GL_ARRAY_BUFFER, obiekt::buff[ob->ob->podobiekty[j]->ktorybuff[2]]);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
			glBindTexture(GL_TEXTURE_2D, tekstura::txtid[ob->ob->podobiekty[j]->mtl->tkdt]);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		glBindBuffer(GL_ARRAY_BUFFER, obiekt::buff[ob->ob->podobiekty[j]->ktorybuff[0]]);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, obiekt::buff[ob->ob->podobiekty[j]->ktorybuff[1]]);
		glNormalPointer(GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, ob->ob->podobiekty[j]->ilewierzcholkow);
		//glDrawElements(GL_TRIANGLES,ob->ob->podobiekty[j]->ilewierzcholkow, GL_UNSIGNED_INT, 0);

		//	glDisable(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	glPopMatrix();
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!ramki++)
		licznik = clock();
	if (czywsp) {
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
		glLoadIdentity();

		DrawString(-2.32, 1.2, -2.5, "FPS: " + info.fps);
		DrawString(-2.32, 1.15, -2.5, "X: " + info.x1);
		DrawString(-2.32, 1.1, -2.5, "Y: " + info.y1);
		DrawString(-2.32, 1.05, -2.5, "Z: " + info.z1);
		DrawString(-2.32, 1, -2.5, "Speed: " + info.speed);

		DrawString(-2.32, 0.95, -2.5, "Ambient: " + info.amb);
		DrawString(-2.32, 0.9, -2.5, "Diffuse: " + info.diff);
		DrawString(-2.32, 0.85, -2.5, "Specular: " + info.spec);
		DrawString(-2.32, 0.8, -2.5, "Pos: " + info.pos);
		DrawString(-2.32, 0.75, -2.5, "Pos: " + info.poss);
		DrawString(-2.32, 0.7, -2.5, "Wszystkie obiekty: " + info.ileob + "   Wyswietlone obiekty: " + info.ileob2);
		DrawString(-2.32, 0.65, -2.5,
				"Obiekt: " + info.ob2 + "  " + obiekty[ktorykutas2]->nazwa + "   sztuk: " + info.licznikob);
		if (ktorykutas != -1) {
			DrawString(-2.32, 0.6, -2.5, "Zaznaczony obiekt: " + info.ob + "  " + wybrany->ob->nazwa);
			if (obiekty_f[ktorykutas]->ociec)
				DrawString(-2.32, 0.55, -2.5, "Dziecko obiektu: " + wybrany->ociec->ob->nazwa);
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glLoadIdentity();
	if (ktorykutas > -1)
		glTranslatef(0, 0, -kamera); //obrot kamery
	glRotatef(cx, 1, 0, 0);
	glRotatef(cy, 0, 1, 0);

	if (ktorykutas == -1)
		glTranslatef(-px, -py, -pz);
	else {
		if (obracamy) {
			wybrany->rx = cx2;
			wybrany->ry = -cy2;
			wybrany->rz = cz2;
		}
		wybrany->px = px;
		wybrany->py = py;
		wybrany->pz = pz;
		GLfloat p1 = px;
		GLfloat p2 = py;
		GLfloat p3 = pz;
		obiekt_final *kutas = wybrany->ociec;
		while (kutas) {
			p1 += kutas->px;
			p2 += kutas->py;
			p3 += kutas->pz;
			kutas = kutas->ociec;
		}
		glTranslatef(-p1, -p2 - 5, -p3);
	}
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	glDisable(GL_BLEND);
	for (unsigned i = 0; i < obiekt_final::granica1; i++)
		if (obiekty_posortowane[i])
			rysuj(obiekty_posortowane[i]);
	glEnable(GL_BLEND);
	for (unsigned i = obiekt_final::granica1; i < obiekt_final::granica2; i++) {
		if (obiekty_posortowane[i])
			rysuj(obiekty_posortowane[i]);
	}

	//for(int i=0; i<ileobiektow; i++) rysuj(obiekty_f[i]);
	glutSwapBuffers();
}

void klawiaturka(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
		break;

	case 'w':
		pz -= modelview[10] * predkosc;
		px -= modelview[2] * predkosc;
		break;

	case 's':
		pz += modelview[10] * predkosc;
		px += modelview[2] * predkosc;
		break;

	case 'd':
		px += modelview[10] * predkosc;
		pz -= modelview[2] * predkosc;
		break;

	case 'a':
		px -= modelview[10] * predkosc;
		pz += modelview[2] * predkosc;
		break;

	case 'q':
		py += predkosc;
		break;

	case 'e':
		py -= predkosc;
		break;

	case 'o':
		if (czywsp)
			czywsp = false;
		else
			czywsp = true;
		break;

	case '=':
		if (predkosc < 10)
			predkosc += 0.1;
		break;

	case '-':
		if (predkosc > 0)
			predkosc -= 0.1;
		break;

	case '{':
		if (ktoreswiatlo > 0)
			ktoreswiatlo--;
		break;

	case '}':
		if (ktoreswiatlo < 3)
			ktoreswiatlo++;
		break;

	case '[':
		if (ktorapos > 0)
			ktorapos--;
		break;

	case ']':
		if (ktorapos < 3)
			ktorapos++;
		break;

	case 39:
		switch (ktoreswiatlo) {
		case 0:
			if (light_ambient[ktorapos] < 1)
				light_ambient[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			break;
		case 1:
			if (light_diffuse[ktorapos] < 1)
				light_diffuse[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
			break;
		case 2:
			if (light_specular[ktorapos] < 1)
				light_specular[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
			break;
		case 3:
			light_position[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			break;
		}
		break;

	case ';':
		switch (ktoreswiatlo) {
		case 0:
			if (light_ambient[ktorapos] > 0)
				light_ambient[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			break;
		case 1:
			if (light_diffuse[ktorapos] > 0)
				light_diffuse[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
			break;
		case 2:
			if (light_specular[ktorapos] > 0)
				light_specular[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
			break;
		case 3:
			light_position[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			break;
		}
		break;

	case '8':
		obiekty_f[ktorykutas]->sx += predkosc;
		obiekty_f[ktorykutas]->sy += predkosc;
		obiekty_f[ktorykutas]->sz += predkosc;
		if (obiekty_f[ktorykutas]->anim) {
			obiekty_f[ktorykutas]->anim->startsx += predkosc;
			obiekty_f[ktorykutas]->anim->startsy += predkosc;
			obiekty_f[ktorykutas]->anim->startsz += predkosc;
		}
		break;

	case '5':
		obiekty_f[ktorykutas]->sx -= predkosc;
		obiekty_f[ktorykutas]->sy -= predkosc;
		obiekty_f[ktorykutas]->sz -= predkosc;
		if (obiekty_f[ktorykutas]->anim) {
			obiekty_f[ktorykutas]->anim->startsx -= predkosc;
			obiekty_f[ktorykutas]->anim->startsy -= predkosc;
			obiekty_f[ktorykutas]->anim->startsz -= predkosc;
		}
		break;

	case '4':
		if (ktorykutas > -1) {
			wybrany = obiekty_f[--ktorykutas];
			px = obiekty_f[ktorykutas]->px;
			py = obiekty_f[ktorykutas]->py;
			pz = obiekty_f[ktorykutas]->pz;
			cx2 = -obiekty_f[ktorykutas]->rx;
			cy2 = -obiekty_f[ktorykutas]->ry;
		}
		break;

	case '6':
		if (ktorykutas < ileobiektow - 1) {
			wybrany = obiekty_f[++ktorykutas];
			px = obiekty_f[ktorykutas]->px;
			py = obiekty_f[ktorykutas]->py;
			pz = obiekty_f[ktorykutas]->pz;
			cx2 = -obiekty_f[ktorykutas]->rx;
			cy2 = -obiekty_f[ktorykutas]->ry;
		}
		break;

	case '7':
		if (ktorykutas != -1) {
			delete obiekty_f[ktorykutas];
			obiekty_f[ktorykutas] = obiekty_f[ileobiektow];
			obiekty_f[ileobiektow] = NULL;
			ktorykutas = -1;
			wybrany = NULL;
		}
		break;

	case '9':
		obiekty_f[ileobiektow++] = new obiekt_final(obiekty[ktorykutas2]);
		if (ktorykutas != -1)
			obiekty_f[ileobiektow - 1]->ociec = obiekty_f[ktorykutas];
		ktorykutas = ileobiektow - 1;
		wybrany = obiekty_f[ktorykutas];
		break;

	case '1':
		if (ktorykutas2 > 0)
			ktorykutas2--;
		break;

	case '3':
		if (ktorykutas2 < ileobiektow2 - 1)
			ktorykutas2++;
		break;

	case '*':
		zapisz();
		break;

	}

	glutPostRedisplay();
}

void mysza(int x, int y) {
	int w, h;
	w = glutGet( GLUT_WINDOW_WIDTH);
	h = glutGet( GLUT_WINDOW_HEIGHT);
	cx -= (h / 2 - y) / 20.0;
	cy -= (w / 2 - x) / 20.0;
	if (cx > 90)
		cx = 90;
	if (cx < -90)
		cx = -90;
	obracamy = false;
	myk++;
	if (myk >= 3) {
		glutWarpPointer(w / 2, h / 2);
		myk = 0;
	}
}

void mysza2(int x, int y) {
	int w, h;
	w = glutGet( GLUT_WINDOW_WIDTH);
	h = glutGet( GLUT_WINDOW_HEIGHT);
	if (wcisnietylewy) {
		cx2 -= (h / 2 - y) / 15.0;
		cz2 -= (w / 2 - x) / 15.0;
	}

	if (wcisnietyprawy)
		cy2 -= (w / 2 - x) / 15.0;
	myk2++;
	obracamy = true;
	if (myk2 >= 3) {
		glutWarpPointer(w / 2, h / 2);
		myk2 = 0;
	}
}

void mysza3(int button, int state, int x, int y) {
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
		wcisnietylewy = false;
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		wcisnietylewy = true;
	if (state == GLUT_UP && button == GLUT_RIGHT_BUTTON)
		wcisnietyprawy = false;
	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON)
		wcisnietyprawy = true;
}

void kulko(int button, int dir, int x, int y) {
	if (dir > 0)
		kamera -= predkosc;
	else
		kamera += predkosc;
}

void idle(void) {
	glutPostRedisplay();
}

void wczytaj() {

	int x;
	GLfloat a, b, c, d;
	fstream wczytywacz, wczytywacz2;
	string nazwaobiektu;
	wczytywacz2.open("ustawienia/pliki.txt");
	if (!wczytywacz2.is_open()) {
		Logger::log(Logger::ERR + "brak pliku z plikami");
		exit(0);
	}

	while (!wczytywacz2.eof()) {
		wczytywacz2 >> nazwaobiektu;
		Logger::log(nazwaobiektu);
		obiekty[ileobiektow2++] = new obiekt(nazwaobiektu);
	}
	wczytywacz2.close();
	mapa *map = new mapa();
	wczytywacz.open("ustawienia/ustawienia.txt");
	if (!wczytywacz.is_open()) {
		Logger::log(Logger::ERR + "brak pliku z ustawieniami");
		exit(0);
	}

	while (!wczytywacz.eof()) {
		wczytywacz >> nazwaobiektu;
		if (nazwaobiektu == "o") {
			wczytywacz >> x;
			if (x >= ileobiektow2) {
				Logger::log(Logger::ERR + "nie ma tyle obiektow");
				exit(0);
			}
			obiekty_f[ileobiektow++] = new obiekt_final(obiekty[x]);

		}

		if (nazwaobiektu == "p") {
			wczytywacz >> a >> b >> c;
			d = mapa::oblicz_wysokosc(a, b, c);
			//xd << "p " << a << " " << d << " " << c << endl;
			obiekty_f[ileobiektow - 1]->dodaj_przesuniecie(a, d, c);
		}

		if (nazwaobiektu == "pc") {
			wczytywacz >> a >> b >> c;
			//xd << "pc " << a << " " << b << " " << c << endl;
			obiekty_f[ileobiektow - 1]->dodaj_przesuniecie(a, b, c);
		}

		if (nazwaobiektu == "s") {
			wczytywacz >> a >> b >> c;
			//xd << "s " << a << " " << b << " " << c << endl;
			obiekty_f[ileobiektow - 1]->dodaj_skale(a, b, c);
		}

		if (nazwaobiektu == "r") {
			wczytywacz >> a >> b >> c;
			//xd << "r " << a << " " << b << " " << c << endl;
			obiekty_f[ileobiektow - 1]->dodaj_obrot(a, b, c);
		}

		if (nazwaobiektu == "d") {
			wczytywacz >> x;
			//xd << "d " << x << endl;
			obiekty_f[ileobiektow - 1]->ociec = obiekty_f[ileobiektow - 1 + x];
		}

		if (nazwaobiektu == "a") {
			wczytywacz >> nazwaobiektu;
			//xd << "animacja " + nazwaobiektu << endl;
			obiekty_f[ileobiektow - 1]->anim = new animacja("modele/" + obiekty_f[ileobiektow - 1]->ob->nazwa,
					nazwaobiektu, obiekty_f[ileobiektow - 1]);
			obiekty_animowane[ileanimacji++] = obiekty_f[ileobiektow - 1];
		}
		if (nazwaobiektu == "v") {
			obiekty_f[ileobiektow - 1]->zawsze = true;
		}
	}
	obiekty_f[ileobiektow++] = new obiekt_final(obiekty[ileobiektow2 - 1]);
	obiekty_f[ileobiektow - 1]->zawsze = true;
	obiekty_f[ileobiektow - 1]->dodaj_skale(mapa::stosunekx, mapa::stosuneky, mapa::stosunekz);
	SDL_Quit();

	ostringstream stream;
	Logger::log(Logger::LINE);
	stream << "Utworzono " << ilee << " trojkatow";
	Logger::log(stream.str());
	stream.str("");
	stream << "Wczytanych obiektow: " << ileobiektow2 << ", wyswietlonych obiektow:\n\n" << ileobiektow;
	Logger::log(stream.str());

	int ilerez = 0;
	GLboolean *czyrezydentne = new GLboolean[iletekstur];
	glAreTexturesResident(iletekstur, tekstura::txtid, czyrezydentne);

	for (int i = 0; i < iletekstur; i++)
		if (czyrezydentne[i]) {
			ilerez++;
		}
	delete[] czyrezydentne;

	ostringstream steam;

	stream << "Tekstur: " << iletekstur << ", rezydentne: " << ilerez << endl;
	Logger::log(stream.str());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obiekt::numerkowybuforXD);
}

void zapisz() {
	fstream zapisywacz;
	zapisywacz.open("ustawienia/dupa.txt", ios::out);
	int j;
	for (int i = 0; i < ileobiektow; i++) {
		for (j = 0; j < ileobiektow2; j++)
			if (obiekty_f[i]->ob == obiekty[j])
				break;
		zapisywacz << endl << "o " << j << endl;
		if (obiekty_f[i]->anim) {
			if (obiekty_f[i]->anim->startpx || obiekty_f[i]->anim->startpy || obiekty_f[i]->anim->startpz)
				zapisywacz << "p " << obiekty_f[i]->anim->startpx << " " << obiekty_f[i]->anim->startpy << " "
						<< obiekty_f[i]->anim->startpz << endl;
			if (obiekty_f[i]->anim->startsx || obiekty_f[i]->anim->startsy || obiekty_f[i]->anim->startsz)
				zapisywacz << "s " << obiekty_f[i]->anim->startsx << " " << obiekty_f[i]->anim->startsy << " "
						<< obiekty_f[i]->anim->startsz << endl;
			if (obiekty_f[i]->anim->startrx || obiekty_f[i]->anim->startry || obiekty_f[i]->anim->startrz)
				zapisywacz << "r " << obiekty_f[i]->anim->startrx << " " << obiekty_f[i]->anim->startry << " "
						<< obiekty_f[i]->anim->startrz << endl;
			zapisywacz << "a" << obiekty_f[i]->anim->nazwa << endl;
		} else {
			if (obiekty_f[i]->px || obiekty_f[i]->py || obiekty_f[i]->pz)
				zapisywacz << "p " << obiekty_f[i]->px << " " << obiekty_f[i]->py << " " << obiekty_f[i]->pz << endl;
			if (obiekty_f[i]->sx || obiekty_f[i]->sy || obiekty_f[i]->sz)
				zapisywacz << "s " << obiekty_f[i]->sx << " " << obiekty_f[i]->sy << " " << obiekty_f[i]->sz << endl;
			if (obiekty_f[i]->rx || obiekty_f[i]->ry || obiekty_f[i]->rz)
				zapisywacz << "r " << obiekty_f[i]->rx << " " << obiekty_f[i]->ry << " " << obiekty_f[i]->rz << endl;
		}

	}
}

long long unsigned sprawdz_rozmiar(string nazwa) {
	long long unsigned kutas;
	ifstream sprawdzacz;
	sprawdzacz.open(nazwa.c_str(), fstream::binary);
	if (!sprawdzacz.is_open())
		exit(0);
	sprawdzacz.seekg(0, ios::end);
	kutas = sprawdzacz.tellg();
	sprawdzacz.close();
	return kutas;

}

void __cdecl animuj(void *kutas) {
	while (1) {
		if (ktorykutas == -1)
			for (int i = 0; i < ileanimacji; i++)
				obiekty_animowane[i]->anim->animuj(obiekty_animowane[i]);
		Sleep(15);
	}
}

void __cdecl informuj(void *kutas) {
	while (1) {
		stringstream x1, y1, z1, fps, speed, amb, diff, spec, pos, poss, ob, ob2, ileob, ileob2, licznikob;
		x1 << px;
		y1 << py;
		z1 << pz;
		amb << light_ambient[0] << " " << light_ambient[1] << " " << light_ambient[2] << " " << light_ambient[3];
		diff << light_diffuse[0] << " " << light_diffuse[1] << " " << light_diffuse[2] << " " << light_diffuse[3];
		spec << light_specular[0] << " " << light_specular[1] << " " << light_specular[2] << " " << light_specular[3];
		pos << light_position[0] << " " << light_position[1] << " " << light_position[2] << " " << light_position[3];
		poss << "Swiatlo: " << ktoreswiatlo << " Pozycja: " << ktorapos;
		ob << ktorykutas;
		ob2 << ktorykutas2;
		ileob << ileobiektow;
		ileob2 << obiekt_final::granica2;
		licznikob << obiekty[ktorykutas2]->licznik;
		speed << predkosc;
		if (clock() - licznik >= CLOCKS_PER_SEC) {
			fps << ramki;
			info.fps = fps.str();
			ramki = 0;
		}

		info.x1 = x1.str();
		info.y1 = y1.str();
		info.z1 = z1.str();
		info.speed = speed.str();
		info.amb = amb.str();
		info.diff = diff.str();
		info.spec = spec.str();
		info.pos = pos.str();
		info.poss = poss.str();
		info.ob = ob.str();
		info.ob2 = ob2.str();
		info.ileob = ileob.str();
		info.ileob2 = ileob2.str();
		info.licznikob = licznikob.str();
		Sleep(100);
	}
}

void __cdecl sortuj(void *dupa) {
	while (1) {
		obiekt_final **obiekty_posortowane2 = new obiekt_final*[ileobiektow];
		int granica1 = 0;
		int granica2 = 0;
		obiekt_final **obiekty_pom;
		obiekty_pom = new obiekt_final*[ileobiektow];
		int licznik = 0;
		bool cycki;
		for (int i = 0; i < ileobiektow; i++) {
			cycki = false;
			for (int j = 0; j < obiekty_f[i]->ob->ileobiektow; j++)
				if (obiekty_f[i]->ob->podobiekty[j]->mtl->kat[3] < 1
						|| (obiekty_f[i]->ob->podobiekty[j]->mtl->tkdt != -1
								&& tekstury[obiekty_f[i]->ob->podobiekty[j]->mtl->tkdt]->przezroczysta)) {
					cycki = true;
					break;
				}
			if (ciach->nalezy(i)) {
				if (cycki)
					obiekty_pom[licznik++] = obiekty_f[i];
				else
					obiekty_posortowane2[granica1++] = obiekty_f[i];
			}
		}
		granica2 = granica1;

		if (licznik) {
			float *tab = new float[licznik];
			for (int i = 0; i < licznik; i++) {
				tab[i] = pow(px - obiekty_pom[i]->px, 2) + pow(py - obiekty_pom[i]->py, 2)
						+ pow(pz - obiekty_pom[i]->pz, 2);
				if (tab[i] < 0)
					tab[i] *= -1;
			}
			int a;

			while (licznik > 0) {
				a = 0;
				for (int i = 0; i < licznik; i++)
					if (tab[a] < tab[i])
						a = i;
				obiekty_posortowane2[granica2++] = obiekty_pom[a];
				if (a != licznik - 1) {
					obiekty_pom[a] = obiekty_pom[licznik - 1];
					tab[a] = tab[licznik - 1];
				}
				licznik--;

			}
			delete[] tab;
			delete[] obiekty_pom;
		}
		obiekt_final::granica1 = granica1;
		obiekt_final::granica2 = granica2;
		obiekt_final **lol = obiekty_posortowane;
		obiekty_posortowane = obiekty_posortowane2;
		delete[] lol;
		Sleep(100);
	}
}

string utnij(string kutas) {
	string dupa = "";
	int a = -1;
	for (int i = kutas.length() - 1; i >= 0; i--)
		if (kutas[i] == '/' || kutas[i] == 92) {
			a = i;
			break;
		}
	if (a == -1)
		return kutas;
	for (int i = 0; i < a; i++)
		dupa += kutas[i];
	return dupa;
}

string otworz(string nazwa, string koniec) {
	string nazwa2 = "";
	string nazwa3 = "";
	int tak = 0;
	if (nazwa[1] == ':') {
		int cycki = 0;
		for (unsigned i = 0; i < nazwa.length(); i++)
			if (nazwa[i] == '/' || nazwa[i] == '\\')
				cycki = i;
		string kutas = "";
		for (unsigned i = cycki; i < nazwa.length(); i++)
			kutas += nazwa[i];
		return nazwa + kutas + koniec;
	}
	for (unsigned i = 0; i < nazwa.length(); i++) {
		nazwa3 += nazwa[i];
		if (nazwa[i] == '/' || nazwa[i] == 92)
			tak = i;
	}

	if (!tak)
		nazwa2 = "modele/" + nazwa + "/" + nazwa + koniec;
	else {
		string nazwa5 = "modele/" + nazwa3;
		string nazwa4 = "";
		for (unsigned i = tak + 1; i < nazwa.length(); i++)
			nazwa4 += nazwa[i];
		nazwa2 = nazwa5 + "/" + nazwa4 + koniec;

	}
	return nazwa2;;
}

void DrawString(double x, double y, double z, string string) {
	glRasterPos3d(x, y, z);
	for (unsigned i = 0; i < string.length(); i++)
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, (int) string[i]);
}

void sprawdz_rozszerzenie(string roz) {
	if (!glewIsSupported(roz.c_str()))
		Logger::log(Logger::ERR + "nieobslugiwane roszerzenie " + roz);
}

int main(int argc, char* args[]) {
	Logger::log("Tworzenie okna...");
	glutInit(&argc, args);
	glutInitWindowSize(szer, wys);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Ka³nter Strajk");
	Logger::log("Inicjalizacja GLEW...");
	glewExperimental = GL_TRUE;
	ostringstream stream;
	GLenum err = glewInit();
	if (err) {
		stream << glewGetErrorString(err);
		Logger::log(stream.str());
		stream.str("");
	} else {
		Logger::log("GLEW OK");
	}
	if (!GLEW_VERSION_3_0)
		Logger::log(Logger::ERR + "masz wersje " + (char *) glGetString( GL_VERSION) + " OpenGL zamiast 4.2.0 XD");
	else {
		stream << "Wersja OpenGL: " << (char*) glGetString( GL_VERSION) << ", OK\n";
		Logger::log(stream.str());
		stream.str("");
	}
	sprawdz_rozszerzenie("GL_ARB_framebuffer_object");
	sprawdz_rozszerzenie("GL_EXT_texture_compression_s3tc");
	sprawdz_rozszerzenie("GL_ARB_vertex_buffer_object");

	mysz_pozycja = new tagPOINT;
	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutKeyboardFunc(klawiaturka);
	glutPassiveMotionFunc(mysza);
	glutMotionFunc(mysza2);
	glutMouseFunc(mysza3);
	glutMouseWheelFunc(kulko);
	glutIdleFunc(idle);
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1
			| IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP) == -1) {
		Logger::log(Logger::ERR + "inicjalizacja SDL");
		exit(0);
	}
	Logger::log(Logger::LINE);
	Logger::log("SDL OK");

	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	wczytaj();
	glEnableClientState( GL_VERTEX_ARRAY);
	glEnableClientState( GL_NORMAL_ARRAY);
	ciach = new obcinanie();
	obiekty_posortowane = new obiekt_final*[1000];
	hThread = (HANDLE) _beginthread(animuj, 0, NULL);
	hThread2 = (HANDLE) _beginthread(informuj, 0, NULL);
	hThread3 = (HANDLE) _beginthread(sortuj, 0, NULL);

	glutMainLoop();
	return 0;
}
//TODO zapis
/*
 x86/zlib1.dll
 x86/freeglut.dll
 x86/glew32.dll
 x86/libjpeg-9.dll
 x86/libpng16-16.dll
 x86/libtiff-5.dll
 x86/libwebp-4.dll
 x86/SDL2.dll
 x86/SDL2_image.dll

 bin/zlib1.dll
 bin/freeglut.dll
 bin/glew32.dll
 bin/libjpeg-9.dll
 bin/libpng16-16.dll
 bin/libtiff-5.dll
 bin/libwebp-4.dll
 bin/SDL2.dll
 bin/SDL2_image.dll

 */
