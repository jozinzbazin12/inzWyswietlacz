/*
 * material_lib.h
 *
 *  Created on: 8 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_MATERIAL_LIB_H_
#define SRC_MATERIAL_LIB_H_

class MaterialLib {
public:
	Material **mtl;
	int whichMaterial;
	string path;

	int searchMaterial(string name) {
		for (int i = 0; i <= this->whichMaterial; i++)
			if (mtl[i]->imie == name)
				return i;
		exit(0);
	}

	void loadMtl(string name) {
		float tab[3];
		float floatValue;
		string text;
		ifstream file;
		int textureNumber;

		file.open(name.c_str());
		if (!file.is_open()) {
			Logger::log(Logger::ERR + "brak mtllib");
			exit(0);
		}

		while (!file.eof()) {
			file >> text;

			if (text == "newmtl") {
				file >> text;
				mtl[++this->whichMaterial] = new Material(text);
			}
			if (text == "Ns") {
				file >> text;
				floatValue = atof(text.c_str()) * 128.0 / 1000.0;
				mtl[this->whichMaterial]->setNst(floatValue + 1);
			}

			if (text == "Ka") {
				file >> text;
				tab[0] = atof(text.c_str());
				file >> text;
				tab[1] = atof(text.c_str());
				file >> text;
				tab[2] = atof(text.c_str());
				mtl[this->whichMaterial]->setAmbient(tab);
			}

			if (text == "Kd") {
				file >> text;
				tab[0] = atof(text.c_str());
				file >> text;
				tab[1] = atof(text.c_str());
				file >> text;
				tab[2] = atof(text.c_str());
				mtl[this->whichMaterial]->setDiffuse(tab);
			}

			if (text == "Ks") {
				file >> text;
				tab[0] = atof(text.c_str());
				file >> text;
				tab[1] = atof(text.c_str());
				file >> text;
				tab[2] = atof(text.c_str());
				mtl[this->whichMaterial]->setSpecular(tab);
			}

			if (text == "d") {
				file >> text;
				floatValue = atof(text.c_str());
				mtl[this->whichMaterial]->setD(floatValue);
			}

			if (text == "map_Kd") {
				file >> text;
				string textureName = utnij(path) + "/";
				textureNumber = Texture::isTextureAlreadyDefined(textureName + text);
				if (textureNumber == -1) {
					tekstury[iletekstur++] = new Texture(textureName + text, "map_Kd");
					mtl[this->whichMaterial]->setMapKd(iletekstur - 1);
				} else
					mtl[this->whichMaterial]->setMapKd(textureNumber);

			}
		}
		ostringstream stream;
		stream << "Utworzono " << whichMaterial + 1 << " materialow";
		Logger::log(stream.str());
	}

	MaterialLib(string nazwa) {
		Logger::log("-MTL Lib: " + nazwa);
		path = nazwa;
		whichMaterial = -1;
		mtl = new Material*[ilemtl];
		loadMtl(nazwa);
	}
	~MaterialLib() {
		for (int i = 0; i < this->whichMaterial; i++)
			delete mtl;
	}
};

#endif /* SRC_MATERIAL_LIB_H_ */
