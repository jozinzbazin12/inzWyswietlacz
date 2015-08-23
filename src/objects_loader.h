/*
 * objects_loader.h
 *
 *  Created on: 23 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_OBJECTS_LOADER_H_
#define SRC_OBJECTS_LOADER_H_
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
using namespace rapidxml;
class ObjectsLoader {
private:
	static ObjectsLoader* instance;
	Map* mapBuilder;

	ObjectsLoader() {

	}

	char* fileToChar(const char * nazwa_pliku) {
		ifstream plik(nazwa_pliku);
		if (plik.bad()) { // (3)
			Logger::log(Logger::ERR + "brak pliku z obiektami");
			exit(-1);
		}

		filebuf * pbuf = plik.rdbuf();
		long wielkosc_pliku = pbuf->pubseekoff(0, ios::end, ios::in);
		plik.seekg(0);
		char * wyjscie = new char[wielkosc_pliku + 1];
		plik.read(wyjscie, wielkosc_pliku);
		wyjscie[wielkosc_pliku] = 0;
		return wyjscie;
	}

public:
	static ObjectsLoader* getInstance() {
		if (!instance) {
			instance = new ObjectsLoader();
		}
		return instance;
	}

	void loadObjects(const char* path) {
		GLfloat a, b, c, d;
		string stringValue;
		char* fileContent = fileToChar(path);
		unsigned long loadTime = time(0);
		//Logger::log(fileContent);
		xml_document<> document;
		try {
			document.parse<0>(fileContent);
		} catch (parse_error &p) {
			Logger::log(Logger::ERR + p.what());
			exit(0);
		}

		xml_node<> * root = document.first_node();

		for (xml_node<> * node = root->first_node(); node; node = node->next_sibling()) {
			stringValue = node->name();
			if (stringValue == "Map") { //todo ladowac info
				xml_node<>* settings = node->first_node();
				stringValue = node->first_attribute("mapFile")->value();
				mapBuilder = new Map();
				a = stod(settings->first_node("scaleX")->value());
				b = stod(settings->first_node("scaleY")->value());
				c = stod(settings->first_node("scaleZ")->value());
				mapBuilder->wymx = a; //todo
				mapBuilder->stosuneky = b;
				mapBuilder->wymz = c;
				mapBuilder->createMap(stringValue, "mapy/tekstury/tex.png", "mapy/mtl/mtl.mtl");
				Object::addObject(mapBuilder->mapObject);
				Entity* mapObject = new Entity(mapBuilder->mapObject);
				Entity::allObjects.push_back(mapObject);
				mapObject->alwaysDisplay = true;
				mapObject->setScale(mapBuilder->stosunekx, mapBuilder->stosuneky, mapBuilder->stosunekz);
			} else {
				string objectName = node->first_attribute("objectFile")->value();
				Object* object = Object::getObject(objectName);
				if (object == NULL) {
					object = new Object(objectName);
					Object::addObject(object);
				}
				Entity* entity = new Entity(object);
				xml_node<>* settings = node->first_node();
				stringValue = settings->first_node("relative")->value();
				a = stod(settings->first_node("posX")->value());
				b = stod(settings->first_node("posY")->value());
				c = stod(settings->first_node("posZ")->value());
				if (stringValue == "false") {
					entity->setPosition(a, d, c);
				} else {
					d = mapBuilder->calculateHeight(a, b, c);
					entity->setPosition(a, d, c);
				}

				a = stod(settings->first_node("scaleX")->value());
				b = stod(settings->first_node("scaleY")->value());
				c = stod(settings->first_node("scaleZ")->value());
				entity->setScale(a, b, c);

				a = stod(settings->first_node("rotationX")->value());
				b = stod(settings->first_node("rotationY")->value());
				c = stod(settings->first_node("rotationZ")->value());
				entity->setRotation(a, b, c);
				Entity::allObjects.push_back(entity);
			}
		}

		SDL_Quit();

		ostringstream stream;
		Logger::log(Logger::LINE);
		stream << "Utworzono " << totalVerticesCount << " trojkatow";
		Logger::log(stream.str());
		stream.str("");
		stream << "Wczytanych obiektow: " << Object::objectsCount() << ", wyswietlonych obiektow:"
				<< Entity::allObjects.size();
		Logger::log(stream.str());

		int residentTexturesCount = 0;
		unsigned texturesCount = Texture::textures.size();
		GLboolean *residentArray = new GLboolean[texturesCount];
		glAreTexturesResident(texturesCount, &Texture::txtid[0], residentArray);

		for (unsigned i = 0; i < texturesCount; i++)
			if (residentArray[i]) {
				residentTexturesCount++;
			}
		delete[] residentArray;

		stream.str("");
		stream << "Tekstur: " << texturesCount << ", rezydentne: " << residentTexturesCount << endl;
		Logger::log(stream.str());
		Logger::log("Zakoñczono wczytywanie w " + to_string(time(0) - loadTime) + " s.");
	}

};
ObjectsLoader* ObjectsLoader::instance = NULL;
#endif /* SRC_OBJECTS_LOADER_H_ */
