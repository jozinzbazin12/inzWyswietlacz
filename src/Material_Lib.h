/*
 * material_lib.h
 *
 *  Created on: 8 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_MATERIAL_LIB_H_
#define SRC_MATERIAL_LIB_H_

class MaterialLib {
private:
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
					mtl[material->name] = material;
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
		mtl[material->name] = material;
		ostringstream stream;
		stream << "Utworzono " << mtl.size() << " materialow";
		Logger::log(stream.str());
	}

public:
	static vector<MaterialLib*> materials;
	map<string, Material*> mtl;
	string path;

	Material* searchMaterial(string name) {
		Material* material = mtl[name];
		if (!material) {
			Logger::log(Logger::ERR + "nie znaleziono materia³u: " + name);
			exit(0);
		}
		return material;
	}

	MaterialLib(string nazwa) {
		Logger::log("-MTL Lib: " + nazwa);
		path = nazwa;
		loadMtl(nazwa);
	}
	~MaterialLib() {
//	for (unsigned i = 0; i < mtl.size(); i++)
//		delete mtl[i];
	}
};
vector<MaterialLib*> MaterialLib::materials;
#endif /* SRC_MATERIAL_LIB_H_ */
