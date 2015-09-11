/*
 * objects_loader.h
 *
 *  Created on: 23 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_OBJECTS_LOADER_H_
#define SRC_OBJECTS_LOADER_H_
using namespace rapidxml;
class ObjectsLoader {
private:
	static ObjectsLoader* instance;
	static ThreadWorker* worker;
	static Map* mapBuilder;
	ObjectsLoader() {

	}

	static char* fileToChar(string nazwa_pliku) {
		ifstream plik(nazwa_pliku, ios::binary);
		if (!plik.is_open() || plik.bad()) {
			Logger::log(Logger::ERR + "brak pliku z obiektami");
			exit(-1);
		}

		filebuf * pbuf = plik.rdbuf();
		long wielkosc_pliku = pbuf->pubseekoff(0, ios::end, ios::in);
		pbuf->pubseekpos(0, plik.in);
		char * wyjscie = new char[wielkosc_pliku + 1];
		pbuf->sgetn(wyjscie, wielkosc_pliku);
		wyjscie[wielkosc_pliku] = 0;
		return wyjscie;
	}

	static void loadThread(void *arg) {
		string* path = static_cast<string*>(arg);
		Logger::log("£adujê plik: " + *path, true);
		string stringValue;
		char* fileContent = fileToChar(*path);
		delete path;
		unsigned long loadTime = time(0);
		xml_document<> document;
		try {
			document.parse<0>(fileContent);
		} catch (parse_error &p) {
			ostringstream str;
			str << Logger::ERR << p.what() << ", " << p.where<char**>();
			Logger::log(str.str());
			exit(0);
		}

		xml_node<> * root = document.first_node();
		for (xml_node<> * node = root->first_node(); node; node = node->next_sibling()) {
			stringValue = node->name();
			if (stringValue == "Map") {
				worker->loadMap(node);
			} else {
				worker->loadEntity(node);
			}
		}
		worker->finish();
		delete worker;
		SDL_Quit();

		ostringstream stream;
		Logger::log(Logger::LINE);
		stream << "Utworzono " << totalVerticesCount << " trojkatow";
		Logger::log(stream.str());
		stream.str("");
		stream << "Wczytanych obiektow: " << Object::objectsCount() << ", wyswietlonych obiektow:" << Entity::allEntitiesCount();
		Logger::log(stream.str());

		int residentTexturesCount = 0;
		unsigned texturesCount = Texture::getTexturesCount();
		GLboolean *residentArray = new GLboolean[texturesCount];
		glAreTexturesResident(texturesCount, &Texture::getTextureIds()[0], residentArray);

		for (unsigned i = 0; i < texturesCount; i++) {
			if (residentArray[i]) {
				residentTexturesCount++;
			}
		}
		delete[] residentArray;

		stream.str("");
		stream << "Tekstur: " << texturesCount << ", rezydentne: " << residentTexturesCount << endl;
		Logger::log(stream.str());
		Logger::log("Zakoñczono wczytywanie w " + to_string(time(0) - loadTime) + " s.");
	}

public:
	static ObjectsLoader* getInstance() {
		if (!instance) {
			instance = new ObjectsLoader();
		}
		return instance;
	}

	void loadObjects(string path) {
		worker = ThreadWorker::getInstance();
		worker->setThreadsCount(2);
		string* str = new string(path);
		void* args = static_cast<void*>(str);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) loadThread, (void*) args, 0, NULL);
	}

};
ObjectsLoader* ObjectsLoader::instance = NULL;
ThreadWorker* ObjectsLoader::worker;
Map* ObjectsLoader::mapBuilder;
#endif /* SRC_OBJECTS_LOADER_H_ */
