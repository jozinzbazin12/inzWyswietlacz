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
	static const int ERROR_HEIGHT = 1000;
	const string MAP_NAME = "models/0/0.obj";
	long long unsigned mapSize;
	const char* mapFile = "models/0/0.obj";
	MD5 md5;

	void subtractVector(float* dest, float* v, int size = 3) {
		for (int i = 0; i < size; i++) {
			dest[i] -= v[i];
		}
	}

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
	}

	void copyMtlToModels() {
		string destTextureName = "tex" + getFileExtension(texturePath);
		ofstream mtlDst("models/0/0.mtl", ios::binary);
		ifstream texSrc(texturePath.c_str(), ios::binary);
		ofstream texDst("models/0/" + destTextureName, ios::binary);
		ostringstream ss;
		ss << "newmtl main" << endl;
		ss << "Ka" << " " << mtl->ka[0] << " " << mtl->ka[1] << " " << mtl->ka[2] << endl;
		ss << "Kd" << " " << mtl->kd[0] << " " << mtl->kd[1] << " " << mtl->kd[2] << endl;
		ss << "Ks" << " " << mtl->ks[0] << " " << mtl->ks[1] << " " << mtl->ks[2] << endl;
		ss << "Ns" << " " << mtl->ns << endl;
		ss << "d" << " " << mtl->d << endl;
		ss << "map_Kd" << " " << destTextureName << endl;
		mtlDst << ss.str();
		texDst << texSrc.rdbuf();
		mtlDst.close();
		texSrc.close();
		texDst.close();
		delete mtl;
	}

	void loadHeights(SDL_Surface* txt, fstream* dest = NULL) {
		int bpp = txt->format->BytesPerPixel;
		unsigned pixel, r, g, b;

		heights = new int*[mapX];
		for (int i = 0; i < mapX; i++) {
			heights[i] = new int[mapZ];
		}

		long double height;
		long double max = -0xffff;
		for (int i = 0; i < mapX; i++) {
			for (int j = 0; j < mapZ; j++) {
				pixel = ((Uint32*) txt->pixels)[i * (txt->pitch / sizeof(Uint32)) + j * bpp / 4];
				r = pixel & 0x000000FF;
				g = pixel & 0x0000FF00;
				b = pixel & 0x00FF0000;
				g >>= 8;
				b >>= 16;
				height = ((r + b + g) / 3.0) - (double) 128;
				if (dest) {
					*dest << "v " << mapZ / 2.0 - j << " " << height << " " << mapX / 2.0 - i << endl; //moze sie zjebac!
				}
				if (max < height) {
					max = height;
				}
				heights[j][mapX - i - 1] = height;
			}
		}
		yRate = (float) wymy / max;
		yRate = yRate < 0 ? yRate * -1 : yRate;
	}

	void loadHeights(string map) {
		SDL_Surface* txt = IMG_Load(map.c_str());
		loadHeights(txt, NULL);
		SDL_FreeSurface(txt);
	}

//todo
	bool tryLoadLastMap(string name) {
		fstream file, lastSettingsFile;
		string map;
		file.open(mapFile);
		if (!file.is_open()) {
			Logger::log("No map found, creating...");
			return false;
		} else {
			lastSettingsFile.open("maps/last.txt");
			if (lastSettingsFile.is_open()) {
				string hash;
				string hash2;
				double texScale;
				lastSettingsFile >> hash;
				lastSettingsFile >> hash2;
				lastSettingsFile >> map;
				lastSettingsFile >> texScale;
				lastSettingsFile >> mapX;
				lastSettingsFile >> mapZ;

				string mapHash = md5.digestFile(mapFile);
				string mapImageHash = md5.digestFile(name.c_str());
				if (map == name && hash == mapHash && hash2 == mapImageHash && texScale == scale) {
					Logger::log("Loading last map...");
					loadHeights(map);
					calculateRate();
					mapObject = new Object(MAP_NAME, true);
					file.close();
					lastSettingsFile.close();
					return true;
				}
			}
		}
		file.close();
		lastSettingsFile.close();
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

	void calculateRate() {
		xRate = xLength / (mapX - 1);
		zRate = zLength / (mapZ - 1);
	}

public:
	static int **heights;
	static int mapX, mapZ;
	long double xRate;
	long double yRate = 0;
	long double zRate = 0;
	double xLength = -1;
	double wymy;
	double zLength = -1;
	double scale = 1;
	string texturePath = "maps/textures/tex.png";
	Object* mapObject;
	MapMaterial* mtl;

	void setMapSize(double lengthX, double lengthY, double lengthZ) {
		xLength = lengthX;
		wymy = lengthY;
		zLength = lengthZ;
	}

	void createMap(string mapName) {
		if (tryLoadLastMap(mapName)) {
			copyMtlToModels();
			return;
		}
		deleteDirectory("models/0/");
		copyMtlToModels();
		Logger::log("No map found, creating\n");
		SDL_Surface* txt = IMG_Load(mapName.c_str());
		if (txt == NULL) {
			Logger::log(Logger::ERR + "creating map failed");
			exit(0);
		}
		mapX = txt->h;
		mapZ = txt->w;
		calculateRate();

		fstream destObject;
		destObject.open("models/0/0.obj", ios::out);
		destObject << "mtllib 0.mtl\no map" << endl;

		loadHeights(txt, &destObject);

		//face normals
		float **vectors = new float*[(mapZ - 1) * (mapX - 1) * 2];
		int v = 0;
		float t1[3], t2[3], t3[3], t4[3];
		for (int i = 0; i < mapX - 1; i++) {
			for (int j = 0; j < mapZ - 1; j++) {
				double x = mapX / 2.0 - i;
				double z = mapZ / 2.0 - j;
				double x2 = mapX / 2.0 - i - 1;
				double z2 = mapZ / 2.0 - j - 1;
				int hx = j;
				int hz = mapX - i;
				int hx2 = j + 1;
				int hz2 = mapX - i - 1;

				t1[0] = x;
				t1[1] = heights[hx][hz];  //i,j
				t1[2] = z;

				t2[0] = x;
				t2[1] = heights[hx][hz2]; //i,j+1
				t2[2] = z2;

				t3[0] = x2;
				t3[1] = heights[hx2][hz]; //i+1,j
				t3[2] = z;

				t4[0] = x2;
				t4[1] = heights[hx2][hz2]; //i+1,j+1
				t4[2] = z2;

				vectors[v++] = makeNormal(t1, t2, t3);
				vectors[v++] = makeNormal(t4, t2, t3);
			}
		}
		ostringstream ss;
		const string VT = "vt ";
		ss << VT << 0 << " " << 0 << endl;
		ss << VT << 0 << " " << scale << endl;
		ss << VT << scale << " " << 0 << endl;
		ss << VT << scale << " " << scale << endl;
		destObject << ss.str();
		destObject << "usemtl main" << endl; //TODO
		destObject << "s 1" << endl;

		//vertex normals
		int vertex, vetex2;
		float normals[3];
		for (int i = 0; i < mapX; i++) {
			for (int j = 0; j < mapZ; j++) {
				vertex = i * (mapZ - 1) * 2 + j * 2;
				vetex2 = (i - 1) * (mapZ - 1) * 2 + j * 2;
				normals[0] = 0;
				normals[1] = 0;
				normals[2] = 0;
				if (j != mapZ - 1 && i != mapX - 1) {
					addVector(normals, vectors[vertex]);
				}

				if (j != 0 && i != mapX - 1) {
					addVector(normals, vectors[vertex - 2]);
					subtractVector(normals, vectors[vertex - 1]);
				}

				if (j != 0 && i != 0) {
					subtractVector(normals, vectors[vetex2 - 1]);
				}

				if (j != mapZ - 1 && i != 0) {
					addVector(normals, vectors[vetex2]);
					subtractVector(normals, vectors[vetex2 + 1]);
				}
				normalize(normals);

				destObject << "vn " << normals[0] << " " << normals[1] << " " << normals[2] << endl;
			}
		}
		//faces
		int normal, normal2;
		for (int i = 0; i < mapX - 2; i++) {
			for (int j = 1; j < mapZ; j++) {
				vertex = i * mapZ + j;
				vetex2 = (i + 1) * mapZ + j;
				normal = vertex;
				normal2 = vetex2;
				destObject << "f " << vertex << "/1/" << normal << " " << vertex + 1 << "/2/" << normal + 1 << " " << vetex2 << "/3/"
						<< normal2 << endl;
				destObject << "f " << vertex + 1 << "/2/" << normal + 1 << " " << vetex2 << "/3/" << normal2 << " " << vetex2 + 1 << "/4/"
						<< normal2 + 1 << endl;
			}
		}
		Logger::log("Utworzono mape");
		fstream mapInfo;
		mapInfo.open("maps/last.txt", ios::out);
		mapInfo << md5.digestFile(mapFile) << endl;
		mapInfo << md5.digestFile(mapName.c_str()) << endl;
		mapInfo << mapName << endl;
		mapInfo << scale << endl;
		mapInfo << mapX << endl;
		mapInfo << mapZ;
		mapInfo.close();
		SDL_FreeSurface(txt);
		for (int i = 0; i < v; i++) {
			delete[] vectors[i];
		}
		delete[] vectors;
		mapObject = new Object(MAP_NAME, true);
		destObject.close();
	}

	float calculateHeight(float x, float y, float z) {
		int indexX, indexZ;
		long double valueX, valueZ;
		double actual;
		float height1 = 0, height2 = 0;
		while (!xRate && !zRate) {
			Sleep(20);
		}
		valueX = (long double) (x / xRate) + (mapX / 2.0);	//-1
		valueZ = (long double) (z / zRate) + (mapZ / 2.0) - 1;		//0
		indexX = (int) valueX;
		indexZ = (int) valueZ;

		if (indexX < 0 || indexX > mapX || indexZ < 0 || indexZ >= mapZ) {
			return ERROR_HEIGHT;
		}

		if (indexX == 0) {
			if (valueZ - indexZ < 0.5) {
				actual = heights[mapX - 1][indexZ - 1];
				height1 = (actual - heights[mapX - 1][indexZ]) * (valueZ - indexZ);
			} else {
				actual = heights[mapX - 1][indexZ];
				height1 = (actual - heights[mapX - 1][indexZ + 1]) * (valueZ - indexZ);
			}

			return (actual - height1) * yRate + y;
		}

		actual = heights[mapX - indexX][indexZ];
		if (valueX == indexX && valueZ == indexZ) {
			return actual * yRate + y;
		}

		if (valueX == indexX || indexX == mapX) {
			height1 = (actual - heights[mapX - indexX][indexZ + 1]) * (valueZ - indexZ);
			return (actual - height1) * yRate + y;
		}

		if (valueZ == indexZ || indexZ == mapZ) {
			height1 = (actual - heights[mapX - indexX - 1][indexZ]) * (valueX - indexX);
			return (actual - height1) * yRate + y;
		}

		if (valueZ - indexZ < 0.5 && valueX - indexX < 0.5) {
			if (indexX > 0) {
				height1 = (actual - heights[mapX - indexX][indexZ + 1]) * (valueZ - indexZ);
			}
			if (indexX + 1 < mapX) {
				height2 = (actual - heights[mapX - indexX - 1][indexZ]) * (valueX - indexX);
			}
			return (actual - height1 - height2) * yRate + y;
		}

		else {
			actual = heights[mapX - indexX - 1][indexZ + 1];
			if (indexZ + 1 < mapZ) {
				height1 = (actual - heights[mapX - indexX - 1][indexZ]) * (valueX - indexX);
				height2 = (actual - heights[mapX - indexX][indexZ + 1]) * (valueZ - indexZ);
			}
			return (actual - height1 - height2) * yRate + y;
		}

		return ERROR_HEIGHT;
	}

};
int** Map::heights;
int Map::mapX = 0;
int Map::mapZ = 0;

#endif /* SRC_MAP_H_ */
