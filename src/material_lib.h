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
	static HANDLE mutex;
	string prepareString(string str) {
		return "[" + mtlName + "] " + str;
	}
	void loadMtl(string name) {
		float tab[3];
		float floatValue;
		string text;
		ifstream file;

		file.open(name.c_str());
		if (!file.is_open()) {
			Logger::log(Logger::ERR + "no mtllib");
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
				string textureName = getRealPath(text, path);
				Texture* txt = Texture::getTexture(textureName, "map_Kd");
				material->setMapKd(txt);
			}
		}
		mtl[material->name] = material;
		ostringstream stream;
		stream << "Created " << mtl.size() << " materials";
		Logger::log(prepareString(stream.str()));
	}

public:
	static vector<MaterialLib*> materials;
	map<string, Material*> mtl;
	string mtlName;
	string path;

	Material* searchMaterial(string name) {
		Material* material = mtl[name];
		if (!material) {
			Logger::log(prepareString(Logger::ERR + "material not found: " + name));
			exit(0);
		}
		return material;
	}

	MaterialLib(string path) {
		Logger::log("MTL Lib: " + path);
		this->mtlName = path;
		this->path = getPath(path);
		loadMtl(path);
		WaitForSingleObject(mutex, INFINITE);
		materials.push_back(this);
		ReleaseMutex(mutex);
	}
};
vector<MaterialLib*> MaterialLib::materials;
HANDLE MaterialLib::mutex = CreateMutex(NULL, false, NULL);
#endif /* SRC_MATERIAL_LIB_H_ */
