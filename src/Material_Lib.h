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
	vector<Material*> mtl;
	string path;

	int searchMaterial(string name) {
		for (unsigned i = 0; i < mtl.size(); i++)
			if (mtl[i]->name == name)
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

		Material *material = NULL;
		while (!file.eof()) {
			file >> text;
			if (text == "newmtl") {
				if (material) {
					mtl.push_back(material);
				}
				file >> text;
				material = new Material(text);
			}
			if (text == "Ns") {
				file >> text;
				floatValue = atof(text.c_str()) * 128.0 / 1000.0;
				material->setNst(floatValue + 1);
			}

			if (text == "Ka") {
				file >> text;
				tab[0] = atof(text.c_str());
				file >> text;
				tab[1] = atof(text.c_str());
				file >> text;
				tab[2] = atof(text.c_str());
				material->setAmbient(tab);
			}

			if (text == "Kd") {
				file >> text;
				tab[0] = atof(text.c_str());
				file >> text;
				tab[1] = atof(text.c_str());
				file >> text;
				tab[2] = atof(text.c_str());
				material->setDiffuse(tab);
			}

			if (text == "Ks") {
				file >> text;
				tab[0] = atof(text.c_str());
				file >> text;
				tab[1] = atof(text.c_str());
				file >> text;
				tab[2] = atof(text.c_str());
				material->setSpecular(tab);
			}

			if (text == "d") {
				file >> text;
				floatValue = atof(text.c_str());
				material->setD(floatValue);
			}

			if (text == "map_Kd") {
				file >> text;
				string textureName = utnij(path) + "/";
				textureNumber = Texture::isTextureAlreadyDefined(textureName + text);
				if (textureNumber == -1) {
					Texture::textures.push_back(new Texture(textureName + text, "map_Kd"));
					material->setMapKd(Texture::textures.size() - 1);
				} else
					material->setMapKd(textureNumber);
			}
		}
		mtl.push_back(material);
		ostringstream stream;
		stream << "Utworzono " << mtl.size() << " materialow";
		Logger::log(stream.str());
	}

	MaterialLib(string nazwa) {
		Logger::log("-MTL Lib: " + nazwa);
		path = nazwa;
		loadMtl(nazwa);
	}
	~MaterialLib() {
		for (unsigned i = 0; i < mtl.size(); i++)
			delete mtl[i];
	}
};
#endif /* SRC_MATERIAL_LIB_H_ */
