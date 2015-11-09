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
	HANDLE thread;
	ObjectsLoader() {

	}

	static char* fileToChar(string fileName) {
		ifstream file(fileName, ios::binary);
		if (!file.is_open() || file.bad()) {
			Logger::log(Logger::ERR + "file with scene not found");
			_Exit(0);
		}

		filebuf * pbuf = file.rdbuf();
		long fileSize = pbuf->pubseekoff(0, ios::end, ios::in);
		pbuf->pubseekpos(0, file.in);
		char * result = new char[fileSize + 1];
		pbuf->sgetn(result, fileSize);
		result[fileSize] = 0;
		return result;
	}

	static void loadThread(void *arg) {
		string* path = static_cast<string*>(arg);
		Logger::log("Loading file: " + *path, true);
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
			} else if (stringValue == "Object") {
				worker->loadEntity(node);
			}
		}
		worker->finish();
		delete worker;
		SDL_Quit();

		ostringstream stream;
		Logger::log(Logger::LINE);
		stream << "Created " << totalVerticesCount << " triangles";
		Logger::log(stream.str());
		stream.str("");
		stream << "Loaded objects: " << Object::objectsCount() << ", created entities: " << Entity::entitiesCount;
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
		stream << "Textures: " << texturesCount << ", resident: " << residentTexturesCount << endl;
		Logger::log(stream.str());
		Logger::log("Loading finished in " + to_string(time(0) - loadTime) + " s.");
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
		thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) (loadThread), (void*) (args), 0, NULL);
	}

	void terminate() {
		TerminateThread(thread, 0);
		worker->terminate();
	}

};
ObjectsLoader* ObjectsLoader::instance = NULL;
ThreadWorker* ObjectsLoader::worker;
Map* ObjectsLoader::mapBuilder;
#endif /* SRC_OBJECTS_LOADER_H_ */
