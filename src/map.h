/*
 * map.h
 *
 *  Created on: 10 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_MAP_H_
#define SRC_MAP_H_
//todo
class mapa {
public:
	static int **heights;
	static int mapX, mapZ;
	static long double stosunekx;
	static long double stosuneky;
	static long double stosunekz;
	int wymx, wymz;
	long long unsigned rozmiarmapy;

	int deleteDirectory(const string &refcstrRootDirectory, bool bDeleteSubdirectories = true) {
		bool bSubdirectory = false;
		HANDLE hFile;
		string strFilePath;
		string strPattern;
		WIN32_FIND_DATA FileInformation;

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
							int iRC = deleteDirectory(strFilePath, bDeleteSubdirectories);
							if (iRC)
								return iRC;
						} else
							bSubdirectory = true;
					} else {
						// Set file attributes
						if (::SetFileAttributes(strFilePath.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
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
					if (::SetFileAttributes(refcstrRootDirectory.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();
				}
			}
		}
		return 0;
	}

	static float calculateHeight(float a2, float b2, float c2) {
		int aa, cc;
		long double a, c;
		float suma = 0, suma2 = 0;
		a = (long double) ((a2) / stosunekx) + (long double) (mapX / 2);	//-1
		c = (long double) ((c2) / stosunekz) + (long double) (mapZ / 2);		//0
		aa = (int) a;
		cc = (int) c;
		if (aa < 1 || aa >= mapX || cc < 0 || cc >= mapZ)
			return 0;
		if (a == aa && c == cc)
			return heights[mapX - aa][cc] * stosuneky + b2;

		if (a == aa) {
			suma = (heights[mapX - aa][cc] - heights[mapX - aa][cc + 1]) * (c - cc);
			return (heights[mapX - aa][cc] - suma) * stosuneky + b2;
		}

		if (c == cc) {
			suma = (heights[mapX - aa][cc] - heights[mapX - aa + 1][cc]) * (a - aa);
			return (heights[mapX - aa][cc] - suma) * stosuneky + b2;
		}

		if (c - cc < 0.5 && a - aa < 0.5) {
			suma = (heights[mapX - aa][cc] - heights[mapX - aa][cc + 1]) * (c - cc);
			suma2 = (heights[mapX - aa][cc] - heights[mapX - aa + 1][cc]) * (a - aa);
			return (heights[mapX - aa][cc] - suma - suma2) * stosuneky + b2;
		}

		else {
			suma = (heights[mapX - aa + 1][cc + 1] - heights[mapX - aa + 1][cc]) * (1 - a + aa);
			suma2 = (heights[mapX - aa + 1][cc + 1] - heights[mapX - aa][cc + 1]) * (1 - c + cc);
			return (heights[mapX - aa + 1][cc + 1] - suma - suma2) * stosuneky + b2;
		}

		//return 1000;
		return 0;
	}
//todo
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

	void normalize(float *t1) {
		float d = sqrt(t1[0] * t1[0] + t1[1] * t1[1] + t1[2] * t1[2]);
		if (d != 0) {
			t1[0] /= d;
			t1[1] /= d;
			t1[2] /= d;
		}
	}

	void copyToModels(string mtl, string texture, string dupaa) {
		ifstream src(mtl.c_str(), ios::binary);
		ofstream dst("modele/0/0.mtl", ios::binary);
		ifstream src2(texture.c_str(), ios::binary);
		ofstream dst2(dupaa.c_str(), ios::binary);
		dst << src.rdbuf();
		dst2 << src2.rdbuf();
		src.close();
		dst.close();
		src2.close();
		dst2.close();
	}

	void loadHeights() {
		heights = new int*[mapX];
		for (int i = 0; i < mapX; i++)
			heights[i] = new int[mapZ];
		fstream file;
		file.open("mapy/wysokosci.txt");
		if (!file.is_open()) {
			Logger::log(Logger::ERR + "nie ma wysokosci");
			exit(0);
		}

		for (int i = 0; i < mapX; i++)
			for (int j = 0; j < mapZ; j++)
				file >> heights[i][j];
		file.close();
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
				sprawdzacz2 >> mapX;
				sprawdzacz2 >> mapZ;
				rozmiarmapy = sprawdz_rozmiar(nazwa);
				if (a == rozmiarmapy && nazwa == b) {
					Logger::log("Jest zrobiona mapa, wczytuje...");
					tag = true;
					loadHeights();
					stosunekx = (float) wymx / (float) mapX;
					stosunekz = (float) wymz / (float) mapZ;
					stosuneky = 2;
					Object::objects.push_back(new Object("0", true));
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
		deleteDirectory("modele/0/");
		copyToModels(mtl, tekstura, dupaa);
		Logger::log("Nie ma mapy, probuje utworzyc\n");
		SDL_Surface *txt = IMG_Load(nazwa.c_str());
		if (txt == NULL) {
			Logger::log(Logger::ERR + "chujowy obrazek");
			exit(0);
		}
		stosunekx = (float) wymx / (float) txt->w;
		stosunekz = (float) wymz / (float) txt->h;
		stosuneky = 2;
		mapX = txt->w;
		mapZ = txt->h;
		unsigned pixel, r, g, b;
		long double wys2;
		fstream zapisywacz, zapisywacz2;
		zapisywacz.open("modele/0/0.obj", ios::out);
		zapisywacz2.open("mapy/wysokosci.txt", ios::out);
		zapisywacz << "mtllib 0.mtl\no kutas" << endl;
		int bpp = txt->format->BytesPerPixel;

		heights = new int*[txt->h];
		for (int i = 0; i < txt->h; i++)
			heights[i] = new int[txt->w];
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
				heights[i][j] = wys2 - 128;
			}
		zapisywacz2.close();
		float t1[3], t2[3], t3[3];
		for (int i = 0; i < txt->h - 1; i++)
			for (int j = 0; j < txt->w - 1; j++) {
				t1[0] = txt->h / 2 - i;
				t1[1] = heights[i][j];  //i,j
				t1[2] = j - txt->w / 2;

				t2[0] = txt->h / 2 - i;
				t2[1] = heights[i][j + 1]; //i,j+1
				t2[2] = j + 1 - txt->w / 2;

				t3[0] = txt->h / 2 - i - 1;
				t3[1] = heights[i + 1][j]; //i+1,j
				t3[2] = j - txt->w / 2;

				vec[v++] = zrup_normalny(t1, t3, t2);

				t1[0] = txt->h / 2 - i - 1;
				t1[1] = heights[i + 1][j];  //i+1,j
				t1[2] = j - txt->w / 2;

				t2[0] = txt->h / 2 - i;
				t2[1] = heights[i][j + 1]; //i,j+1
				t2[2] = j + 1 - txt->w / 2;

				t3[0] = txt->h / 2 - i - 1;
				t3[1] = heights[i + 1][j + 1]; //i+1,j+1
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
				normalize(lol);
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
		Object::objects.push_back(new Object("0", true));
		zapisywacz.close();
	}

	mapa() {
		sprawdz_co();
	}
};
int** mapa::heights;
int mapa::mapX = 0;
int mapa::mapZ = 0;
long double mapa::stosunekx;
long double mapa::stosuneky;
long double mapa::stosunekz;

#endif /* SRC_MAP_H_ */
