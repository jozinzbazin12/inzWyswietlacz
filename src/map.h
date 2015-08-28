/*
 * map.h
 *
 *  Created on: 10 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_MAP_H_
#define SRC_MAP_H_
#include "md5.h"

class Map {
private:
	static int **heights;
	long long unsigned mapSize;
	const char* mapFile = "modele/0/0.obj";
	MD5 md5;

	void addVector(float* dest, float* v, int size = 3) {
		for (int i = 0; i < size; i++) {
			dest[i] += v[i];
		}
	}

	void normalize(float *t1) {
		float d = sqrt(t1[0] * t1[0] + t1[1] * t1[1] + t1[2] * t1[2]);
		if (d != 0) {
			t1[0] /= d;
			t1[1] /= d;
			t1[2] /= d;
		}
//		t1[0] = t1[0] < 0 ? t1[0] * -1 : t1[0];
//		t1[1] = t1[1] < 0 ? t1[1] * -1 : t1[1];
//		t1[2] = t1[2] < 0 ? t1[2] * -1 : t1[2];
	}

	void copyToModels(string mtl, string texture, string destTexture) {
		ifstream src(mtl.c_str(), ios::binary);
		ofstream dst("modele/0/0.mtl", ios::binary);
		ifstream src2(texture.c_str(), ios::binary);
		ofstream dst2("modele/0/" + destTexture, ios::binary);
		dst << src.rdbuf();
		dst2 << src2.rdbuf();
		src.close();
		dst.close();
		src2.close();
		dst2.close();
	}
//todo
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
//todo
	bool tryLoadLastMap(string name) {
		fstream sprawdzacz, sprawdzacz2;
		string map;
		sprawdzacz.open(mapFile);
		if (!sprawdzacz.is_open()) {
			Logger::log("Nie ma mapy, probuje utworzyc");
			return false;
		} else {
			sprawdzacz2.open("mapy/ostatnia.txt");
			if (sprawdzacz2.is_open()) {
				string hash;
				sprawdzacz2 >> hash;
				sprawdzacz2 >> map;
				sprawdzacz2 >> mapX;
				sprawdzacz2 >> mapZ;

				string mapHash = md5.digestFile(mapFile);
				if (map == name && hash == mapHash) {
					Logger::log("Jest zrobiona mapa, wczytuje...");
					loadHeights();
					stosunekx = (float) wymx / (float) mapX;
					stosunekz = (float) wymz / (float) mapZ;
					mapObject = new Object("0", true);
					sprawdzacz.close();
					sprawdzacz2.close();
					return true;
				}
			}
		}
		sprawdzacz.close();
		sprawdzacz2.close();
		return false;
	}

	float* makeNormal(float t1[3], float t2[3], float t3[3]) {
		float temp1[3], temp2[3];
		float *vec = new float[3];
		for (int k = 0; k < 3; k++) {
			temp1[k] = t2[k] - t1[k];
			temp2[k] = t3[k] - t1[k];
		}

		vec[0] = temp1[1] * temp2[2] - temp1[2] * temp2[1];
		vec[1] = temp1[2] * temp2[0] - temp1[0] * temp2[2];
		vec[2] = temp1[0] * temp2[1] - temp1[1] * temp2[0];
		normalize(vec);
		return vec;
	}

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
							if (iRC) {
								return iRC;
							}
						} else {
							bSubdirectory = true;
						}
					} else {
						// Set file attributes
						if (::SetFileAttributes(strFilePath.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE) {
							return ::GetLastError();
						}

						// Delete file
						if (::DeleteFile(strFilePath.c_str()) == FALSE) {
							return ::GetLastError();
						}
					}
				}
			} while (::FindNextFile(hFile, &FileInformation) == TRUE);

			// Close handle
			::FindClose(hFile);

			DWORD dwError = ::GetLastError();
			if (dwError != ERROR_NO_MORE_FILES) {
				return dwError;
			} else {
				if (!bSubdirectory) {
					// Set directory attributes
					if (::SetFileAttributes(refcstrRootDirectory.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE) {
						return ::GetLastError();
					}
				}
			}
		}
		return 0;
	}

public:
	static int mapX, mapZ;
	long double stosunekx;
	long double stosuneky;
	long double stosunekz;
	int wymx, wymz;
	Object* mapObject;

	void setMapSize(double lengthX, double lengthZ, double scaleY) {
		wymx = lengthX;
		wymz = lengthZ;
		stosuneky = scaleY;
	}

	void createMap(string mapName, string textureName, string mtlName) {
		if (tryLoadLastMap(mapName)) {
			return;
		}
		deleteDirectory("modele/0/");
		copyToModels(mtlName, textureName, "tex" + getFileExtension(textureName));
		Logger::log("Nie ma mapy, probuje utworzyc\n");
		SDL_Surface *txt = IMG_Load(mapName.c_str());
		if (txt == NULL) {
			Logger::log(Logger::ERR + "chujowy obrazek");
			exit(0);
		}
		stosunekx = (float) wymx / (float) txt->w;
		stosunekz = (float) wymz / (float) txt->h;
		mapX = txt->w;
		mapZ = txt->h;
		unsigned pixel, r, g, b;
		long double height;
		fstream zapisywacz, zapisywacz2;
		zapisywacz.open("modele/0/0.obj", ios::out);
		zapisywacz2.open("mapy/wysokosci.txt", ios::out);
		zapisywacz << "mtllib 0.mtl\no kutas" << endl;
		int bpp = txt->format->BytesPerPixel;

		heights = new int*[txt->h];
		for (int i = 0; i < txt->h; i++) {
			heights[i] = new int[txt->w];
		}
		//heights
		for (int i = 0; i < txt->h; i++) {
			for (int j = 0; j < txt->w; j++) {
				pixel = ((Uint32*) txt->pixels)[i * (txt->pitch / sizeof(Uint32)) + j * bpp / 4];
				r = pixel & 0x000000FF;
				g = pixel & 0x0000FF00;
				b = pixel & 0x00FF0000;
				g >>= 8;
				b >>= 16;
				height = ((r + b + g) / 3) - (double) 128;
				zapisywacz << "v " << txt->h / 2 - i << " " << height << " " << j - txt->w / 2 << endl; //moze sie zjebac!
				zapisywacz2 << height << " ";
				heights[i][j] = height;
			}
		}
		zapisywacz2.close();

		//face normals
		float **vectors = new float*[(txt->w - 1) * (txt->h - 1) * 2];
		int v = 0;
		float t1[3], t2[3], t3[3], t4[3];
		for (int i = 0; i < txt->h - 1; i++) {
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

				t4[0] = txt->h / 2 - i - 1;
				t4[1] = heights[i + 1][j + 1]; //i+1,j+1
				t4[2] = j + 1 - txt->w / 2;

				vectors[v++] = makeNormal(t1, t3, t2);
				vectors[v++] = makeNormal(t4, t2, t1);
			}
		}
		zapisywacz << "vt 0 0" << endl;
		zapisywacz << "vt 0 2" << endl;
		zapisywacz << "vt 2 0" << endl;
		zapisywacz << "vt 2 2" << endl;
		zapisywacz << "usemtl cipa" << endl; //TODO
		zapisywacz << "s 1" << endl;

		//vertex normals
		int vertex, vetex2;
		float normals[3];
		for (int i = 0; i < txt->h; i++) {
			for (int j = 0; j < txt->w; j++) {
				vertex = i * (txt->w - 1) * 2 + j * 2;
				vetex2 = (i - 1) * (txt->w - 1) * 2 + j * 2;
				normals[0] = 0;
				normals[1] = 0;
				normals[2] = 0;
				if (j != txt->w - 1 && i != txt->h - 1) {
					addVector(normals, vectors[vertex]);
				}

				if (j != 0 && i != txt->h - 1) {
					addVector(normals, vectors[vertex - 2]);
					addVector(normals, vectors[vertex - 1]);
				}

				if (j != 0 && i != 0) {
					addVector(normals, vectors[vetex2 - 1]);
				}

				if (j != txt->w - 1 && i != 0) {
					addVector(normals, vectors[vetex2]);
					addVector(normals, vectors[vetex2 + 1]);
				}
				normalize(normals);
				normals[0] = vectors[vertex > 1 && vertex < v ? vertex - 1 : 0][0];
				normals[1] = vectors[vertex > 1 && vertex < v ? vertex - 1 : 0][1];
				normals[2] = vectors[vertex > 1 && vertex < v ? vertex - 1 : 0][2];

				zapisywacz << "vn " << normals[0] << " " << normals[1] << " " << normals[2] << endl;
			}
		}
		//faces
		int normal, normal2;
		for (int i = 0; i < txt->h - 1; i++) {
			for (int j = 1; j < txt->w; j++) {
				vertex = i * txt->w + j;
				vetex2 = (i + 1) * txt->w + j;
				normal = vertex;
				normal2 = vetex2;
				zapisywacz << "f " << vertex << "/1/" << normal << " " << vertex + 1 << "/2/" << normal + 1 << " "
						<< vetex2 << "/3/" << normal2 << endl;
				zapisywacz << "f " << vertex + 1 << "/2/" << normal + 1 << " " << vetex2 << "/3/" << normal2 << " "
						<< vetex2 + 1 << "/4/" << normal2 + 1 << endl;
			}
		}
		Logger::log("Utworzono mape");
		fstream sprawdzacz2;
		sprawdzacz2.open("mapy/ostatnia.txt", ios::out);
		sprawdzacz2 << md5.digestFile(mapFile) << endl;
		sprawdzacz2 << mapName << endl;
		sprawdzacz2 << txt->w << endl;
		sprawdzacz2 << txt->h;
		sprawdzacz2.close();
		SDL_FreeSurface(txt);
		for (int i = 0; i < v; i++) {
			delete[] vectors[i];
		}
		delete[] vectors;
		mapObject = new Object("0", true);
		zapisywacz.close();
	}

	float calculateHeight(float x, float y, float z) {
		int indexX, indexZ;
		long double valueX, valueZ;
		float suma = 0, suma2 = 0;
		valueX = (long double) ((x) / stosunekx) + (long double) (mapX / 2);	//-1
		valueZ = (long double) ((z) / stosunekz) + (long double) (mapZ / 2);		//0
		indexX = (int) valueX;
		indexZ = (int) valueZ;
		if (indexX <= 0 || indexX >= mapX || indexZ <= 0 || indexZ >= mapZ) {
			return 0;
		}
		if (valueX == indexX && valueZ == indexZ) {
			return heights[mapX - indexX][indexZ] * stosuneky + y;
		}

		if (valueX == indexX) {
			suma = (heights[mapX - indexX][indexZ] - heights[mapX - indexX][indexZ + 1]) * (valueZ - indexZ);
			return (heights[mapX - indexX][indexZ] - suma) * stosuneky + y;
		}

		if (valueZ == indexZ) {
			suma = (heights[mapX - indexX][indexZ] - heights[mapX - indexX - 1][indexZ]) * (valueX - indexX);
			return (heights[mapX - indexX][indexZ] - suma) * stosuneky + y;
		}

		if (valueZ - indexZ < 0.5 && valueX - indexX < 0.5) {
			if (indexX > 0) {
				suma = (heights[mapX - indexX][indexZ] - heights[mapX - indexX][indexZ + 1]) * (valueZ - indexZ);
			}
			if (indexX + 1 > 0) {
				suma2 = (heights[mapX - indexX][indexZ] - heights[mapX - indexX - 1][indexZ]) * (valueX - indexX);
			}
			return (heights[mapX - indexX][indexZ] - suma - suma2) * stosuneky + y;
		}

		// todo?
		else {
			if (indexX + 1 > 0) {
				if (indexZ + 1 < mapZ) {
					suma = (heights[mapX - indexX - 1][indexZ + 1] - heights[mapX - indexX - 1][indexZ])
							* (1 - valueX + indexX);
				}
				if (indexZ + 1 < mapZ) {
					suma2 = (heights[mapX - indexX - 1][indexZ + 1] - heights[mapX - indexX][indexZ + 1])
							* (1 - valueZ + indexZ);
				}
				return (heights[mapX - indexX - 1][indexZ + 1] - suma - suma2) * stosuneky + y;
			}
			return -100;
		}

		return 1000;
		//return 0;
	}

};
int** Map::heights;
int Map::mapX = 0;
int Map::mapZ = 0;

#endif /* SRC_MAP_H_ */
